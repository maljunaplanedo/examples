#ifndef RESIDUE_H
#define RESIDUE_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

namespace ResidueHelper {
    unsigned normalMod(long long n, int m) {
        n %= m;
        if (n < 0)
            n += m;
        return n;
    }

    unsigned getPhi(unsigned n) {
        unsigned result = n;
        for (unsigned i = 2; i * i <= n; ++i) {
            if (n % i == 0) {
                while (n % i == 0) {
                    n /= i;
                }
                result -= result / i;
            }
        }
        if (n > 1)
            result -= result / n;
        return result;
    }

    unsigned gcd(unsigned a, unsigned b) {
        while (a > 0 && b > 0) {
            if (a > b)
                a %= b;
            else
                b %= a;
        }
        return a + b;
    }


    template <bool v>
    struct staticAssert;

    template<>
    struct staticAssert<true> {  };


    template <unsigned N, unsigned I, bool NdivsI, bool I2GreaterN>
    struct PrimeCheckHelper {
        static const bool value = false;
    };

    template <unsigned N, unsigned I>
    struct PrimeCheckHelper<N, I, true, false> {
        static const bool value = false;
    };

    template <unsigned N, unsigned I>
    struct PrimeCheckHelper<N, I, false, false> {
        static const bool value = PrimeCheckHelper<N, I + 1, ((N % (I + 1)) == 0), ((I + 1) * (I + 1) > N)>::value;
    };

    template <unsigned N, unsigned I, bool NdivsI>
    struct PrimeCheckHelper<N, I, NdivsI, true> {
        static const bool value = true;
    };



    template <unsigned N, unsigned I, bool NdivsI>
    struct thisPrimePowHelper {
        static const bool value = false;
    };

    template <unsigned N, unsigned I>
    struct thisPrimePowHelper<N, I, true> {
        static const bool value = thisPrimePowHelper<N / I, I, (N / I) % I == 0>::value;
    };

    template <unsigned N>
    struct thisPrimePowHelper<N, N, true> {
        static const bool value = true;
    };




    template <unsigned N, unsigned I, bool NdivsI, bool I2greaterN>
    struct oddPrimePowHelper {
        static const bool value = false;
    };

    template <unsigned N, unsigned I>
    struct oddPrimePowHelper<N, I, false, false> {
        static const bool value =
                oddPrimePowHelper<N, I + 2, N % (I + 2) == 0, ((I + 2) * (I + 2) > N)>::value;
    };

    template <unsigned N, unsigned I>
    struct oddPrimePowHelper<N, I, true, false> {
        static const bool value = thisPrimePowHelper<N, I, true>::value;
    };

    template <unsigned N, unsigned I, bool NdivsI>
    struct oddPrimePowHelper<N, I, NdivsI, true> {
        static const bool value = true;
    };



    template <unsigned N, unsigned Nmod4>
    struct primitiveRootBaseHelper {
        static const bool value = oddPrimePowHelper<N, 3, (N % 3) == 0, (3 * 3 > N)>::value;
    };

    template <unsigned N>
    struct primitiveRootBaseHelper<N, 2> {
        static const bool value = primitiveRootBaseHelper<N / 2, 1>::value;
    };

    template <unsigned N>
    struct primitiveRootBaseHelper<N, 0> {
        static const bool value = false;
    };

}


template <unsigned N>
struct is_prime {
    static const bool value = ResidueHelper::PrimeCheckHelper<N, 2, (N % 2) == 0, (2 * 2 > N)>::value;
};

template <>
struct is_prime<1> {
    static const bool value = false;
};

template <unsigned N>
static const bool is_prime_v = is_prime<N>::value;




template <unsigned N>
struct has_primitive_root {
    static const bool value = ResidueHelper::primitiveRootBaseHelper<N, N % 4>::value;
};

template <>
struct has_primitive_root<2> {
    static const bool value = true;
};

template <>
struct has_primitive_root<4> {
    static const bool value = true;
};

template <unsigned N>
static const bool has_primitive_root_v = has_primitive_root<N>::value;





template <unsigned M>
class Residue;

template <unsigned M>
Residue<M> operator+(const Residue<M>& left, const Residue<M>& right);

template <unsigned M>
Residue<M> operator-(const Residue<M>& left, const Residue<M>& right);

template <unsigned M>
Residue<M> operator*(const Residue<M>& left, const Residue<M>& right);

template <unsigned M>
Residue<M> operator/(const Residue<M>& left, const Residue<M>& right);

template <unsigned M>
class Residue {

private:
    unsigned int number;

public:

    Residue():
        Residue(0)
    {  }

    explicit Residue(int n):
        number(ResidueHelper::normalMod(n, M))
    {  }

    explicit operator int() const {
        return number;
    }

    Residue(const Residue& other):
        number(other.number)
    {  }

    void swap(Residue& other) {
        std::swap(number, other.number);
    }

    Residue& operator=(const Residue& other) {
        Residue copy(other);
        swap(copy);
        return *this;
    }

    Residue operator-() const {
        return Residue(-number);
    }

    Residue& operator+=(const Residue& other) {
        number = ResidueHelper::normalMod(1ll * number + other.number, M);
        return *this;
    }

    Residue& operator-=(const Residue& other) {
        return *this += -other;
    }

    Residue& operator*=(const Residue& other) {

        number = ResidueHelper::normalMod(1ll * number * other.number, M);
        return *this;
    }

    Residue& operator/=(const Residue& other) {
        return *this *= other.getInverse();
    }

    Residue pow(unsigned k) const {
        if (k == 0)
            return Residue(1);
        if (k & 1)
            return *this * pow(k - 1);
        Residue temp = pow(k >> 1);
        return temp * temp;
    }

    Residue pow(signed k) const = delete;

    Residue getInverse() const {
        ResidueHelper::staticAssert<is_prime_v<M> >();
        if (number == 0)
            return Residue(0);
        return pow(M - 2);
    }

    unsigned order() const {
        if (ResidueHelper::gcd(number, M) != 1)
            return 0;
        unsigned phi = ResidueHelper::getPhi(M);
        std::vector<unsigned> divisors;
        for (unsigned i = 1; 1ll * i * i <= phi; ++i) {
            if (phi % i == 0) {
                divisors.push_back(i);
                if (i * i < phi)
                    divisors.push_back(phi / i);
            }
        }

        unsigned answer = M;
        for (auto divisor: divisors) {
            if (static_cast<int>(pow(divisor)) == 1)
                answer = std::min(divisor, answer);
        }

        answer %= M;
        return answer;
    }

    static Residue getPrimitiveRoot() {
        ResidueHelper::staticAssert<has_primitive_root_v<M> >();

        std::vector<unsigned> factorization;
        unsigned phi = ResidueHelper::getPhi(M);
        unsigned n = phi;

        if (n == 1) {
            return Residue(1);
        }

        for (unsigned i = 2; 1ll * i * i <= n; ++i) {
            if (n % i == 0) {
                factorization.push_back(i);
                while (n % i == 0) {
                    n /= i;
                }
            }
        }
        if (n > 1)
            factorization.push_back(n);

        for (unsigned res = 2; res < M; ++res) {
            if (ResidueHelper::gcd(res, M) != 1)
                continue;
            bool ok = true;
            for (size_t i = 0; i < factorization.size() && ok; ++i) {
                ok &= (static_cast<int>(Residue(res).pow(phi / factorization[i])) != 1);
            }
            if (ok)
                return Residue(res);
        }
        return Residue(0);
    }

};

template <unsigned M>
Residue<M> operator+(const Residue<M>& left, const Residue<M>& right) {
    Residue<M> answer(left);
    answer += right;
    return answer;
}

template <unsigned M>
Residue<M> operator-(const Residue<M>& left, const Residue<M>& right) {
    Residue<M> answer(left);
    answer -= right;
    return answer;
}

template <unsigned M>
Residue<M> operator*(const Residue<M>& left, const Residue<M>& right) {
    Residue<M> answer(left);
    answer *= right;
    return answer;
}

template <unsigned M>
Residue<M> operator/(const Residue<M>& left, const Residue<M>& right) {
    Residue<M> answer(left);
    answer /= right;
    return answer;
}

#endif //RESIDUE_H