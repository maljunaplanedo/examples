#include <memory>
#include <typeinfo>
#include <type_traits>
#include <cassert>

namespace Helpers {

    template<typename T, typename Y>
    T ConditionalDynamicCast(Y pointer) {
        if constexpr(std::is_same_v<T, Y>) {
            return pointer;
        } else {
            if (pointer == nullptr)
                return nullptr;
            return dynamic_cast<T>(pointer);
        }
    }

    template<typename T, typename Alloc>
    void autoDeallocHelper(T* pointer, Alloc&& alloc) {
        using AllocTraits = std::allocator_traits<Alloc>;

        using TAlloc = typename AllocTraits::template rebind_alloc<T>;
        using TAllocTraits = std::allocator_traits<TAlloc>;

        auto tAlloc = static_cast<TAlloc>(alloc);

        TAllocTraits::deallocate(tAlloc, pointer, 1);
    }

    class ControlBlockBase {
    private:
        int sharedCount = 0;
        int weakCount = 0;

        void checkAllDestroy() {
            if (sharedCount == 0) {
                sharedCount = -1;
                destroyObject();
                sharedCount = 0;
                checkSelfDestroy();
            }
        }

        void checkSelfDestroy() {
            if (sharedCount == 0 && weakCount == 0) {
                destroySelf();
            }
        }

    protected:

        virtual void destroyObject() = 0;
        virtual void destroySelf() = 0;

    public:

        ControlBlockBase() = default;

        void addShared() {
            ++sharedCount;
        }

        void addWeak() {
            ++weakCount;
        }

        void removeShared() {
            --sharedCount;
            checkAllDestroy();
        }

        void removeWeak() {
            --weakCount;
            checkSelfDestroy();
        }

        [[nodiscard]] size_t getSharedCount() const {
            return sharedCount;
        }

        virtual void* getPointer() = 0;

        virtual ~ControlBlockBase() = default;
    };


    template<typename T, typename Alloc>
    class ControlBlockCombined: public ControlBlockBase {
    private:
        Alloc alloc;
        char buffer[sizeof(T)]{};

        using AllocTraits = std::allocator_traits<Alloc>;

        using ThisAlloc = typename AllocTraits::template rebind_alloc<ControlBlockCombined>;

        void destroyObject() override {
            AllocTraits::destroy(alloc, getTPointer());
        }

        void destroySelf() override {
            autoDeallocHelper(this, std::move(alloc));
        }

        T* getTPointer() {
            return static_cast<T*>(getPointer());
        }

    public:
        template<typename... Args>
        ControlBlockCombined(const Alloc& alloc, Args&&... args)
        : ControlBlockBase(), alloc(alloc) {
            AllocTraits::construct(this->alloc, getTPointer(), std::forward<Args>(args)...);
        }

        void* getPointer() override {
            return buffer;
        }

        ~ControlBlockCombined() override = default;

    };

    struct DeleteByAllocTag{};

    template<typename T, typename Deleter, typename Alloc>
    class ControlBlockSplit: public ControlBlockBase {
    private:

        using AllocTraits = std::allocator_traits<Alloc>;

        using ThisAlloc = typename AllocTraits::template rebind_alloc<ControlBlockSplit>;

        Deleter deleter;
        Alloc alloc;
        T* pointer;

        void destroyObject() override {
            if (pointer == nullptr)
                return;
            if constexpr(std::is_same_v<Deleter, DeleteByAllocTag>) {
                AllocTraits::destroy(alloc, pointer);
                AllocTraits::deallocate(alloc, pointer, 1);
            } else {
                deleter(pointer);
            }
            pointer = nullptr;
        }

        void destroySelf() override {
            deleter.~Deleter();
            autoDeallocHelper(this, std::move(alloc));
        }

    public:
        ControlBlockSplit(Deleter deleter, const Alloc& alloc, T* pointer)
        : ControlBlockBase(), deleter(deleter), alloc(alloc), pointer(pointer)
        {   }

        void* getPointer() override {
            return pointer;
        }

        ~ControlBlockSplit() override = default;
    };
}


template<typename T>
class SharedPtr {

private:
    Helpers::ControlBlockBase* controlBlock = nullptr;
    T* pointer = nullptr;

    struct ConstructByAllocateShared{};
    struct ConstructByWeakPtr{};

    template<typename U, typename Alloc, typename... Args>
    friend SharedPtr<U> allocateShared(const Alloc& alloc, Args&&... args);

    template<typename Y>
    friend class SharedPtr;

    template<typename Y>
    friend class WeakPtr;

    SharedPtr(ConstructByWeakPtr, Helpers::ControlBlockBase* controlBlock, T* pointer)
    :controlBlock(controlBlock), pointer(pointer) {
        if (controlBlock != nullptr)
            controlBlock->addShared();
    }

    template<typename Alloc, typename... Args>
    SharedPtr(ConstructByAllocateShared, const Alloc& alloc, Args&&... args) {

        using AllocTraits = std::allocator_traits<Alloc>;
        using ControlBlockAlloc = typename AllocTraits::
                template rebind_alloc<Helpers::ControlBlockCombined<T, Alloc>>;
        using ControlBlockAllocTraits = std::allocator_traits<ControlBlockAlloc>;

        auto controlBlockAlloc = static_cast<ControlBlockAlloc>(alloc);

        Helpers::ControlBlockCombined<T, Alloc>* controlBlockChild =
                ControlBlockAllocTraits::allocate(controlBlockAlloc, 1);
        controlBlock = dynamic_cast<Helpers::ControlBlockBase*>(controlBlockChild);

        new (controlBlockChild) Helpers::ControlBlockCombined<T, Alloc>(alloc, std::forward<Args>(args)...);

        pointer = static_cast<T*>(controlBlock->getPointer());
        controlBlock->addShared();
    }

    void disconnectCurrentObject() {
        if (controlBlock != nullptr)
            controlBlock->removeShared();
        controlBlock = nullptr;
        pointer = nullptr;
    }

public:

    template<typename Y, typename Deleter, typename Alloc>
    SharedPtr(Y* pointer_, Deleter deleter, Alloc alloc)
                : pointer(Helpers::ConditionalDynamicCast<T*>(pointer_)) {

        if (pointer_ == nullptr) {
            controlBlock = nullptr;
            return;
        }

        using AllocTraits = std::allocator_traits<Alloc>;

        using YAlloc = typename AllocTraits::template rebind_alloc<Y>;

        using ControlBlockAlloc = typename AllocTraits::
                template rebind_alloc<Helpers::ControlBlockSplit<Y, Deleter, YAlloc>>;
        using ControlBlockAllocTraits = std::allocator_traits<ControlBlockAlloc>;

        auto yAlloc = static_cast<YAlloc>(alloc);
        auto controlBlockAlloc = static_cast<ControlBlockAlloc>(alloc);

        Helpers::ControlBlockSplit<Y, Deleter, YAlloc>* controlBlockChild =
                ControlBlockAllocTraits::allocate(controlBlockAlloc, 1);
        controlBlock = dynamic_cast<Helpers::ControlBlockBase*>(controlBlockChild);

        new (controlBlockChild) Helpers::ControlBlockSplit<Y, Deleter, YAlloc>(deleter, yAlloc, pointer_);

        controlBlock->addShared();
    }

    template<typename Deleter, typename Alloc>
    SharedPtr(std::nullptr_t, Deleter deleter, Alloc alloc)
            :SharedPtr()
    {   }

    template<typename Y, typename Deleter>
    SharedPtr(Y* pointer, Deleter deleter)
        : SharedPtr(pointer, deleter, std::allocator<T>())
    {   }

    template<typename Deleter>
    SharedPtr(std::nullptr_t, Deleter deleter)
            : SharedPtr()
    {   }

    template<typename Y>
    SharedPtr(Y* pointer)
        : SharedPtr(pointer, Helpers::DeleteByAllocTag())
    {   }

    SharedPtr(std::nullptr_t)
        : SharedPtr()
    {   }

    SharedPtr() = default;

    SharedPtr(const SharedPtr& other)
    :controlBlock(other.controlBlock), pointer(other.pointer) {
        if (controlBlock != nullptr)
            controlBlock->addShared();
    }

    template<typename Y>
    SharedPtr(const SharedPtr<Y>& other)
    :controlBlock(other.controlBlock), pointer(Helpers::ConditionalDynamicCast<T*>(other.pointer)) {
        if (controlBlock != nullptr)
            controlBlock->addShared();
    }

    SharedPtr(SharedPtr&& other)
    :controlBlock(other.controlBlock), pointer(other.pointer) {

        other.controlBlock = nullptr;
        other.pointer = nullptr;
    }

    template<typename Y>
    SharedPtr(SharedPtr<Y>&& other)
    :controlBlock(other.controlBlock), pointer(Helpers::ConditionalDynamicCast<T*>(other.pointer)) {

        other.controlBlock = nullptr;
        other.pointer = nullptr;
    }

    void swap(SharedPtr& other) {
        std::swap(controlBlock, other.controlBlock);
        std::swap(pointer, other.pointer);
    }

    SharedPtr& operator=(const SharedPtr& other) {
        return operator= <>(other);
    }

    template<typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) {
        SharedPtr copy(other);
        swap(copy);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        return operator= <>(std::move(other));
    }

    template<typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) {
        disconnectCurrentObject();
        controlBlock = other.controlBlock;
        pointer = Helpers::ConditionalDynamicCast<T*>(other.pointer);
        other.controlBlock = nullptr;
        other.pointer = nullptr;
        return *this;
    }

    ~SharedPtr() {
        disconnectCurrentObject();
    }

    [[nodiscard]] size_t use_count() const {
        if (controlBlock == nullptr)
            return 0;
        return controlBlock->getSharedCount();
    }

    void reset(std::nullptr_t = nullptr) {
        *this = std::move(SharedPtr<T>());
    }

    template<typename Y>
    void reset(Y* newPointer) {
        *this = std::move(SharedPtr<Y>(newPointer));
    }

    T* get() const {
        return pointer;
    }

    T& operator*() const {
        return *pointer;
    }

    T* operator->() const {
        return pointer;
    }

    explicit operator bool() {
        return pointer != nullptr;
    }

};


template<typename T>
class WeakPtr {
private:

    Helpers::ControlBlockBase* controlBlock = nullptr;
    T* pointer = nullptr;

    void disconnectCurrentObject() {
        if (controlBlock != nullptr)
            controlBlock->removeWeak();
        controlBlock = nullptr;
        pointer = nullptr;
    }

    template<typename Y>
    friend class WeakPtr;

public:

    WeakPtr() = default;

    template<typename Y>
    WeakPtr(const SharedPtr<Y>& sharedPtr)
    :controlBlock(sharedPtr.controlBlock), pointer(Helpers::ConditionalDynamicCast<T*>(sharedPtr.pointer)) {

        if (controlBlock != nullptr)
            controlBlock->addWeak();
    }

    WeakPtr(const WeakPtr& other)
            :controlBlock(other.controlBlock), pointer(other.pointer) {

        if (controlBlock != nullptr)
            controlBlock->addWeak();
    }

    template<typename Y>
    WeakPtr(const WeakPtr<Y>& other)
            :controlBlock(other.controlBlock), pointer(Helpers::ConditionalDynamicCast<T*>(other.pointer)) {

        if (controlBlock != nullptr)
            controlBlock->addWeak();
    }

    WeakPtr(WeakPtr&& other)
    :controlBlock(other.controlBlock), pointer(other.pointer) {

        other.controlBlock = nullptr;
        other.pointer = nullptr;
    }

    template<typename Y>
    WeakPtr(WeakPtr<Y>&& other)
    :controlBlock(other.controlBlock), pointer(Helpers::ConditionalDynamicCast<T*>(other.pointer)) {

        other.controlBlock = nullptr;
        other.pointer = nullptr;
    }

    void swap(WeakPtr& other) {
        std::swap(controlBlock, other.controlBlock);
        std::swap(pointer, other.pointer);
    }

    WeakPtr& operator=(const WeakPtr& other) {
        return operator= <>(other);
    }

    template<typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) {
        WeakPtr copy(other);
        swap(copy);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        return operator= <>(std::move(other));
    }

    template<typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) {
        disconnectCurrentObject();
        controlBlock = other.controlBlock;
        pointer = Helpers::ConditionalDynamicCast<T*>(other.pointer);
        other.controlBlock = nullptr;
        other.pointer = nullptr;
        return *this;
    }

    ~WeakPtr() {
        disconnectCurrentObject();
    }

    [[nodiscard]] bool expired() const {
        if (controlBlock == nullptr)
            return true;
        return controlBlock->getSharedCount() == 0;
    }

    SharedPtr<T> lock() const {
        return SharedPtr<T>(typename SharedPtr<T>::ConstructByWeakPtr(), controlBlock, pointer);
    }

    [[nodiscard]] size_t use_count() const {
        if (controlBlock == nullptr)
            return 0;
        return controlBlock->getSharedCount();
    }


};


template<typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
    return SharedPtr<T>(typename SharedPtr<T>::ConstructByAllocateShared(), alloc, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}