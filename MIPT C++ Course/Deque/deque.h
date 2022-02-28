#ifndef DEQUE_H
#define DEQUE_H

#include <algorithm> // only for std::swap()

namespace DequeHelpers {

    template<typename T>
    class ProtoDeque {
    private:

        static const size_t START_SIZE = 2u;

        size_t beginPosition = 0u;
        size_t endPosition = 0u;
        size_t capacity = START_SIZE;
        T* buffer = new T[capacity];

        size_t shiftPosition(size_t position, int shift) const {
            position += shift + capacity;
            while (position >= capacity) {
                position -= capacity;
            }
            return position;
        }

        void capacityCheck() {
            if (((size() + 1) << 2) < capacity ||
                shiftPosition(endPosition, 1) == beginPosition) {

                ProtoDeque copy(*this);
                swap(copy);
            }
        }

        size_t getRealIndex(size_t index) const {
            return shiftPosition(index, beginPosition);
        }

    public:
        ProtoDeque() = default;

        ~ProtoDeque() {
            delete[] buffer;
        }

        ProtoDeque(const ProtoDeque& other)
                :endPosition(other.size()), buffer(nullptr) {

            while (capacity < endPosition) {
                capacity <<= 1;
            }
            capacity <<= 1;

            buffer = new T[capacity];

            for (size_t i = 0; i < endPosition; ++i) {
                buffer[i] = other[i];
            }
        }

        void swap(ProtoDeque& other) {
            std::swap(beginPosition, other.beginPosition);
            std::swap(endPosition, other.endPosition);
            std::swap(capacity, other.capacity);
            std::swap(buffer, other.buffer);
        }

        ProtoDeque& operator=(const ProtoDeque& other) {
            ProtoDeque copy(other);
            swap(copy);
            return *this;
        }

        T& operator[](size_t index) {
            return buffer[getRealIndex(index)];
        }

        const T& operator[](size_t index) const {
            return buffer[getRealIndex(index)];
        }

        size_t size() const {
            return (endPosition >= beginPosition) ?
                   (endPosition - beginPosition) : (endPosition + capacity - beginPosition);
        }

        void pop_front() {
            beginPosition = shiftPosition(beginPosition, 1);
            capacityCheck();
        }

        void pop_back() {
            endPosition = shiftPosition(endPosition, -1);
            capacityCheck();
        }

        void push_front(const T& element) {
            capacityCheck();
            beginPosition = shiftPosition(beginPosition, -1);
            buffer[beginPosition] = element;
        }

        void push_back(const T& element) {
            capacityCheck();
            buffer[endPosition] = element;
            endPosition = shiftPosition(endPosition, 1);
        }

    };

} // namespace DequeHelpers


template<typename T>
class Deque {

private:

    static const size_t BATCH_SIZE_LOG2 = 20u;

    DequeHelpers::ProtoDeque<T*> batches;
    int firstBatchNumber = 0;

    static size_t getBatchSize() {
        return 1 << BATCH_SIZE_LOG2;
    }

    T* getBatch(int index) const {
        return batches[index - firstBatchNumber];
    }

    static T* createBatch() {
        return new T[getBatchSize()];
    }

    struct ProtoIterator {
        int batchNumber = 0;
        T* pointer = nullptr;
        const Deque* master = nullptr;

        ProtoIterator() = default;

        ProtoIterator(int batchNumber, T* pointer, const Deque* master)
                :batchNumber(batchNumber), pointer(pointer), master(master)
        {    }

        ProtoIterator(const ProtoIterator& other) = default;

        ~ProtoIterator() = default;

        T* getBatchBeginPointer() const {
            return master->getBatch(batchNumber);
        }

        ProtoIterator& operator=(const ProtoIterator& other) = default;

        ProtoIterator& operator+=(int shift) {
            int sign = (shift < 0 ? -1 : 1);
            shift *= sign;

            int batchShift = shift >> BATCH_SIZE_LOG2;
            int pointerShift = shift - (batchShift << BATCH_SIZE_LOG2);

            batchShift *= sign;
            pointerShift *= sign;

            pointerShift += pointer - getBatchBeginPointer();

            if (pointerShift < 0) {
                --batchShift;
                pointerShift += getBatchSize();
            }

            if (pointerShift >= static_cast<int>(getBatchSize())) {
                ++batchShift;
                pointerShift -= getBatchSize();
            }

            batchNumber += batchShift;
            pointer = getBatchBeginPointer() + pointerShift;


            return *this;
        }

        ProtoIterator& operator-=(int shift) {
            return *this += -shift;
        }

        ProtoIterator operator+(int shift) const {
            ProtoIterator copy(*this);
            copy += shift;
            return copy;
        }

        ProtoIterator operator-(int shift) const {
            ProtoIterator copy(*this);
            copy -= shift;
            return copy;
        }

        ProtoIterator& operator++() {
            return *this += 1;
        }

        ProtoIterator& operator--() {
            return *this -= 1;
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

        bool operator<(const ProtoIterator& other) const {
            if (batchNumber == other.batchNumber)
                return pointer < other.pointer;
            return batchNumber < other.batchNumber;
        }

        bool operator>(const ProtoIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ProtoIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ProtoIterator& other) const {
            return !(*this < other);
        }

        bool operator==(const ProtoIterator& other) const {
            return *this <= other && other <= *this;
        }

        bool operator!=(const ProtoIterator& other) const {
            return !(*this == other);
        }

        int operator-(const ProtoIterator& other) const {
            int answer = (batchNumber - other.batchNumber) * getBatchSize();
            answer += (pointer - getBatchBeginPointer()) -
                      (other.pointer - other.getBatchBeginPointer());
            return answer;
        }

        T& operator*() const {
            return *pointer;
        }

        T* operator->() const {
            return pointer;
        }

    };

    ProtoIterator veryBeginProtoIterator() const {
        return ProtoIterator(firstBatchNumber, batches[0], this);
    }

    ProtoIterator veryEndProtoIterator() const {
        return ProtoIterator(firstBatchNumber + batches.size() - 1,
                             batches[batches.size() - 1] + getBatchSize() - 1, this);
    }

    ProtoIterator beginProtoIterator;
    ProtoIterator endProtoIterator;

public:
    Deque() {
        batches.push_back(createBatch());
        beginProtoIterator = endProtoIterator = veryBeginProtoIterator();
    }

    ~Deque() {
        for (size_t i = 0; i < batches.size(); ++i) {
            delete[] batches[i];
        }
    }

    void push_front(const T& value) {
        if (beginProtoIterator == veryBeginProtoIterator()) {
            batches.push_front(createBatch());
            --firstBatchNumber;
        }
        --beginProtoIterator;
        *beginProtoIterator = value;
    }

    void push_back(const T& value) {
        if (endProtoIterator == veryEndProtoIterator())
            batches.push_back(createBatch());
        *endProtoIterator = value;
        ++endProtoIterator;
    }

    void pop_front() {
        ++beginProtoIterator;
        if (beginProtoIterator.batchNumber > firstBatchNumber) {
            batches.pop_front();
            ++firstBatchNumber;
        }
    }

    void pop_back() {
        --endProtoIterator;
        if (endProtoIterator.batchNumber < veryEndProtoIterator().batchNumber)
            batches.pop_back();
    }

    size_t size() const {
        return endProtoIterator - beginProtoIterator;
    }

    T& operator[](size_t index) {
        return *(beginProtoIterator + index);
    }

    const T& operator[](size_t index) const {
        return *(beginProtoIterator + index);
    }

    T& at(size_t index) {
        if (index >= size())
            throw std::out_of_range("Bad index.");
        return (*this)[index];
    }

    const T& at(size_t index) const {
        if (index >= size())
            throw std::out_of_range("Bad index.");
        return (*this)[index];
    }


    class iterator {
    private:
        ProtoIterator core;

    public:

        explicit iterator(const ProtoIterator& core)
                :core(core)
        {   }

        iterator& operator++() {
            ++core;
            return *this;
        }

        iterator& operator--() {
            --core;
            return *this;
        }

        iterator operator++(int) {
            return iterator(core++);
        }

        iterator operator--(int) {
            return iterator(core--);
        }

        iterator& operator+=(int shift) {
            core += shift;
            return *this;
        }

        iterator& operator-=(int shift) {
            core -= shift;
            return *this;
        }

        iterator operator+(int shift) const {
            return iterator(core + shift);
        }

        iterator operator-(int shift) const {
            return iterator(core - shift);
        }

        T& operator*() const {
            return *core;
        }

        T* operator->() const {
            return core.operator->();
        }

        bool operator<(const iterator& other) const {
            return core < other.core;
        }

        bool operator<=(const iterator& other) const {
            return core <= other.core;
        }

        bool operator==(const iterator& other) const {
            return core == other.core;
        }

        bool operator>=(const iterator& other) const {
            return core >= other.core;
        }

        bool operator>(const iterator& other) const {
            return core > other.core;
        }

        bool operator!=(const iterator& other) const {
            return core != other.core;
        }

        int operator-(const iterator& other) const {
            return core - other.core;
        }

    };


    class const_iterator {
    private:
        iterator core;

    public:

        const_iterator(const iterator& core)
                :core(core)
        {   }

        const_iterator& operator++() {
            ++core;
            return *this;
        }

        const_iterator& operator--() {
            --core;
            return *this;
        }

        const_iterator operator++(int) {
            return const_iterator(core++);
        }

        const_iterator operator--(int) {
            return const_iterator(core--);
        }

        const_iterator& operator+=(int shift) {
            core += shift;
            return *this;
        }

        const_iterator& operator-=(int shift) {
            core -= shift;
            return *this;
        }

        const_iterator operator+(int shift) const {
            return const_iterator(core + shift);
        }

        const_iterator operator-(int shift) const {
            return const_iterator(core - shift);
        }

        const T& operator*() const {
            return *core;
        }

        const T* operator->() const {
            return core.operator->();
        }

        bool operator<(const const_iterator& other) const {
            return core < other.core;
        }

        bool operator<=(const const_iterator& other) const {
            return core <= other.core;
        }

        bool operator==(const const_iterator& other) const {
            return core == other.core;
        }

        bool operator>=(const const_iterator& other) const {
            return core >= other.core;
        }

        bool operator>(const const_iterator& other) const {
            return core > other.core;
        }

        bool operator!=(const const_iterator& other) const {
            return core != other.core;
        }

        int operator-(const const_iterator& other) const {
            return core - other.core;
        }

    };

    iterator begin() {
        return iterator(beginProtoIterator);
    }

    iterator end() {
        return iterator(endProtoIterator);
    }

    const_iterator cbegin() const {
        return const_iterator(iterator(beginProtoIterator));
    }

    const_iterator cend() const {
        return const_iterator(iterator(endProtoIterator));
    }

    const_iterator begin() const {
        return cbegin();
    }

    const_iterator end() const {
        return cend();
    }


    Deque(const Deque& other)
            :Deque() {

        for (auto i: other) {
            push_back(i);
        }
    }

    void swap(Deque& other) {
        std::swap(beginProtoIterator, other.beginProtoIterator);
        std::swap(endProtoIterator, other.endProtoIterator);
        std::swap(firstBatchNumber, other.firstBatchNumber);
        batches.swap(other.batches);
        beginProtoIterator.master = this;
        endProtoIterator.master = this;
        other.beginProtoIterator.master = &other;
        other.endProtoIterator.master = &other;
    }

    Deque& operator=(const Deque& other) {
        Deque copy(other);
        swap(copy);
        return *this;
    }

    Deque(int size, const T& value)
            :Deque() {

        while (size > 0) {
            push_back(value);
            --size;
        }
    }

    explicit Deque(int size)
            :Deque(size, T()) {  }


    void insert(const iterator& beforeWhat, const T& value) {
        if (beforeWhat == end()) {
            push_back(value);
            return;
        }

        auto i = end() - 1;

        push_back(*i);

        while (i != beforeWhat) {
            auto previous = i - 1;
            *i = *previous;
            i = previous;
        }

        *i = value;
    }

    void erase(const iterator& place) {

        if (place == end()) {
            pop_back();
            return;
        }

        for (auto i = place + 1; i != end(); ++i) {
            *(i - 1) = *i;
        }
        pop_back();

    }

};


#endif // DEQUE_H