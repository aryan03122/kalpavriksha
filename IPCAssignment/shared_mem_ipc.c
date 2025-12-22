#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    int n, i;
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    int *arr = (int*)shmat(shmid, NULL, 0);

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter elements:\n");
    for (i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Before Sorting:\n");
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    if (fork() == 0) {
        sort(arr, n);
    } else {
        sleep(1);
        printf("After Sorting:\n");
        for (i = 0; i < n; i++)
            printf("%d ", arr[i]);
        printf("\n");
        shmdt(arr);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
