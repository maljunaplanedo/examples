#pragma once

///////////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <list>


template<typename T, typename Allocator = std::allocator<T>>
class List {
private:
    using AllocTraits = std::allocator_traits<Allocator>;

public:
    struct Node {
        Allocator alloc;

        T* value = nullptr;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node() = default;

        void allocateValue() {
            value = AllocTraits::allocate(this->alloc, 1);
        }

        explicit Node(const Allocator& alloc, const T& element)
                :alloc(alloc) {

            allocateValue();
            AllocTraits::construct(this->alloc, value, element);
        }

        explicit Node(const Allocator& alloc, T&& element):
            alloc(alloc) {

            allocateValue();
            AllocTraits::construct(this->alloc, value, std::move(element));
        }

        template<typename... Args>
        explicit Node(const Allocator& alloc, Args&&... args)
            :alloc(alloc) {

            allocateValue();
            AllocTraits::construct(this->alloc, value, std::forward<Args>(args)...);
        }

        ~Node() {
            AllocTraits::destroy(alloc, value);
            AllocTraits::deallocate(alloc, value, 1);
        }

    };

private:
    using NodeAlloc = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

    NodeAlloc alloc_;

    Node* firstNode_ = nullptr;
    Node* lastNode_ = nullptr;

    size_t length_ = 0;

    static void bind(Node* first, Node* second) {
        if (first != nullptr)
            first->next = second;
        if (second != nullptr)
            second->prev = first;
    }

    void insertAfter(Node* node, Node* newNode) {
        if (node != nullptr) {
            bind(newNode, node->next);
            bind(node, newNode);
        } else {
            bind(newNode, firstNode_);
        }

        if (newNode->prev == nullptr)
            firstNode_ = newNode;
        if (newNode->next == nullptr)
            lastNode_ = newNode;

        ++length_;
    }

public:
    Node* make(const T& value) {
        Node* newNode = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, newNode, static_cast<Allocator>(alloc_), value);
        return newNode;
    }

    Node* make(T&& value) {
        Node* newNode = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, newNode, static_cast<Allocator>(alloc_), std::move(value));
        return newNode;
    }

    template<typename... Args>
    Node* make(Args&&... args) {
        Node* newNode = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, newNode, static_cast<Allocator>(alloc_), std::forward<Args>(args)...);
        return newNode;
    }

    void destroyNode(Node* node) {
        NodeAllocTraits::destroy(alloc_, node);
        NodeAllocTraits::deallocate(alloc_, node, 1);
    }

    Node* exclude(Node* node) {
        --length_;

        if (node == firstNode_)
            firstNode_ = node->next;
        if (node == lastNode_)
            lastNode_ = node->prev;

        bind(node->prev, node->next);

        return node->next;
    }

private:
    Node* makeAndInsertAfter(Node* node, const T& value) {
        Node* newNode = make(value);
        insertAfter(node, newNode);
        return newNode;
    }

    Node* makeAndInsertAfter(Node* node, T&& value) {
        Node* newNode = make(std::move(value));
        insertAfter(node, newNode);
        return newNode;
    }

    template<typename... Args>
    Node* makeAndInsertAfter(Node* node, Args&&... args) {
        Node* newNode = make(std::forward<Args>(args)...);
        insertAfter(node, newNode);
        return newNode;
    }

    Node* excludeAndDestroy(Node* node) {
        Node* returnValue = exclude(node);
        destroyNode(node);
        return returnValue;
    }

public:
    template<bool isConst>
    class ProtoIterator {
        friend class List;

        template<bool isOtherConst>
        friend class ProtoIterator;

    private:
        using ValueT = std::conditional_t<isConst, const T, T>;

        Node* node_;
        const List* container_;

        ProtoIterator(Node* node, const List* container)
                : node_(node), container_(container)
        {   }

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = ValueT;
        using pointer = ValueT*;
        using reference = ValueT&;
        using iterator_category = std::bidirectional_iterator_tag;

        Node* getNode() const {
            return node_;
        }

        ProtoIterator()
                :ProtoIterator(nullptr, nullptr)
        {   }

        ProtoIterator(const ProtoIterator& other)
                : node_(other.node_), container_(other.container_) {  }

        void swap(ProtoIterator& other) {
            std::swap(node_, other.node_);
            std::swap(container_, other.container_);
        }

        ProtoIterator& operator=
                (const ProtoIterator& other) {
            ProtoIterator copy(other);
            swap(copy);
            return *this;
        }

        operator ProtoIterator<true>() const {
            return ProtoIterator<true>(node_, container_);
        }

        ~ProtoIterator() = default;

        reference operator*() const {
            return *node_->value;
        }

        pointer operator->() const {
            return &**this;
        };

        ProtoIterator& operator++() {
            if (node_ == nullptr)
                node_ = container_->firstNode_;
            else
                node_ = node_->next;
            return *this;
        }

        ProtoIterator& operator--() {
            if (node_ == nullptr)
                node_ = container_->lastNode_;
            else
                node_ = node_->prev;
            return *this;
        }

        ProtoIterator operator++(int) {
            ProtoIterator copy(*this);
            ++*this;
            return copy;
        }

        ProtoIterator operator--(int) {
            ProtoIterator copy(*this);
            --*this;
            return copy;
        }

        template<bool isOtherConst>
        bool operator==(const ProtoIterator<isOtherConst>& other) {
            return node_ == other.node_;
        }

        template<bool isOtherConst>
        bool operator!=(const ProtoIterator<isOtherConst>& other) {
            return !(*this == other);
        }
    };

    void total_swap(List& other) {
        std::swap(firstNode_, other.firstNode_);
        std::swap(lastNode_, other.lastNode_);
        std::swap(length_, other.length_);
        std::swap(alloc_, other.alloc_);
    }

public:

    using iterator = ProtoIterator<false>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_iterator = ProtoIterator<true>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit List(const Allocator& alloc = Allocator())
            : alloc_(static_cast<NodeAlloc>(alloc))
    {   }

    void push_back(const T& value) {
        makeAndInsertAfter(lastNode_, value);
    }

    void push_back(T&& value) {
        makeAndInsertAfter(lastNode_, std::move(value));
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        makeAndInsertAfter(lastNode_, std::forward<Args>(args)...);
    }

    void pop_back() {
        excludeAndDestroy(lastNode_);
    }

    Allocator get_allocator() const {
        return static_cast<Allocator>(alloc_);
    }

    void push_front(const T& value) {
        makeAndInsertAfter(nullptr, value);
    }

    void push_front(T&& value) {
        makeAndInsertAfter(nullptr, std::move(value));
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        makeAndInsertAfter(nullptr, std::forward<Args>(args)...);
    }

    void pop_front() {
        excludeAndDestroy(firstNode_);
    }

    List(size_t count, const Allocator& alloc = Allocator())
            :List(alloc) {

        for (size_t i = 0; i < count; ++i) {
            makeAndInsertAfter(lastNode_);
        }
    }

    List(size_t count, const T& value, const Allocator& alloc = Allocator())
            :List(alloc) {
        for (size_t i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    [[nodiscard]] size_t size() const {
        return length_;
    }

    [[nodiscard]] bool empty() const {
        return !length_;
    }

    void clear() {
        while (!empty()) {
            pop_back();
        }
    }

    ~List() {
        clear();
    }

    [[nodiscard]] iterator begin() {
        return iterator(firstNode_, this);
    }

    [[nodiscard]] const_iterator cbegin() const {
        return const_iterator(firstNode_, this);
    }

    [[nodiscard]] const_iterator begin() const {
        return cbegin();
    }

    [[nodiscard]] iterator end() {
        return iterator(nullptr, this);
    }

    [[nodiscard]] const_iterator cend() const {
        return const_iterator(nullptr, this);
    }

    [[nodiscard]] const_iterator end() const {
        return cend();
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return crbegin();
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return crend();
    }

    iterator insertNode(const_iterator position, Node* node) {
        --position;
        insertAfter(position.node_, node);
        return iterator(node, this);
    }

    iterator insert(const_iterator position, const T& value) {
        --position;
        return iterator(makeAndInsertAfter(position.node_, value), this);
    }

    iterator insert(const_iterator position, T&& value) {
        --position;
        return iterator(makeAndInsertAfter(position.node_, std::move(value)), this);
    }

    template<typename... Args>
    iterator emplace(const_iterator position, Args&&... args) {
        --position;
        return iterator(makeAndInsertAfter(position.node_,
                                           std::forward<Args>(args)...), this);
    }

    iterator erase(const_iterator position) {
        return iterator(excludeAndDestroy(position.node_), this);
    }

    List(const List& other, const Allocator& alloc)
            : List(alloc) {

        for (const auto& it: other) {
            push_back(it);
        }
    }

    List(const List& other)
            : List(other, AllocTraits::select_on_container_copy_construction(static_cast<Allocator>(other.alloc_)))
    {   }

    List(List&& other) noexcept
            : List(other.alloc_) {

        firstNode_ = other.firstNode_;
        lastNode_ = other.lastNode_;
        length_ = other.length_;

        other.firstNode_ = nullptr;
        other.lastNode_ = nullptr;
        other.length_ = 0;
    }

    List& operator=(const List& other) {
        if (this != &other) {
            clear();
            if (AllocTraits::propagate_on_container_copy_assignment::value)
                alloc_ = other.alloc_;

            for (const auto& it: other) {
                push_back(it);
            }
        }
        return *this;
    }

    List& operator=(List&& other) noexcept {
        firstNode_ = other.firstNode_;
        lastNode_ = other.lastNode_;
        length_ = other.length_;

        other.firstNode_ = nullptr;
        other.lastNode_ = nullptr;
        other.length_ = 0;

        if (AllocTraits::propagate_on_container_move_assignment::value)
            alloc_ = other.alloc_;

        return *this;
    }
};

///////////////////////////////////////////////////////////////

#include <functional>
#include <cmath>


template<typename Key, typename Value,
         typename Hash = std::hash<Key>,
         typename Equal = std::equal_to<Key>,
         typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
private:
    using KeyValPair = std::pair<const Key, Value>;
    using AllocTraits = std::allocator_traits<Alloc>;
    using KeyValList = List<KeyValPair, Alloc>;

public:
    using iterator = typename KeyValList::iterator;
    using const_iterator = typename KeyValList::const_iterator;

private:
    using IteratorAlloc = typename AllocTraits::template rebind_alloc<iterator>;
    using KeyValListNode = typename KeyValList::Node;
    using BucketVector = std::vector<iterator, IteratorAlloc>;

private:
    static const size_t START_BUCKET_COUNT = 4;
    constexpr static double DEFAULT_MAX_LOAD_FACTOR = 1.0;

    double maxLoadFactor_ = DEFAULT_MAX_LOAD_FACTOR;
    size_t bucketCount_;

    Hash hashFunc_;
    Equal equal_;
    Alloc alloc_;

    KeyValList keyValList_;
    BucketVector bucketBegins_;
    BucketVector bucketEnds_;

    [[nodiscard]] size_t getBucket(const Key& key) const {
        return hashFunc_(key) % bucketCount_;
    }

    std::pair<iterator, bool> insertListNode(KeyValListNode* listNode) {
        size_t bucket = getBucket(listNode->value->first);
        auto it = bucketBegins_[bucket];

        if (it != end()) {
            do {
                if (equal_(it->first, listNode->value->first)) {
                    keyValList_.destroyNode(listNode);
                    return std::make_pair(it, false);
                }
            } while (it++ != bucketEnds_[bucket]);
        }

        reserve(size() + 1);

        it = bucketBegins_[bucket];
        auto newIt = keyValList_.insertNode(it, listNode);

        bucketBegins_[bucket] = newIt;
        if (it == end())
            bucketEnds_[bucket] = newIt;
        return std::make_pair(newIt, true);
    }


public:
    UnorderedMap()
            : UnorderedMap(START_BUCKET_COUNT)
    {   }

    explicit UnorderedMap(size_t bucketCount,
            const Hash& hash = Hash(),
            const Equal& equal = Equal(),
            const Alloc& alloc = Alloc())

            : bucketCount_(bucketCount), hashFunc_(hash), equal_(equal),
              alloc_(alloc), keyValList_(alloc),
              bucketBegins_(bucketCount, keyValList_.end(), alloc),
              bucketEnds_(bucketCount, keyValList_.end(), alloc)
    {   }

    UnorderedMap(size_t bucketCount, const Alloc& alloc)
            : UnorderedMap(bucketCount, Hash(), Equal(), alloc)
    {   }

    UnorderedMap(size_t bucketCount, const Hash& hash,
        const Alloc& alloc)
            : UnorderedMap(bucketCount, hash, Equal(), alloc)
    {   }

    explicit UnorderedMap(const Alloc& alloc)
            : UnorderedMap(START_BUCKET_COUNT, alloc)
    {   }

    UnorderedMap(const UnorderedMap& other, const Alloc& alloc)
        : UnorderedMap(alloc) {

        for (const auto& it: other) {
            insert(it);
        }
    }

    UnorderedMap(const UnorderedMap& other)
            : UnorderedMap(other,
                           AllocTraits::select_on_container_copy_construction(other.alloc_))
    {   }

    UnorderedMap(UnorderedMap&& other)
            : maxLoadFactor_(other.maxLoadFactor_),
              bucketCount_(other.bucketCount_),
              hashFunc_(std::move(other.hashFunc_)),
              equal_(std::move(other.equal_)),
              alloc_(other.alloc_),
              keyValList_(std::move(other.keyValList_)),
              bucketBegins_(std::move(other.bucketBegins_)),
              bucketEnds_(std::move(other.bucketEnds_)) {

        other.bucketCount_ = 0;
    }

    ~UnorderedMap() = default;

    UnorderedMap& operator=(const UnorderedMap& other) {
        Alloc newAlloc =
                (AllocTraits::propagate_on_container_copy_assignment::value ?
                 other.alloc_ : alloc_);
        UnorderedMap copy(other, newAlloc);
        total_swap(copy);
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap&& other) noexcept {
        maxLoadFactor_ = other.maxLoadFactor_;
        bucketCount_ = other.bucketCount_;
        hashFunc_ = std::move(other.hashFunc_);
        equal_ = std::move(other.equal_);
        if (AllocTraits::propagate_on_container_move_assignment::value)
            alloc_ = other.alloc_;
        keyValList_ = std::move(other.keyValList_);
        bucketBegins_ = std::move(other.bucketBegins_);
        bucketEnds_ = std::move(other.bucketEnds_);

        return *this;
    }

    void rehash(size_t count) {
        if (bucketCount_ >= count)
            return;

        KeyValList oldKeyValList(alloc_);
        oldKeyValList.total_swap(keyValList_);

        bucketCount_ = count;
        bucketBegins_.assign(count, keyValList_.end());
        bucketEnds_.assign(count, keyValList_.end());

        auto it = oldKeyValList.begin();

        while (!oldKeyValList.empty()) {
            auto nextIt = it;
            ++nextIt;

            KeyValListNode* listNode = it.getNode();
            oldKeyValList.exclude(listNode);
            insertListNode(listNode);

            it = nextIt;
        }
    }

    void reserve(size_t count) {
        size_t neededBucketCount = std::ceil(count / maxLoadFactor_);
        if (bucketCount_ < neededBucketCount)
            rehash(2 * neededBucketCount);
    }

    [[nodiscard]] size_t size() const {
        return keyValList_.size();
    }

    [[nodiscard]] bool empty() const {
        return keyValList_.empty();
    }

    [[nodiscard]] iterator begin() {
        return keyValList_.begin();
    }

    [[nodiscard]] const_iterator begin() const {
        return keyValList_.cbegin();
    }

    [[nodiscard]] const_iterator cbegin() const {
        return keyValList_.cbegin();
    }

    [[nodiscard]] iterator end() {
        return keyValList_.end();
    }

    [[nodiscard]] const_iterator end() const {
        return keyValList_.cend();
    }

    [[nodiscard]] const_iterator cend() const {
        return keyValList_.cend();
    }

    iterator find(const Key& key) {
        size_t bucket = getBucket(key);
        if (bucketBegins_[bucket] == end())
            return end();

        auto it = bucketBegins_[bucket];
        do {
            if (equal_(it->first, key))
                return it;
        } while (it++ != bucketEnds_[bucket]);

        return end();
    }

    const_iterator find(const Key& key) const {
        size_t bucket = getBucket(key);
        if (bucketBegins_[bucket] == cend())
            return cend();

        auto it = bucketBegins_[bucket];
        do {
            if (equal_(it->first, key))
                return it;
        } while (it++ != bucketEnds_[bucket]);

        return cend();
    }

    std::pair<iterator, bool> insert(const KeyValPair& value) {
        KeyValPair copy(value);
        return insert(std::move(copy));
    }

    std::pair<iterator, bool> insert(KeyValPair&& value) {
        KeyValListNode* listNode = keyValList_.make(std::move(value));
        return insertListNode(listNode);
    }

    template<typename P>
    std::pair<iterator, bool> insert(P&& value) {
        return emplace(std::forward<P>(value));
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        KeyValListNode* listNode
            = keyValList_.make(std::forward<Args>(args)...);

        return insertListNode(listNode);
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Value& operator[](const Key& key) {
        return insert(KeyValPair(key, Value())).first->second;
    }

    Value& at(const Key& key) {
        auto it = find(key);
        if (it == end())
            throw std::out_of_range("Key is not in the container_.");
        return it->second;
    }

    const Value& at(const Key& key) const {
        auto it = find(key);
        if (it == end())
            throw std::out_of_range("Key is not in the container_.");
        return it->second;
    }

    iterator erase(const_iterator position) {
        size_t bucket = getBucket(position->first);
        if (bucketBegins_[bucket] == bucketEnds_[bucket])
            bucketBegins_[bucket] = bucketEnds_[bucket] = keyValList_.end();
        else if (bucketBegins_[bucket] == position)
            ++bucketBegins_[bucket];
        else if (bucketEnds_[bucket] == position)
            --bucketEnds_[bucket];
        return keyValList_.erase(position);
    }

    const_iterator erase(const_iterator first, const_iterator last) {
        for (auto it = first; it != last;) {
            auto nextIt =  it;
            ++nextIt;
            keyValList_.erase(it);
            it = nextIt;
        }
        return last;
    }

    [[nodiscard]] size_t max_size() const {
        return std::numeric_limits<std::ptrdiff_t>::max() /
               (sizeof(KeyValPair) + 2 * sizeof(iterator)) - 20;
    }

    [[nodiscard]] double load_factor() const {
        return 1.0 * size() / bucketCount_;
    }

    [[nodiscard]] double max_load_factor() const {
        return maxLoadFactor_;
    }

    void max_load_factor(double newMaxLoadFactor) {
        maxLoadFactor_ = newMaxLoadFactor;
    }
};