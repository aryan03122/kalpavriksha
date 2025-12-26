#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct message {
    long type;
    int arr[20];
};

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
    key_t key = ftok("msgfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct message msg;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter elements:\n");
    for (i = 0; i < n; i++)
        scanf("%d", &msg.arr[i]);

    printf("Before Sorting:\n");
    for (i = 0; i < n; i++)
        printf("%d ", msg.arr[i]);
    printf("\n");

    msg.type = 1;
    msgsnd(msgid, &msg, sizeof(msg.arr), 0);

    if (fork() == 0) {
        msgrcv(msgid, &msg, sizeof(msg.arr), 1, 0);
        sort(msg.arr, n);
        msg.type = 2;
        msgsnd(msgid, &msg, sizeof(msg.arr), 0);
    } else {
        sleep(1);
        msgrcv(msgid, &msg, sizeof(msg.arr), 2, 0);
        printf("After Sorting:\n");
        for (i = 0; i < n; i++)
            printf("%d ", msg.arr[i]);
        printf("\n");
        msgctl(msgid, IPC_RMID, NULL);
    }
    return 0;
}
