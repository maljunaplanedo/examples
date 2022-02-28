#include "fastallocator.h"
#include <algorithm>
#include <cassert>

void time_measure() {
    List<int, FastAllocator<int>> v1;

    auto start1 = std::chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i) {
        v1.push_back(i);
    }

    for (int x: v1)
        std::cout << x << ' ';
    std::cout << '\n';

    double duration1 = (std::chrono::steady_clock::now() - start1).count();



    List<int> v2;

    auto start2 = std::chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i) {
        v2.push_back(i);
    }

    for (int x: v2)
        std::cout << x << ' ';
    std::cout << '\n';

    double duration2 = (std::chrono::steady_clock::now() - start2).count();

    std::cout << duration1 << '\n' << duration2 << '\n';
}

template <typename Alloc = std::allocator<int>>
void BasicListTest() {
    List<int, Alloc> lst;

    assert(lst.size() == 0);

    lst.push_back(3);
    lst.push_back(4);
    lst.push_front(2);
    lst.push_back(5);
    lst.push_front(1);

    std::reverse(lst.begin(), lst.end());
    // now lst is 5 4 3 2 1

    assert(lst.size() == 5);

    std::string s;
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "54321");
    //std::cerr << "Tests log: check 1.1 ok, list contains 5 4 3 2 1" << std::endl;

    auto cit = lst.cbegin();
    std::advance(cit, 3);

    lst.insert(cit, 6);
    lst.insert(cit, 7);

    std::advance(cit, -3);
    lst.insert(cit, 8);
    lst.insert(cit, 9);
    // now lst is 5 4 8 9 3 6 7 2 1

    assert(lst.size() == 9);

    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "548936721");
    //std::cerr << "Tests log: check 1.2 ok, list contains 5 4 8 9 3 6 7 2 1" << std::endl;

    lst.erase(lst.cbegin());
    lst.erase(cit);

    lst.pop_front();
    lst.pop_back();

    const auto copy = lst;
    assert(lst.size() == 5);
    assert(copy.size() == 5);
    // now both lists are 8 9 6 7 2

    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "89672");
    //std::cerr << "Tests log: check 1.3 ok, list contains 8 9 6 7 2" << std::endl;

    auto rit = lst.rbegin();
    ++rit;
    lst.erase(rit.base());
    assert(lst.size() == 4);

    rit = lst.rbegin();
    *rit = 3;

    // now lst: 8 9 6 3, copy: 8 9 6 7 2
    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "8963");

    assert(copy.size() == 5);

    s.clear();
    for (int x: copy) {
        s += std::to_string(x);
    }
    assert(s == "89672");

    //std::cerr << "Tests log: check 1.4 ok, list contains 8 9 6 3, another list is still 8 9 6 7 2" << std::endl;

    typename List<int, Alloc>::const_reverse_iterator crit = rit;
    crit = copy.rbegin();
    assert(*crit == 2);

    cit = crit.base();
    std::advance(cit, -2);
    assert(*cit == 7);

}

struct P {
    char buffer[10]{};
};

int main() {

    P x;
    std::cout << &x << ' ' << &(x.buffer) << '\n';

    std::list<int, FastAllocator<int>> l;
    for (int i = 0; i < 20000000; ++i)
        l.push_back(i);


    return 0;
}