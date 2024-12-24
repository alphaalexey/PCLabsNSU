#include <benchmark/benchmark.h>

#define X 2147483648
#define N 1000000

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

long double ExponentCalculation_ld(long double x, long double n) {
    long double result = 1.0;
    long double prev = x;

    for (long double i = 2; i < n; i++) {
        prev *= x;
        prev /= i;
        result += prev;
    }

    return result;
}

long double ExponentCalculation_ii(long double x, long long n) {
    long double result = 1.0;
    long double prev = x;
    long double ii = 1.0;

    for (long long i = 2; i < n; i++) {
        ii++;
        prev *= x;
        prev /= ii;
        result += prev;
    }

    return result;
}

static void BM_ExponentCalculation(benchmark::State& state) {
    long double x = state.range(0);
    long long n = state.range(1);
    for (auto _ : state) {
        benchmark::DoNotOptimize(ExponentCalculation(x, n));
    }
}
BENCHMARK(BM_ExponentCalculation)->Args({X, N})->Unit(benchmark::kMillisecond);

static void BM_ExponentCalculation_ld(benchmark::State& state) {
    long double x = state.range(0);
    long double n = state.range(1);
    for (auto _ : state) {
        benchmark::DoNotOptimize(ExponentCalculation_ld(x, n));
    }
}
BENCHMARK(BM_ExponentCalculation_ld)->Args({X, N})->Unit(benchmark::kMillisecond);

static void BM_ExponentCalculation_ii(benchmark::State& state) {
    long double x = state.range(0);
    long long n = state.range(1);
    for (auto _ : state) {
        benchmark::DoNotOptimize(ExponentCalculation_ii(x, n));
    }
}
BENCHMARK(BM_ExponentCalculation_ii)->Args({X, N})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
