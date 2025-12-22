#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sort(int a[], int n) {
    int i, j, t;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (a[j] > a[j + 1]) {
                t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
        }
    }
}

int main() {
    int n, i;
    FILE *fp;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter elements:\n");
    for (i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Before Sorting:\n");
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    fp = fopen("data.txt", "w");
    for (i = 0; i < n; i++)
        fprintf(fp, "%d ", arr[i]);
    fclose(fp);

    if (fork() == 0) {
        fp = fopen("data.txt", "r");
        for (i = 0; i < n; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        sort(arr, n);

        fp = fopen("data.txt", "w");
        for (i = 0; i < n; i++)
            fprintf(fp, "%d ", arr[i]);
        fclose(fp);
        exit(0);
    }
    fp = fopen("data.txt", "r");
    printf("After Sorting:\n");
    for (i = 0; i < n; i++) {
        fscanf(fp, "%d", &arr[i]);
        printf("%d ", arr[i]);
    }
    printf("\n");
    fclose(fp);

    return 0;
}