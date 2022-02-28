#include <iostream>
#include "matrix.h"

int main() {

    Matrix<20, 20> a;
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            int x;
            std::cin >> x;
            a[i][j] = Rational(x);
        }
    }

    a.invert();

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            std::cerr << a[i][j].asDecimal(20) << ' ';
        }
        std::cerr << '\n';
    }

    return 0;
}
