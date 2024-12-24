#include <limits.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

#define USE_RDTSCP

#define COUNT_MIN 300
#define COUNT_MAX 350

// instruction array
// 3x10 + padding
#define PREFIX_SIZE_MAX 32
// mov + nop's + mov + nop's
#define LOOP_SIZE_MAX (3 + 2 * (COUNT_MAX - 1)) * 2 * UNROLL
// sub + jne + ret
#define SUFFIX_SIZE_MAX 3 + 6 + 1
#define INST_MAX_SIZE (PREFIX_SIZE_MAX + LOOP_SIZE_MAX + SUFFIX_SIZE_MAX)

#define DATA_SIZE 64 * 1024 * 1024
#define UNROLL 16
#define INNER_LOOPS 2 * 1024
#define OUTER_LOOPS 16
#define LOOPS (UNROLL * INNER_LOOPS * OUTER_LOOPS)
#define RUNS 50

#define ADD_BYTE(pbuf, val)                                                    \
    do {                                                                       \
        *(uint8_t *)pbuf = (uint8_t)(val);                                     \
        pbuf++;                                                                \
    } while (0)
#define ADD_WORD(pbuf, val)                                                    \
    do {                                                                       \
        *(uint16_t *)pbuf = (uint16_t)(val);                                   \
        pbuf += 2;                                                             \
    } while (0)
#define ADD_DWORD(pbuf, val)                                                   \
    do {                                                                       \
        *(uint32_t *)pbuf = (uint32_t)(val);                                   \
        pbuf += 4;                                                             \
    } while (0)
#define ADD_QWORD(pbuf, val)                                                   \
    do {                                                                       \
        *(uint64_t *)pbuf = (uint64_t)(val);                                   \
        pbuf += 8;                                                             \
    } while (0)

const bool is64bit = sizeof(void *) == 8;

static inline void swap(size_t *a, size_t *b) {
    size_t t = *a;
    *a = *b;
    *b = t;
}

static void shuffle(size_t array[], size_t n) {
    if (n < 2)
        return;
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        swap(&array[i], &array[j]);
    }
}

static ssize_t find(size_t array[], size_t n, size_t value) {
    for (size_t i = 0; i < n; i++) {
        if (array[i] == value)
            return i;
    }
    return -1;
}

int init_array(void *array[], size_t count) {
    size_t *indicies = malloc(count * sizeof(indicies[0]));
    for (size_t i = 0; i < count; i++) {
        indicies[i] = i;
    }
    shuffle(indicies, count);

    size_t zero = find(indicies, count, 0);
    if (zero == -1) {
        fputs("ERROR: could not find zero\n", stderr);
        free(indicies);
        return -1;
    }
    swap(&indicies[zero], &indicies[count - 1]);

    size_t index = 0;
    for (size_t i = 0; i < count; i++) {
        array[index] = &array[indicies[i]];
        index = indicies[i];
    }

    free(indicies);
    return 0;
}

int init_instructions(uint8_t *inst, size_t count, void **data1, void **data2) {
    if (count < 3) {
        fputs("count must be >= 3\n", stderr);
        return -1;
    }

    uint8_t *pbuf = inst;
    if (!is64bit) {
        ADD_BYTE(pbuf, 0xb9); // mov ecx, data1;
        ADD_DWORD(pbuf, data1);

        ADD_BYTE(pbuf, 0xba); // mov edx, data2;
        ADD_DWORD(pbuf, data2);

        ADD_BYTE(pbuf, 0xb8); // mov eax, its;
        ADD_DWORD(pbuf, INNER_LOOPS);
    } else {
        ADD_WORD(pbuf, 0xb948); // mov rcx, data1;
        ADD_QWORD(pbuf, data1);

        ADD_WORD(pbuf, 0xba48); // mov rdx, data2;
        ADD_QWORD(pbuf, data2);

        ADD_WORD(pbuf, 0xb848); // mov rax, its;
        ADD_QWORD(pbuf, INNER_LOOPS);
    }

    while ((size_t)pbuf & 0xf) {
        ADD_BYTE(pbuf, 0x90);
    }

    uint8_t *loop_start = pbuf; // loop branch target.

    for (size_t i = 0; i < UNROLL; i++) {
        if (!is64bit) {
            ADD_BYTE(pbuf, 0x8b); // mov r32, r/m32
            ADD_BYTE(pbuf, 0x09); //		... ecx, [ecx]
        } else {
            ADD_WORD(pbuf, 0x8b48); // mov r64, r/m64
            ADD_BYTE(pbuf, 0x09);   //	... rcx, [rcx]
        }

        for (size_t j = 0; j < count - 1; j++) {
            ADD_WORD(pbuf, 0x9066);
        }

        if (!is64bit) {
            ADD_BYTE(pbuf, 0x8b); // mov r32, r/m32
            ADD_BYTE(pbuf, 0x12); //		... edx, [edx]
        } else {
            ADD_WORD(pbuf, 0x8b48); // mov r64, r/m64
            ADD_BYTE(pbuf, 0x12);   //		... rdx, [rdx]
        }

        for (size_t j = 0; j < count - 1; j++) {
            ADD_WORD(pbuf, 0x9066);
        }
    }

    ADD_WORD(pbuf, 0xe883); // sub eax
    ADD_BYTE(pbuf, 0x1);    //    1
    ADD_WORD(pbuf, 0x850f); // jne loop_start
    ADD_DWORD(pbuf, loop_start - pbuf - 4);

    ADD_BYTE(pbuf, 0xc3); // c3 ret

    mprotect(inst, (size_t)pbuf - (size_t)inst,
             PROT_READ | PROT_WRITE | PROT_EXEC);
    return 0;
}

long long run(void **data, size_t count) {
    uint8_t *inst = valloc(INST_MAX_SIZE);
    if (!inst) {
        perror("valloc");
        return -1;
    }
    if (init_instructions(inst, count, &data[0], &data[1])) {
        fputs("ERROR: could not generate instructions\n", stderr);
        free(inst);
        return -1;
    }

    void (*routine)() = (void (*)())inst;

#ifndef USE_RDTSCP
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
#else

    unsigned long long start = __rdtsc();
#endif
    for (size_t i = 0; i < OUTER_LOOPS; i++) {
        routine();
    }
#ifndef USE_RDTSCP
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    long long elapsed = (end.tv_sec - start.tv_sec) * 1000000000LL +
                        (end.tv_nsec - start.tv_nsec);
#else
    unsigned long long end = __rdtsc();
    long long elapsed = end - start;
#endif

    free(inst);
    return elapsed;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    void **data = valloc(DATA_SIZE * sizeof(data[0]));
    if (!data) {
        perror("valloc");
        return EXIT_FAILURE;
    }
    if (init_array(data, DATA_SIZE)) {
        fputs("ERROR: could not generate array\n", stderr);
        free(data);
        return EXIT_FAILURE;
    }

    for (size_t count = COUNT_MIN; count < COUNT_MAX; count++) {
        long long min_time = LLONG_MAX;
        for (size_t j = 0; j < RUNS; j++) {
            long long f_time = run(data, count);
            if (f_time == -1) {
                fprintf(stderr, "ERROR: failed run for count=%zu\n", count);
                continue;
            }

            if (f_time < min_time) {
                min_time = f_time;
            }
        }

        double min_time_per_instruction = (double)min_time / LOOPS/* / count*/;
        printf("%zu\t%f\n", count, min_time_per_instruction);
        // printf("%zu\t%lld\n", count, min_time);
    }

    return EXIT_SUCCESS;
}
