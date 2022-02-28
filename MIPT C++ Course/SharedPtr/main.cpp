#include <iostream>
#include <vector>
#include "smart_pointers.h"

template<typename T>
void destroy(T* t) {
    t->~T();
}

struct Node {
    int value;
    WeakPtr<Node> parent;
    std::vector<SharedPtr<Node>> children;
};

int main() {
    std::string* s = new std::string("abcd");
    s->~basic_string();
    destroy(s);
}