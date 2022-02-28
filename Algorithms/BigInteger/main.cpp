#include "biginteger.h"

int main() {
    int a;
    BigInteger ans = 1;

    for (int i = 1; i <= 10000; i++)
        ans *= 10000;

    return 0;
}