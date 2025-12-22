#include <stdio.h>
#include <unistd.h>

void sort(int a[], int n) {
    int i, j, t;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1]) {
                t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
}

int main() {
    int fd[2], n, i;
    pipe(fd);

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

    if (fork() == 0) {
        close(fd[1]);
        read(fd[0], arr, sizeof(arr));
        sort(arr, n);
        write(fd[0], arr, sizeof(arr));
    } else {
        close(fd[0]);
        write(fd[1], arr, sizeof(arr));
        sleep(1);
        read(fd[1], arr, sizeof(arr));

        printf("After Sorting:\n");
        for (i = 0; i < n; i++)
            printf("%d ", arr[i]);
        printf("\n");
    }
    return 0;
}
