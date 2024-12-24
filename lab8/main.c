#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define START 1 * 1024
#define DELTA 1 * 1024
#define MAX_SIZE 1024 * 1024

#define LOOPS 4
#define RUNS 6

void swap(size_t *a, size_t *b) {
    size_t t = *a;
    *a = *b;
    *b = t;
}

void shuffle(size_t array[], size_t n) {
    if (n < 2) {
        return;
    }
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        swap(&array[i], &array[j]);
    }
}

size_t find(size_t array[], size_t n, size_t value) {
    for (size_t i = 0; i < n; i++) {
        if (array[i] == value) {
            return i;
        }
    }
    return -1;
}

long f(size_t size) {
    size_t *indicies = malloc(size * sizeof(indicies[0]));
    for (size_t i = 0; i < size; i++) {
        indicies[i] = i;
    }
    shuffle(indicies, size);

    size_t zero = find(indicies, size, 0);
    if (zero == -1) {
        printf("ERROR: could not find zero\n");

        free(indicies);
        exit(EXIT_FAILURE);
    }
    swap(&indicies[zero], &indicies[size - 1]);

    size_t *data = malloc(size * sizeof(data[0]));
    size_t index = 0;
    for (size_t i = 0; i < size; i++) {
        data[index] = indicies[i];
        index = indicies[i];
    }
    free(indicies);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    uint64_t sum = 0;
    size_t prev = 0;
    for (size_t i = 0; i < size * LOOPS; i++) {
        size_t current = data[prev];
        sum += current;

        prev = current;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    if (sum != (size * (size - 1) / 2) * LOOPS) {
        printf("ERROR: sum is %ld, size=%ld\n", sum, size);
        for (size_t i = 0; i < size; i++) {
            printf("%ld ", data[i]);
        }
        printf("\n");

        free(data);
        exit(EXIT_FAILURE);
    }

    free(data);

    return (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
}

int main(int argc, char *argv[]) {
    for (size_t size = START; size < MAX_SIZE; size += DELTA) {
        double kib = size / 1024.0;
        if (size % 1024 == 0) {
            fprintf(stderr, "%f KiB\n", kib);
        }

        double min_time = 1/.0; // +inf
        for (size_t j = 0; j < RUNS; j++) {
            long f_time = f(size);
            double curr_time = (double)f_time / LOOPS / size;
            if (curr_time < min_time) {
                min_time = curr_time;
            }
        }
        printf("%f\t%f\n", kib, min_time);
    }

    return EXIT_SUCCESS;
}
