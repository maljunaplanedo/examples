#include <iostream>
#include <variant>
#include "variant.h"

template<typename T>
struct A {
    A() = delete;
};

int main() {
    Variant<int, const std::string> q = "Grg";
    std::cout << get<1>(q);
    q = 3;
}
