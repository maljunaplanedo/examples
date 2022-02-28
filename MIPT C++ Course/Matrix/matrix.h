#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <algorithm>

/* NO REVIEW PART BEGINS HERE
 * Do not spend time reviewing the code below!
 * This is just copy of BigInteger, Rational, Residue classes,
 * which have already been checked or will be checked independently.
 */

#ifndef BIGINTEGER_H
#define BIGINTEGER_H

void reverseString(std::string& string) {
    for (int i = 0; i < static_cast<int>(string.size()) - i - 1; ++i) {
        std::swap(string[i], string[string.size() - i - 1]);
    }
}

class BigInteger;

BigInteger operator+(const BigInteger&, const BigInteger&);
BigInteger operator-(const BigInteger&, const BigInteger&);
BigInteger operator*(const BigInteger&, const BigInteger&);
BigInteger operator/(const BigInteger&, const BigInteger&);

bool operator<(const BigInteger&, const BigInteger&);
bool operator<=(const BigInteger&, const BigInteger&);
bool operator==(const BigInteger&, const BigInteger&);
bool operator!=(const BigInteger&, const BigInteger&);
bool operator>(const BigInteger&, const BigInteger&);
bool operator>=(const BigInteger&, const BigInteger&);

std::ostream& operator<<(std::ostream& out, const BigInteger& number);

class BigInteger {
private:
    typedef unsigned char block;
    typedef int operationBuffer;
    typedef long double real;
    typedef std::complex<real> complex;
    typedef std::vector<complex> Polynomial;

    static const operationBuffer BLOCK_SIZE = 100;

    static const real PI;

    operationBuffer sign;
    std::vector<block> buffer;

    void removeLeadingZeroes() {
        while (buffer.size() > 1u && buffer.back() == 0u) {
            buffer.pop_back();
        }
        if (buffer.empty())
            buffer.push_back(0u);
    }

    BigInteger& addOther(const BigInteger& other, operationBuffer addSign) {

        addSign *= sign * other.sign;
        buffer.resize(std::max(buffer.size(), other.buffer.size()), 0);

        bool largerFromSmallerSubstraction = false;
        if (addSign == -1)
            largerFromSmallerSubstraction = (compare(other, false) < 0);
        operationBuffer memory = 0;

        for (size_t i = 0; i < buffer.size(); ++i) {
            operationBuffer left = buffer[i];
            operationBuffer right = 0;
            if (i < other.buffer.size())
                right = other.buffer[i];
            else if (!memory)
                break;

            if (largerFromSmallerSubstraction)
                std::swap(left, right);

            left += addSign * right;
            left += memory;

            operationBuffer operationSign = (left < 0 || left >= BLOCK_SIZE);
            operationSign *= addSign;

            memory = operationSign;
            left -= operationSign * BLOCK_SIZE;

            buffer[i] = left;
        }

        if (largerFromSmallerSubstraction)
            sign *= -1;

        if (memory == 1)
            buffer.push_back(1);

        removeLeadingZeroes();

        return *this;
    }

    block getDigit(size_t index) const {
        if (index >= buffer.size())
            return 0;
        return buffer[index];
    }

    void setDigit(size_t index, block d) {
        while (buffer.size() <= index) {
            buffer.push_back(0);
        }
        buffer[index] = d;
    }

    static void fft(Polynomial& polynomial, bool invert) {
        size_t size = polynomial.size();

        for (size_t i = 1, invertedI = 0; i < size; ++i) {
            size_t bit = size >> 1;

            while (invertedI >= bit) {
                invertedI -= bit;
                bit >>= 1;
            }
            invertedI += bit;

            if (i < invertedI)
                std::swap(polynomial[i], polynomial[invertedI]);
        }

        for (size_t len = 2; len <= size; len <<= 1) {
            size_t halfLen = len >> 1;
            real angle = (2 * PI / len) * (invert ? -1 : 1);
            complex wLen(cos(angle), sin(angle));

            for (size_t begin = 0; begin < size; begin += len) {
                complex wI = 1;
                for (size_t i = 0; i < halfLen; ++i, wI *= wLen) {
                    complex left = polynomial[begin + i];
                    complex right = polynomial[begin + halfLen + i] * wI;

                    polynomial[begin + i] = left + right;
                    polynomial[begin + halfLen + i] = left - right;
                }
            }
        }

        if (invert) {
            for (size_t i = 0; i < size; ++i) {
                polynomial[i] /= size;
            }
        }
    }

    void buildBufferFromULL(unsigned long long prototype) {
        while (prototype > 0) {
            unsigned long long oldValue = prototype;
            prototype /= BLOCK_SIZE;
            buffer.push_back(oldValue - BLOCK_SIZE * prototype);
        }
        if (buffer.empty())
            buffer.push_back(0);
    }

    void multiplyByShort(block multiplier) {
        operationBuffer memory = 0;
        for (block& i : buffer) {
            operationBuffer temporary = i;
            temporary *= multiplier;
            temporary += memory;

            i = temporary % BLOCK_SIZE;
            memory = temporary / BLOCK_SIZE;
        }
        if (memory)
            buffer.push_back(memory);
        removeLeadingZeroes();
    }

    void divideByShort(block divisor) {
        operationBuffer memory = 0;
        for (int i = static_cast<int>(buffer.size()) - 1; i >= 0; --i) {
            operationBuffer temporary = memory * BLOCK_SIZE;
            temporary += buffer[i];
            memory = temporary % divisor;
            buffer[i] = temporary / divisor;
        }

        removeLeadingZeroes();
    }

public:
    BigInteger(int prototype):
            sign(prototype < 0 ? -1 : 1) {

        prototype = abs(prototype);
        buildBufferFromULL(prototype);
    }

    explicit BigInteger(unsigned long long prototype): BigInteger() {
        buildBufferFromULL(prototype);
    }

    BigInteger(): BigInteger(0)
    {   }

    explicit BigInteger(std::string prototype): BigInteger() {
        buffer.clear();
        reverseString(prototype);
        if (prototype.back() == '-') {
            sign = -1;
            prototype.pop_back();
        }

        while (prototype.back() == '0') {
            prototype.pop_back();
        }

        if (prototype.size() % 2)
            prototype.push_back('0');
        for (int i = 0; i < static_cast<int>(prototype.size()); i += 2) {
            buffer.push_back((prototype[i] - '0') + 10 * (prototype[i + 1] - '0'));
        }
    }

    BigInteger(const BigInteger&) = default;

    BigInteger& operator=(const BigInteger&) = default;

    BigInteger& operator+=(const BigInteger& other) {
        return addOther(other, 1);
    }

    BigInteger& operator-=(const BigInteger& other) {
        return addOther(other, -1);
    }

    BigInteger operator-() const {
        BigInteger answer = *this;
        answer.sign *= -1;
        return answer;
    }

    BigInteger& operator++() {
        return *this += BigInteger(1);
    }

    BigInteger& operator--() {
        return *this += BigInteger(-1);
    }

    BigInteger operator++(int) {
        const BigInteger old = *this;
        ++*this;
        return old;
    }

    BigInteger operator--(int) {
        BigInteger old = *this;
        --*this;
        return old;
    }

    BigInteger& operator*=(const BigInteger& other) {
        sign *= other.sign;

        if (buffer.size() == 1u || other.buffer.size() == 1u) {
            bool isThisShort = buffer.size() == 1u;

            block multiplier = (isThisShort ? buffer[0] : other.buffer[0]);
            operationBuffer newSign = sign;
            if (isThisShort)
                *this = other;

            multiplyByShort(multiplier);
            sign = newSign;
            return *this;
        }

        Polynomial left;
        Polynomial right;

        for (auto x: buffer) {
            left.push_back(x);
        }
        for (auto x: other.buffer) {
            right.push_back(x);
        }

        size_t size = 1;
        while (size <= std::max(left.size(), right.size())) {
            size <<= 1;
        }
        size <<= 1;

        left.resize(size);
        right.resize(size);

        fft(left, false);
        fft(right, false);

        for (size_t i = 0; i < size; ++i) {
            left[i] *= right[i];
        }

        fft(left, true);

        buffer.clear();
        operationBuffer memory = 0;
        for (size_t i = 0; memory != 0 || i < size; ++i) {
            operationBuffer temporary = memory;
            if (i < size)
                temporary += static_cast<operationBuffer>(round(left[i].real()));
            memory = temporary / BLOCK_SIZE;
            buffer.push_back(temporary - memory * BLOCK_SIZE);
        }

        removeLeadingZeroes();

        return *this;
    }

    BigInteger& operator/=(const BigInteger& other) {
        if (other == 0)
            throw std::runtime_error("Zero division");

        BigInteger mutableOther = other;

        operationBuffer newSign = sign * mutableOther.sign;
        mutableOther.sign = sign = 1;

        if (compare(mutableOther, false) < 0)
            return *this = 0;

        if (mutableOther.buffer.size() == 1u) {
            divideByShort(mutableOther.buffer[0]);
            sign = newSign;
            return *this;
        }

        /* Knuth Algorithm D
         * Author's variable names left without changes
         * Dear code reviewer, pls don't kill me
         * */

        // D1

        size_t n = mutableOther.buffer.size();
        size_t m = buffer.size() - n;

        BigInteger bPowNPlusOne;
        bPowNPlusOne.buffer.assign(n + 2, 0);
        bPowNPlusOne.buffer[n + 1] = 1;

        BigInteger q;
        q.buffer.assign(m + 1, 0);

        block d = BLOCK_SIZE / (mutableOther.getDigit(n - 1) + 1);
        multiplyByShort(d);
        mutableOther.multiplyByShort(d);

        for (int j = m; j >= 0; --j) { // D2, D7

            // D3
            operationBuffer qv = 1 * getDigit(j + n) * BLOCK_SIZE
                                 + getDigit(j + n - 1);
            operationBuffer rv = qv % mutableOther.getDigit(n - 1);
            qv /= mutableOther.getDigit(n - 1);

            while (qv == BLOCK_SIZE || qv * mutableOther.getDigit(n - 2) >
                                       BLOCK_SIZE * rv + getDigit(j + n - 2)) {

                --qv;
                rv += mutableOther.getDigit(n - 1);

                if (rv >= BLOCK_SIZE)
                    break;
            }

            // D4

            BigInteger otherCopy = mutableOther;
            otherCopy.multiplyByShort(qv);

            BigInteger thisCopy;
            thisCopy.buffer.clear();
            for (size_t i = j; i <= j + n; ++i) {
                thisCopy.buffer.push_back(getDigit(i));
            }

            thisCopy.removeLeadingZeroes();
            thisCopy -= otherCopy;

            bool flag = false;
            if (thisCopy < 0) {
                flag = true;
                thisCopy += bPowNPlusOne;
            }

            // D5

            q.buffer[j] = qv;

            if (flag) {
                // D6

                --q.buffer[j];
                mutableOther.setDigit(n, 0);
                thisCopy += mutableOther;

                if (thisCopy.buffer.size() == n + 2)
                    thisCopy.buffer.pop_back();
            }

            for (size_t i = j; i <= j + n; ++i) {
                setDigit(i, thisCopy.getDigit(i - j));
            }
        }

        buffer = q.buffer;
        removeLeadingZeroes();

        sign = newSign;
        return *this;
    }

    BigInteger& operator%=(const BigInteger& other) {
        BigInteger temporary = *this;
        temporary /= other;
        temporary *= other;

        return *this -= temporary;
    }

    explicit operator int() const {
        int answer = 0;
        for (int i = 0, base = 1; i < static_cast<int>(buffer.size()); ++i) {
            answer += buffer[i] * base;
            base *= BLOCK_SIZE;
        }

        return answer;
    }

    explicit operator bool() const {
        return buffer[0] || buffer.size() > 1u;
    }

    std::string toString(size_t neededLength = 0) const {
        if (*this == 0) {
            std::string answer;
            while (neededLength > 0) {
                answer += '0';
                --neededLength;
            }
            if (answer.empty())
                answer += '0';
            return answer;
        }
        std::string answer;

        for (block i : buffer) {
            answer += static_cast<char>(i % 10) + '0';
            answer += static_cast<char>(i / 10) + '0';
        }
        while (answer.size() > 1u && answer.back() == '0')
            answer.pop_back();

        while (answer.size() < neededLength)
            answer += '0';
        if (sign == -1)
            answer += '-';
        reverseString(answer);

        return answer;
    }

    operationBuffer compare(const BigInteger& other, bool checkSigns = true) const {

        if (buffer.size() == 1u && other.buffer.size() == 1u
            && buffer[0] == 0u && other.buffer[0] == 0u)
            return 0;

        operationBuffer comparator;

        if (checkSigns) {
            comparator = sign - other.sign;
            if (comparator)
                return comparator;
        }

        comparator = static_cast<int>(buffer.size()) - other.buffer.size();
        if (comparator)
            return comparator;

        for (int i = static_cast<int>(buffer.size()) - 1; i >= 0; --i) {
            comparator = static_cast<int>(buffer[i]) - other.buffer[i];
            if (comparator)
                return comparator;
        }
        return 0;
    }

    friend BigInteger abs(BigInteger);
};

const BigInteger::real BigInteger::PI = 3.14159265358979323846;

BigInteger operator+(const BigInteger& left, const BigInteger& right) {
    BigInteger answer = left;
    answer += right;
    return answer;
}

BigInteger operator-(const BigInteger& left, const BigInteger& right) {
    BigInteger answer = left;
    answer -= right;
    return answer;
}

BigInteger operator*(const BigInteger& left, const BigInteger& right) {
    BigInteger answer = left;
    answer *= right;
    return answer;
}

BigInteger operator/(const BigInteger& left, const BigInteger& right) {
    BigInteger answer = left;
    answer /= right;
    return answer;
}

BigInteger operator%(const BigInteger& left, const BigInteger& right) {
    BigInteger answer = left;
    answer %= right;
    return answer;
}

bool operator<(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) < 0;
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) <= 0;
}

bool operator==(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) == 0;
}

bool operator>(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) > 0;
}

bool operator>=(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) >= 0;
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
    return left.compare(right) != 0;
}

std::istream& operator>>(std::istream& in, BigInteger& number) {
    std::string inputBuffer;
    in >> inputBuffer;

    number = BigInteger(inputBuffer);

    return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& number) {
    return out << number.toString();
}

BigInteger operator""_bi(unsigned long long prototype) {
    return BigInteger(prototype);
}

BigInteger operator""_bi(const char* prototype, size_t) {
    return BigInteger(prototype);
}

BigInteger abs(BigInteger a) {
    a.sign = 1;
    return a;
}

BigInteger gcd(BigInteger a, BigInteger b) {
    while (a > 0 && b > 0) {
        if (a > b)
            a %= b;
        else
            b %= a;
    }
    return a + b;
}

BigInteger lcm(const BigInteger& a, const BigInteger& b) {
    return a / gcd(a, b) * b;
}

class Rational;

Rational operator+(const Rational&, const Rational&);
Rational operator-(const Rational&, const Rational&);
Rational operator*(const Rational&, const Rational&);
Rational operator/(const Rational&, const Rational&);

bool operator<(const Rational&, const Rational&);
bool operator<=(const Rational&, const Rational&);
bool operator==(const Rational&, const Rational&);
bool operator!=(const Rational&, const Rational&);
bool operator>(const Rational&, const Rational&);
bool operator>=(const Rational&, const Rational&);

class Rational {
private:
    BigInteger numerator;
    BigInteger denominator;

    void reduce() {
        BigInteger reductionCoefficient = gcd(abs(numerator), denominator);
        numerator /= reductionCoefficient;
        denominator /= reductionCoefficient;
    }

    void toCommonDenominator(Rational& other) {
        BigInteger newDenominator = lcm(denominator, other.denominator);
        numerator *= newDenominator / denominator;
        other.numerator *= newDenominator / other.denominator;
        denominator = other.denominator = newDenominator;
    }

    BigInteger integerQuotient(size_t powerOfTen) const {
        BigInteger answer = 1;
        for (size_t i = 0; i < powerOfTen; i++)
            answer *= 10;
        answer *= numerator;
        answer /= denominator;

        return answer;
    }

public:

    Rational(int prototype): Rational(static_cast<BigInteger>(prototype))
    {   }

    Rational(const BigInteger& prototype):
            numerator(prototype),
            denominator(1)
    {   }

    Rational(): Rational(0)
    {   }

    Rational(const Rational&) = default;

    Rational& operator=(const Rational&) = default;

    Rational operator-() const {
        Rational answer = *this;
        answer.numerator = -answer.numerator;
        return answer;
    }

    Rational& operator+=(const Rational& other) {
        Rational mutableOther = other;
        toCommonDenominator(mutableOther);
        numerator += mutableOther.numerator;

        reduce();
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        return *this += -other;
    }

    Rational& operator*=(const Rational& other) {
        BigInteger crossGcd = gcd(abs(numerator), other.denominator);
        numerator /= crossGcd;
        BigInteger newDenominator = other.denominator / crossGcd;

        crossGcd = gcd(abs(other.numerator), denominator);

        numerator *= other.numerator / crossGcd;
        newDenominator *= denominator / crossGcd;

        denominator = newDenominator;



        return *this;
    }

    Rational& operator/=(const Rational& other) {
        if (other.numerator == 0)
            other.denominator / other.numerator;

        Rational multiplier;
        multiplier.numerator = other.denominator;
        multiplier.denominator = other.numerator;

        if (multiplier.denominator < 0) {
            multiplier.numerator *= -1;
            multiplier.denominator *= -1;
        }

        return *this *= multiplier;
    }

    std::string toString() const {
        std::string answer = numerator.toString();
        if (denominator != 1)
            answer += '/' + denominator.toString();
        return answer;
    }

    std::string asDecimal(size_t precision = 0) const {
        std::string answer;
        if (numerator == 0) {
            answer = "0.";
            for (size_t i = 0; i < precision; ++i)
                answer += '0';
            return answer;
        }

        answer = integerQuotient(precision).toString(precision);

        if (answer.size() == precision || (answer.size() == precision + 1 && answer[0] == '-')) {
            answer = '0' + answer;
            if (answer.size() > 1u && answer[1] == '-')
                std::swap(answer[0], answer[1]);
        }
        if (precision != 0)
            answer.insert(answer.begin() + answer.size() - precision, '.');

        return answer;
    }

    explicit operator double() const {
        const size_t PRECISION = 20;
        std::string buffer = asDecimal(PRECISION);

        double answer = 0;
        double sign = 1;
        if (buffer[0] == '-')
            sign = -1;

        int point = buffer.find('.');
        double power10 = 1;
        for (int i = point - 1; i >= 0 && buffer[i] != '-'; --i) {
            answer += (buffer[i] - '0') * power10 * sign;
            power10 *= 10;
        }

        power10 = .1;
        for (size_t i = point + 1; i < buffer.size(); ++i) {
            answer += (buffer[i] - '0') * power10 * sign;
            power10 /= 10;
        }

        return answer;
    }

    int compare(const Rational& other) const {
        if (numerator == 0 && other.numerator == 0)
            return 0;
        if (numerator == other.numerator && denominator == other.denominator)
            return 0;
        return ((*this - other).numerator > 0 ? 1 : -1);
    }

};

std::istream& operator>>(std::istream& in, Rational& x) {
    std::string prototype;
    in >> prototype;

    int point = prototype.find('.');
    if (point != -1)
        prototype.erase(point, 1);
    else
        point = prototype.size();

    x = Rational(BigInteger(prototype));
    std::string pow10Shift = "1";
    while (point < static_cast<int>(prototype.size())) {
        pow10Shift += '0';
        ++point;
    }

    x /= Rational(BigInteger(pow10Shift));

    return in;
}

Rational operator+(const Rational& left, const Rational& right) {
    Rational answer = left;
    answer += right;
    return answer;
}

Rational operator-(const Rational& left, const Rational& right) {
    Rational answer = left;
    answer -= right;
    return answer;
}

Rational operator*(const Rational& left, const Rational& right) {
    Rational answer = left;
    answer *= right;
    return answer;
}

Rational operator/(const Rational& left, const Rational& right) {
    Rational answer = left;
    answer /= right;
    return answer;
}

bool operator<(const Rational& left, const Rational& right) {
    return left.compare(right) < 0;
}

bool operator<=(const Rational& left, const Rational& right) {
    return left.compare(right) <= 0;
}

bool operator==(const Rational& left, const Rational& right) {
    return left.compare(right) == 0;
}

bool operator>(const Rational& left, const Rational& right) {
    return left.compare(right) > 0;
}

bool operator>=(const Rational& left, const Rational& right) {
    return left.compare(right) >= 0;
}

bool operator!=(const Rational& left, const Rational& right) {
    return left.compare(right) != 0;
}

#endif //BIGINTEGER_H


#ifndef RESIDUE_H
#define RESIDUE_H

namespace Helpers {

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

} // namespace Helpers


template <unsigned N>
struct is_prime {
    static const bool value = Helpers::PrimeCheckHelper<N, 2, (N % 2) == 0, (2 * 2 > N)>::value;
};

template <>
struct is_prime<1> {
    static const bool value = false;
};

template <unsigned N>
static const bool is_prime_v = is_prime<N>::value;




template <unsigned N>
struct has_primitive_root {
    static const bool value = Helpers::primitiveRootBaseHelper<N, N % 4>::value;
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

    ~Residue() = default;

    explicit Residue(int n):
            number(Helpers::normalMod(n, M))
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
        number = Helpers::normalMod(1ll * number + other.number, M);
        return *this;
    }

    Residue& operator-=(const Residue& other) {
        return *this += -other;
    }

    Residue& operator*=(const Residue& other) {

        number = Helpers::normalMod(1ll * number * other.number, M);
        return *this;
    }

    Residue& operator/=(const Residue& other) {
        return *this *= other.getInverse();
    }

    bool operator==(const Residue& other) const {
        return number == other.number;
    }

    bool operator!=(const Residue& other) const {
        return !(*this == other);
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
        Helpers::staticAssert<is_prime_v<M> >();
        if (number == 0)
            return Residue(0);
        return pow(M - 2);
    }

    unsigned order() const {
        if (Helpers::gcd(number, M) != 1)
            return 0;
        unsigned phi = Helpers::getPhi(M);
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
        Helpers::staticAssert<has_primitive_root_v<M> >();

        std::vector<unsigned> factorization;
        unsigned phi = Helpers::getPhi(M);
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
            if (Helpers::gcd(res, M) != 1)
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

#endif // RESIDUE_H



/* NO REVIEW PART ENDS HERE
 * The new code of class Matrix is below
 */



#ifndef MATRIX_H
#define MATRIX_H

//////////////////////////////////////

template<typename Field>
void out(const Field& f);

template<>
void out(const Rational& f) {
    std::cerr << f.toString() << '\n';
}

template<unsigned P>
void out(const Residue<P>& f) {
    std::cerr << static_cast<int>(f) << '\n';
}

template <unsigned N, unsigned M, typename Field = Rational>
class Matrix;

template <unsigned N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;


template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator+
        (const Matrix<N, M, Field>& left, const Matrix<N, M, Field>& right);

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator-
        (const Matrix<N, M, Field>& left, const Matrix<N, M, Field>& right);

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator*
        (const Field& lambda, const Matrix<N, M, Field>& right);

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator*
        (const Matrix<N, M, Field>& left, const Field& lambda);

template <unsigned N, unsigned M, unsigned K, typename Field>
Matrix<N, K, Field> operator*
        (const Matrix<N, M, Field>& left, const Matrix<M, K, Field>& right);


template <unsigned N, unsigned M, typename Field>
class Matrix {
private:
    std::vector<std::vector<Field> > matrix = { N, std::vector<Field>(M, Field(0)) };

    void addRow(unsigned a, unsigned b, Field lambda) {
        for (size_t j = 0; j < M; ++j) {
            matrix[a][j] += lambda * matrix[b][j];
        }
    }

public:

    Matrix() = default;

    Matrix(const std::initializer_list<std::initializer_list<int> >& prototype):
        Matrix() {
        unsigned i = 0;
        unsigned j = 0;
        for (auto& row: prototype) {
            for (auto& element: row) {
                matrix[i][j] = Field(element);
                ++j;
            }
            ++i;
            j = 0;
        }
    }

    Matrix(const Matrix& other) {
        matrix = other.matrix;
    }

    void swap(Matrix& other) {
        std::swap(matrix, other.matrix);
    }

    Matrix& operator=(const Matrix& other) {
        Matrix copy(other);
        swap(copy);
        return *this;
    }

    const std::vector<Field>& operator[](unsigned i) const {
        return matrix[i];
    }

    std::vector<Field>& operator[](unsigned i) {
        return matrix[i];
    }

    std::vector<Field> getRow(unsigned i) const {
        return matrix[i];
    }

    std::vector<Field> getColumn(unsigned j) const {
        std::vector<Field> answer(N);
        for (unsigned i = 0; i < N; ++i) {
            answer[i] = matrix[i][j];
        }
        return answer;
    }

    Matrix& operator*=(const Field& lambda) {
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < M; ++j) {
                matrix[i][j] *= lambda;
            }
        }
        return *this;
    }

    Matrix operator-() const {
        Matrix answer(*this);
        answer *= Field(-1);
        return answer;
    }

    Matrix& operator+=(const Matrix& other) {
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < M; ++j) {
                matrix[i][j] += other.matrix[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& other) {
        *this *= Field(-1);
        *this += other;
        *this *= Field(-1);
        return *this;
    }

    bool operator==(const Matrix& other) const {
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < M; ++j) {
                if (matrix[i][j] != other.matrix[i][j])
                    return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix& other) const {
        return !(*this == other);
    }

    void Gauss() {

        unsigned squareSize = std::min(N, M);

        unsigned i = 0;
        unsigned j = 0;

        for (; i < squareSize && j < M; ++i) {
            unsigned nonZero = N;

            for (; j < M; ++j) {
                for (nonZero = i; nonZero < N; ++nonZero) {
                    if (matrix[nonZero][j] != Field(0))
                        break;
                }
                if (nonZero != N)
                    break;
            }

            if (nonZero == N)
                continue;

            if (j == M)
                break;

            if (i != nonZero)
                addRow(i, nonZero, Field(1));

            for (unsigned i1 = i + 1; i1 < N; ++i1) {
                addRow(i1, i, -matrix[i1][j] / matrix[i][j]);
            }

        }

        for (int k = static_cast<int>(squareSize) - 1; k >= 0; --k) {
            if (matrix[k][k] == Field(0))
                break;
            for (int i1 = 0; i1 < k; ++i1) {
                addRow(i1, k, -matrix[i1][k] / matrix[k][k]);
            }
            if (k == 0)
                break;
        }
    }

    Field det() const {
        Helpers::staticAssert<N == M>();

        Matrix copy(*this);
        copy.Gauss();
        Field answer(1);
        for (unsigned i = 0; i < N; ++i) {
            answer *= copy[i][i];
        }
        return answer;
    }

    Matrix<M, N, Field> transposed() const {
        Matrix<M, N, Field> answer;
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < M; ++j) {
                answer[j][i] = matrix[i][j];
            }
        }

        return answer;
    }

    unsigned rank() const {
        Matrix copy(*this);
        copy.Gauss();

        for (int i = N - 1; i >= 0; --i) {
            bool nonZero = false;
            for (unsigned j = 0; j < M; ++j) {
                if (copy[i][j] != Field(0)) {
                    nonZero = true;
                    break;
                }
            }
            if (nonZero)
                return i + 1;
        }
        return 0;
    }

    void invert() {

        Helpers::staticAssert<N == M>();
        Matrix<N, 2 * N, Field> buffer;
        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                buffer[i][j] = matrix[i][j];
            }
            buffer[i][N + i] = Field(1);
        }

        buffer.Gauss();

        for (unsigned i = 0; i < N; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                matrix[i][j] = buffer[i][j + N] / buffer[i][i];
            }
        }
    }

    Matrix inverted() const {
        Matrix copy(*this);
        copy.invert();
        return copy;
    }

    Field trace() const {
        Helpers::staticAssert<N == M>();
        Field answer(0);
        for (unsigned i = 0; i < N; ++i) {
            answer += matrix[i][i];
        }
        return answer;
    }

};


template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator+
        (const Matrix<N, M, Field>& left, const Matrix<N, M, Field>& right) {

    Matrix<N, M, Field> answer(left);
    answer += right;
    return answer;
}

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator-
        (const Matrix<N, M, Field>& left, const Matrix<N, M, Field>& right) {

    Matrix<N, M, Field> answer(left);
    answer -= right;
    return answer;
}

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator*
        (const Field& lambda, const Matrix<N, M, Field>& right) {

    Matrix<N, M, Field> answer(right);
    answer *= lambda;
    return answer;
}

template <unsigned N, unsigned M, typename Field>
Matrix<N, M, Field> operator*
        (const Matrix<N, M, Field>& left, const Field& lambda) {

    Matrix<N, M, Field> answer(left);
    answer *= lambda;
    return answer;
}

namespace Helpers {

const unsigned MIN_STRASSEN = 64;

} // namespace Helpers

template <unsigned N, unsigned M, unsigned K, typename Field>
Matrix<N, K, Field> traditionalMultiplication
        (const Matrix<N, M, Field>& left, const Matrix<M, K, Field>& right) {

    Matrix<N, K, Field> answer;
    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < K; ++j) {
            for (unsigned k = 0; k < M; ++k) {
                answer[i][j] += left[i][k] * right[k][j];
            }
        }
    }

    return answer;
}

template <unsigned N, typename Field>
Matrix<N, N, Field> StrassenMultiplication
    (const Matrix<N, N, Field>& a, const Matrix<N, N, Field>& b) {

    const unsigned L = N >> 1;

    Matrix<L, L, Field> a11;
    Matrix<L, L, Field> a12;
    Matrix<L, L, Field> a21;
    Matrix<L, L, Field> a22;
    Matrix<L, L, Field> b11;
    Matrix<L, L, Field> b12;
    Matrix<L, L, Field> b21;
    Matrix<L, L, Field> b22;

    for (unsigned i = 0; i < L; ++i) {
        for (unsigned j = 0; j < L; ++j) {
            a11[i][j] = a[i][j];
            a12[i][j] = a[i][j + L];
            a21[i][j] = a[i + L][j];
            a22[i][j] = a[i + L][j + L];

            b11[i][j] = b[i][j];
            b12[i][j] = b[i][j + L];
            b21[i][j] = b[i + L][j];
            b22[i][j] = b[i + L][j + L];
        }
    }

    Matrix<L, L, Field> p1 = StrassenMultiplication((a11 + a22), (b11 + b22));
    Matrix<L, L, Field> p2 = StrassenMultiplication((a21 + a22), b11);
    Matrix<L, L, Field> p3 = StrassenMultiplication(a11, (b12 - b22));
    Matrix<L, L, Field> p4 = StrassenMultiplication(a22, (b21 - b11));
    Matrix<L, L, Field> p5 = StrassenMultiplication((a11 + a12), b22);
    Matrix<L, L, Field> p6 = StrassenMultiplication((a21 - a11), (b11 + b12));
    Matrix<L, L, Field> p7 = StrassenMultiplication((a12 - a22), (b21 + b22));

    Matrix<L, L, Field> c11 = p1 + p4 - p5 + p7;
    Matrix<L, L, Field> c12 = p3 + p5;
    Matrix<L, L, Field> c21 = p2 + p4;
    Matrix<L, L, Field> c22 = p1 - p2 + p3 + p6;

    Matrix<N, N, Field> answer;

    for (unsigned i = 0; i < L; ++i) {
        for (unsigned j = 0; j < L; ++j) {
            answer[i][j] = c11[i][j];
            answer[i][j + L] = c12[i][j];
            answer[i + L][j] = c21[i][j];
            answer[i + L][j + L] = c22[i][j];
        }
    }

    return answer;
}

template<typename Field>
Matrix<Helpers::MIN_STRASSEN, Helpers::MIN_STRASSEN, Field> StrassenMultiplication
    (const Matrix<Helpers::MIN_STRASSEN, Helpers::MIN_STRASSEN, Field>& left,
     const Matrix<Helpers::MIN_STRASSEN, Helpers::MIN_STRASSEN, Field>& right) {

    return traditionalMultiplication(left, right);
}


namespace Helpers {

template<unsigned N, unsigned P, bool PGreaterN>
struct majPow2Helper;

template<unsigned N, unsigned P>
struct majPow2Helper<N, P, true> {
    static const unsigned value = P;
};

template<unsigned N, unsigned P>
struct majPow2Helper<N, P, false> {
    static const unsigned value = majPow2Helper<N, 2 * P, (2 * P >= N)>::value;
};

template<unsigned N>
const unsigned majPower2 = majPow2Helper<N, 1, (1 >= N)>::value;

} // namespace Helpers

template <unsigned N, unsigned M, unsigned K, typename Field>
Matrix<N, K, Field> operator*
        (const Matrix<N, M, Field>& left, const Matrix<M, K, Field>& right) {

    if ((N + M + K) / 3 <= Helpers::MIN_STRASSEN)
        return traditionalMultiplication(left, right);

    const unsigned MAX_NM = (N > M) ? N : M;
    const unsigned MAX = (MAX_NM > K) ? MAX_NM : K;

    const unsigned L = Helpers::majPower2<MAX>;

    Matrix<L, L, Field> pow2Left;
    Matrix<L, L, Field> pow2Right;

    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < M; ++j) {
            pow2Left[i][j] = left[i][j];
        }
    }

    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < K; ++j) {
            pow2Right[i][j] = right[i][j];
        }
    }

    Matrix<L, L, Field> pow2Answer(StrassenMultiplication(pow2Left, pow2Right));
    Matrix<N, K, Field> answer;

    for (unsigned i = 0; i < N; ++i) {
        for (unsigned j = 0; j < K; ++j) {
            answer[i][j] = pow2Answer[i][j];
        }
    }

    return answer;
}

template<unsigned N, typename Field>
SquareMatrix<N, Field>& operator*=(SquareMatrix<N, Field>& left, SquareMatrix<N, Field>& right) {
    return left = left * right;
}


#endif // MATRIX_H