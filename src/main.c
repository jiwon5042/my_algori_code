/* 실행: make run-c */
#include <stdio.h>
#include <string.h>
#include "sort.h"

int main(void) {
    const int input[] = {6, 8, 5, 9, 10, 1, 7, 2, 4, 3};
    const int n = (int)(sizeof(input) / sizeof(input[0]));
    const char *names[] = {"버블 정렬", "삽입 정렬", "셸 정렬"};
    void (*sorts[])(int[], int) = {bubbleSort, insertionSort, shellSort};

    for (int algorithm = 0; algorithm < 3; algorithm++) {
        int a[sizeof(input) / sizeof(input[0])];
        memcpy(a, input, sizeof(input));
        sorts[algorithm](a, n);

        printf("%s:", names[algorithm]);
        for (int i = 0; i < n; i++) {
            printf(" %d", a[i]);
        }
        printf("\n");
    }
    return 0;
}
