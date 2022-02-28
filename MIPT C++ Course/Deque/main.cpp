#ifndef DEQUE_H
#define DEQUE_H

#include <iostream> // for std::cerr
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

        ProtoDeque(size_t size, const T& value)
                :ProtoDeque() {

            for (size_t i = 0; i < size; ++i) {
                push_back(value);
            }
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

    static const size_t BATCH_SIZE_LOG2 = 8u;

    DequeHelpers::ProtoDeque<T*> batches;
    int firstBatchNumber;

    static size_t getBatchSize() {
        return 1 << BATCH_SIZE_LOG2;
    }

    static T* createBatch() {
        return new T[getBatchSize()];
    }

    struct ProtoIterator {
        int batchNumber;
        T* pointer;
        const DequeHelpers::ProtoDeque<T*>* masterBatches;
        const int* firstBatchNumber;

        T* getBatch(int index) const {
            return (*masterBatches)[index - *firstBatchNumber];
        }

        ProtoIterator(int batchNumber, T* pointer,
                      const DequeHelpers::ProtoDeque<T*>* masterBatches,
                      const int* firstBatchNumber)
                :batchNumber(batchNumber), pointer(pointer),
                 masterBatches(masterBatches), firstBatchNumber(firstBatchNumber)
        {    }

        ProtoIterator(const ProtoIterator& other) = default;

        ~ProtoIterator() = default;


        T* getBatchBeginPointer() const {
            return getBatch(batchNumber);
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
        return ProtoIterator(firstBatchNumber, batches[0], &batches, &firstBatchNumber);
    }

    ProtoIterator veryEndProtoIterator() const {
        return ProtoIterator(firstBatchNumber + batches.size() - 1,
                             batches[batches.size() - 1] + getBatchSize() - 1, &batches, &firstBatchNumber);
    }

    ProtoIterator beginProtoIterator;
    ProtoIterator endProtoIterator;

public:
    Deque()
            :batches(1, createBatch()), firstBatchNumber(0),
             beginProtoIterator(veryBeginProtoIterator()), endProtoIterator(veryBeginProtoIterator())
    {   }

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
    public:
        //private:
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
        std::cerr << place.core.batchNumber << ' ' << place.core.pointer << '\n';
        std::cerr << batches.size() << ' ' << firstBatchNumber << ' ' << batches[0] << "\n\n";
        for (auto i = place + 1; i != end(); ++i) {
            *(i - 1) = *i;
        }
        pop_back();

        std::cerr << size();
    }

};


#endif // DEQUE_H

using namespace std;

Deque<int> d;

int main()
{
    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
    string s;
    int x;
    cin>>s;
    while(true)
    {
        if(s=="exit")
        {
            cout<<"bye";
            return 0;
        }
        if(s=="push_front")
        {
            cin>>x;
            d.push_front(x);
            cout<<"ok"<<endl;
        }
        if(s=="push_back")
        {
            cin>>x;
            d.push_back(x);
            cout<<"ok"<<endl;
        }
        if(s=="pop_front")
        {
            if (d.size() == 0u) {
                std::cout << "error\n";
                cin >> s;
                continue;
            }
            cout<<d[0]<<endl;
            d.pop_front();
        }
        if(s=="pop_back")
        {
            if (d.size() == 0u) {
                std::cout << "error\n";
                cin >> s;
                continue;
            }
            cout<<d[d.size()-1]<<endl;
            d.pop_back();
        }
        if(s=="front")
        {
            if (d.size() == 0u) {
                std::cout << "error\n";
                cin >> s;
                continue;
            }
            cout<<d[0]<<endl;
        }
        if(s=="back")
        {
            if (d.size() == 0u) {
                std::cout << "error\n";
                cin >> s;
                continue;
            }
            cout<<d[d.size()-1]<<endl;
        }
        if(s=="size")
        {
            cout<<d.size()<<endl;
        }
        if(s=="clear")
        {
            while(d.size() > 0)d.pop_back();
            cout<<"ok"<<endl;
        }
        cin>>s;
    }
}