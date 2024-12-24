#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#define RUNS_COUNT 5

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
    fprintf(stderr, "x = %Lf, n = %lld\n", x, n);

    struct tms start, end;
    double time_min = 0.0;
    long clocks_per_sec = sysconf(_SC_CLK_TCK);

    for (long long i = 0; i < RUNS_COUNT; i++) {
        times(&start);
        long double exponent = ExponentCalculation(x, n);
        times(&end);

        double taken_time = (double)(end.tms_utime - start.tms_utime) / clocks_per_sec;
        fprintf(stderr, "exponent(%Lf) = %.10Le\n", x, exponent);
        fprintf(stderr, "Run N%lld took %f seconds to complete\n", i + 1, taken_time);
        if (time_min == 0.0 || taken_time < time_min) {
            time_min = taken_time;
        }
    }

    printf("Min time: %f seconds\n", time_min);
    return EXIT_SUCCESS;
}
