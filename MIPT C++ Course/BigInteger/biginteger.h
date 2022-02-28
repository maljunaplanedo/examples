#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <algorithm>

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

        block d = BLOCK_SIZE / (mutableOther.buffer[n - 1] + 1);
        multiplyByShort(d);
        mutableOther.multiplyByShort(d);

        if (buffer.size() == m + n)
            buffer.push_back(0);

        for (int j = m; j >= 0; --j) { // D2, D7

            // D3
            operationBuffer qv = 1 * buffer[j + n] * BLOCK_SIZE
                                 + buffer[j + n - 1];
            operationBuffer rv = qv % mutableOther.buffer[n - 1];
            qv /= mutableOther.buffer[n - 1];

            while (qv == BLOCK_SIZE || qv * mutableOther.buffer[n - 2] >
                                       BLOCK_SIZE * rv + buffer[j + n - 2]) {

                --qv;
                rv += mutableOther.buffer[n - 1];

                if (rv >= BLOCK_SIZE)
                    break;
            }

            // D4

            BigInteger otherCopy = mutableOther;
            otherCopy.multiplyByShort(qv);

            BigInteger thisCopy;
            thisCopy.buffer.clear();
            for (size_t i = j; i <= j + n; ++i) {
                thisCopy.buffer.push_back(buffer[i]);
            }
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
                thisCopy += mutableOther;

                if (thisCopy.buffer.size() == n + 2)
                    thisCopy.buffer.pop_back();
            }

            for (size_t i = j; i <= j + n; ++i) {
                buffer[i] = thisCopy.buffer[i - j];
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
        if (*this == 0)
            return "0";
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

    double getDouble(size_t precision) const {
        double power = sign;
        double answer = 0;

        bool oddPrecision = precision & 1;
        if (oddPrecision)
            precision--;

        for (size_t i = (precision >> 1); i < buffer.size(); ++i) {
            answer += power * (buffer[i] % 10);
            answer += 10 * power * (buffer[i] / 10);
            power *= BLOCK_SIZE;
        }

        power = sign / 10.0;

        for (int i = (precision >> 1) - 1; i >= 0; --i) {
            answer += power * (buffer[i] % 10) / 10;
            answer += power * (buffer[i] / 10);
            power /= BLOCK_SIZE;
        }

        if (oddPrecision)
            answer /= 10;

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

        if (answer.size() == precision || (answer[0] == '-' && answer.size() == precision + 1)) {
            answer = '0' + answer;
            if (answer[1] == '-')
                std::swap(answer[0], answer[1]);
        }
        if (precision != 0)
            answer.insert(answer.begin() + answer.size() - precision, '.');

        return answer;
    }

    explicit operator double() const {
        const size_t PRECISION = 10;
        return integerQuotient(PRECISION).getDouble(PRECISION);
    }

    int compare(const Rational& other) const {
        if (numerator == other.numerator && denominator == other.denominator)
            return 0;
        return ((*this - other).numerator > 0 ? 1 : -1);
    }

};

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