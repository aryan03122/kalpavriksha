#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int *getAddress(int *arr, int n, int i, int j) {
    return (arr + i * n + j);
}

void printArray(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", *getAddress(arr, n, i, j));
        }
        printf("\n");
    }
}

void generateRandomMatrix(int *arr, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *getAddress(arr, n, i, j) = rand() % 256;
        }
    }
}

void transposeMatrix(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {  
            int *a = getAddress(arr, n, i, j);
            int *b = getAddress(arr, n, j, i);
            int temp = *a;
            *a = *b;
            *b = temp;
        }
    }
}

void reverseRows(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        int j = 0;
        int k = n - 1;
        while (j < k) {
            int *a = getAddress(arr, n, i, j);
            int *b = getAddress(arr, n, i, k);
            int temp = *a;
            *a = *b;
            *b = temp;
            j++;
            k--;
        }
    }
}

void rotate90Degrees(int *arr, int n) {
    transposeMatrix(arr, n);
    reverseRows(arr, n);
}

void applyingSmoothingFilter(int *arr, int n) {
    
    int *prevRow = (int *)calloc(n, sizeof(int));
    int *currRow = (int *)malloc(n * sizeof(int));
    int *nextRow = (int *)malloc(n * sizeof(int));
    int *outRow = (int *)malloc(n * sizeof(int));

    
    for (int j = 0; j < n; j++) {
        currRow[j] = *getAddress(arr, n, 0, j);
    }
    if (n > 1) {
        for (int j = 0; j < n; j++) {
            nextRow[j] = *getAddress(arr, n, 1, j);
        }
    } else {
        memset(nextRow, 0, n * sizeof(int));
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0, count = 0;
            for (int ki = -1; ki <= 1; ki++) {
                int li = i + ki;
                if (li < 0 || li >= n) continue;

                for (int kj = -1; kj <= 1; kj++) {
                    int lj = j + kj;
                    if (lj < 0 || lj >= n) continue;
                    int val;
                    if (li == i - 1) {
                        val = prevRow[lj];
                    } else if (li == i) {
                        val = currRow[lj];
                    } else {
                        val = nextRow[lj];
                    }
                    sum += val;
                    count++;
                }
            }
            outRow[j] = sum / count;
        }

        for (int j = 0; j < n; j++) {
            *getAddress(arr, n, i, j) = outRow[j];
        }

        
        memcpy(prevRow, currRow, n * sizeof(int));

        if (i + 2 < n) {
            memcpy(currRow, nextRow, n * sizeof(int));
            for (int j = 0; j < n; j++) {
                nextRow[j] = *getAddress(arr, n, i + 2, j);
            }
        } else {
            memcpy(currRow, nextRow, n * sizeof(int));
        }
    }

    free(prevRow);
    free(currRow);
    free(nextRow);
    free(outRow);
}

int main() {
    int n;
    printf("Enter size N for N x N matrix: ");
    scanf("%d", &n);

    int *arr = (int *)malloc(n * n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    generateRandomMatrix(arr, n);
    printf("\nGenerated array:\n");
    printArray(arr, n);

    rotate90Degrees(arr, n);
    printf("\nArray after rotating 90 degrees:\n");
    printArray(arr, n);

    applyingSmoothingFilter(arr, n);
    printf("\nArray after smoothing effect:\n");
    printArray(arr, n);

    free(arr);
    return 0;
}
