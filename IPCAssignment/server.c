#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

pthread_mutex_t lock;

void* client_handler(void* arg) {
    int sock = *(int*)arg;
    int choice, amount, balance;
    FILE *fp;

    pthread_mutex_lock(&lock);
    fp = fopen("accountDB.txt", "r");
    fscanf(fp, "%d", &balance);
    fclose(fp);
    pthread_mutex_unlock(&lock);

    read(sock, &choice, sizeof(choice));

    if (choice == 1) {
        read(sock, &amount, sizeof(amount));
        pthread_mutex_lock(&lock);
        if (amount <= balance) {
            balance -= amount;
            fp = fopen("accountDB.txt", "w");
            fprintf(fp, "%d", balance);
            fclose(fp);
            write(sock, &balance, sizeof(balance));
        } else {
            int fail = -1;
            write(sock, &fail, sizeof(fail));
        }
        pthread_mutex_unlock(&lock);
    }
    else if (choice == 2) {
        read(sock, &amount, sizeof(amount));
        pthread_mutex_lock(&lock);
        balance += amount;
        fp = fopen("accountDB.txt", "w");
        fprintf(fp, "%d", balance);
        fclose(fp);
        pthread_mutex_unlock(&lock);
        write(sock, &balance, sizeof(balance));
    }
    else if (choice == 3) {
        write(sock, &balance, sizeof(balance));
    }

    close(sock);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    pthread_t tid;

    pthread_mutex_init(&lock, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        pthread_create(&tid, NULL, client_handler, &client_fd);
    }
    return 0;
}
