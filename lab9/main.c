#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <time.h>

// #define SIZE 32 * 1024
// #define SIZE 512 * 1024
#define SIZE 16 * 1024 * 1024

#define OFFSET 16 * 1024 * 1024

#define LOOPS 100
#define RUNS 50

long f(size_t fragment_count) {
    const size_t offset = OFFSET / sizeof(size_t);
    const size_t size = SIZE / sizeof(size_t);

    const size_t array_size = offset * fragment_count;
    size_t *data = calloc(array_size, sizeof(data[0]));

    for (size_t i = 0; i < fragment_count; i++) {
        for (size_t j = 0; j < size / fragment_count; j++) {
            data[offset * i + j] = (offset * (i + 1) + j) % array_size;
        }
    }

    // for (size_t i = 0; i < array_size; i++) {
    //     printf("%zu ", data[i]);
    // }
    // printf("\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    volatile size_t summ = 0;
    size_t element = 0;
    for (size_t i = 0; i < size * LOOPS; i++) {
        element = data[element];
        summ += element;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    free(data);

    if (summ == 0) {
        printf("Error occured!\n");
        exit(EXIT_FAILURE);
    }

    return (end.tv_sec - start.tv_sec) * 1000000000 +
           (end.tv_nsec - start.tv_nsec);
}

int main() {
    for (size_t fragments = 2; fragments <= 32; fragments++) {
        long min_time = LONG_MAX;
        for (size_t j = 0; j < RUNS; j++) {
            long curr_time = f(fragments) / LOOPS;
            if (curr_time < min_time) {
                min_time = curr_time;
            }
        }
        printf("%zu\t%ld\n", fragments, min_time);
    }

    return EXIT_SUCCESS;
}
