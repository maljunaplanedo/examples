#include <iostream>
#include "residue.h"

int main() {

    Residue<955049953> a(15);
    std::cout << static_cast<int>(a.pow(123456789u));

    return 0;
}
