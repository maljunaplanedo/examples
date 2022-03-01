#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <complex>
#include <algorithm>

#include "biginteger.h"
#include "residue.h"


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
