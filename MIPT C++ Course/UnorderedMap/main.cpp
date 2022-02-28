#include <iostream>
#include <vector>

class P {
private:
    int x;
    P(int x) {this->x = x;}

public:
    P(){x = 0;}

    template<typename T>
    P(T&& x) {
        this->x = x;
    }

    bool operator==(const P& other) const {return true;}

};

namespace std {
    template<>
    struct hash<P> {
        size_t operator()(const P& p) const {
            return 2;
        }
    };
}

#include "unordered_map.h"

int main() {


    UnorderedMap<int, int> mp;
    mp.emplace(1, 1);
    mp.emplace(2, 3);

    for (const auto& x: mp)
        std::cout << x.first << ' ' << x.second << '\n';

    /*
    UnorderedMap<std::string, int> mp;
    mp["abc"] = 1;
    mp["def"] = 3;
    mp["ghi"] = 2;
    mp.emplace("jkl", 8);
    mp.rehash(2000);
    //mp.erase(mp.find(7));

    UnorderedMap<std::string, int> mps = std::move(mp);
    mps.insert({"abc", 2});

    for (auto& i: mps)
        std::cout << i.first << ' ' << i.second << '\n';
        */
}
