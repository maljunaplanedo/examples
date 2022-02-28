#include <assert.h>
#include <immintrin.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int n;
    scanf("%d", &n);

    int m;
    if (n % 4 == 0)
        m = n / 4;
    else
        m = n / 4 + 1;

    double a[n][4 * m];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            scanf("%lf", &a[i][j]);
        }
        for (int j = n + 1; j < 4 * m; ++j) {
            a[i][j] = 0;
        }
    }

    int order[n];
    bool used[n];

    for (int i = 0; i < n; ++i) {
        int d_index = -1;
        for (int j = 0; j < n; ++j) {
            if (a[j][i] != 0 && !used[j]) {
                d_index = j;
                break;
            }
        }
        assert(d_index != -1);
        used[d_index] = true;
        order[i] = d_index;

        double d = a[d_index][i];

        for (int j = 0; j < n; ++j) {
            if (used[j] || a[j][i] == 0)
                continue;
            __m256d l = _mm256_set1_pd(d / a[j][i]);

            for (int k = 0; k < m; ++k) {
                __m256d current = _mm256_loadu_pd(&a[j][4 * k]);
                __m256d base = _mm256_loadu_pd(&a[d_index][4 * k]);

                current = _mm256_mul_pd(current, l);
                current = _mm256_sub_pd(current, base);

                _mm256_storeu_pd(&a[j][4 * k], current);
            }
        }
    }

    for (int i = n - 1; i >= 0; --i) {
        int d_index = order[i];

        double d = a[d_index][i];
        for (int j = 0; j < n; ++j) {
            if (j == d_index || a[j][i] == 0)
                continue;

            __m256d l = _mm256_set1_pd(d / a[j][i]);
            for (int k = 0; k < m; ++k) {
                __m256d current = _mm256_loadu_pd(&a[j][4 * k]);
                __m256d base = _mm256_loadu_pd(&a[d_index][4 * k]);

                current = _mm256_mul_pd(current, l);
                current = _mm256_sub_pd(current, base);

                _mm256_storeu_pd(&a[j][4 * k], current);
            }
        }
    }

    double* answer = malloc(n * sizeof(double));

    for (int i = 0; i < n; ++i) {
        int j = order[i];
        answer[i] = a[j][n] / a[j][i];
    }

    for (int i = 0; i < n; ++i) {
        printf("%.15lf ", answer[i]);
    }

    printf("\n");

    return 0;
}

