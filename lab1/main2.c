#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long double ExponentCalculation(long double x, long long n) {
    long double result = 1.0;
    long double prev = x;

    for (long long i = 2; i < n; i++) {
        prev *= x;
        prev /= i;
        result += prev;
    }

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: <degree> <number of terms>\n");
        return EXIT_SUCCESS;
    }

    long double x = strtold(argv[1], NULL);
    long long n = atoll(argv[2]);
    // fprintf(stderr, "x = %Lf, n = %lld\n", x, n);

    long double exponent = ExponentCalculation(x, n);

    printf("exponent(%Lf) = %.10Le\n", x, exponent);

    return EXIT_SUCCESS;
}
