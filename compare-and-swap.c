// compare-and-swap-x86.c (x86_64 only)
#include <stdio.h>
#include <stdbool.h>

int global = 0;

static inline unsigned char compare_and_swap(int *ptr, int oldv, int newv) {
    unsigned char ret;
    __asm__ __volatile__ (
        "lock; cmpxchgl %3, %1\n\t"
        "sete %0"
        : "=q"(ret),        // ret in a byte register (for sete)
          "+m"(*ptr),       // *ptr is read-modify-written
          "+a"(oldv)        // EAX holds expected; cmpxchg overwrites it
        : "r"(newv)
        : "memory", "cc"
    );
    return ret;
}

int main(void) {
    printf("before successful cas: %d\n", global);
    int success = compare_and_swap(&global, 0, 100);
    printf("after successful cas: %d (success: %d)\n", global, success);

    printf("before failing cas: %d\n", global);
    success = compare_and_swap(&global, 0, 200);
    printf("after failing cas: %d (success: %d)\n", global, success);
    return 0;
}
