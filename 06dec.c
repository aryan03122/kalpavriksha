#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Contact {
    char *name;
    char *phone;
    struct Contact *next;
};

void parseCommand(char *line, char *cmd, char *name, char *phone) {

    char *rest = line;
    char *token;

    token = strtok_r(rest, " \n", &rest);
    strcpy(cmd, token);

    if (strcmp(cmd, "ADD") == 0) {
        token = strtok_r(NULL, " \n", &rest);
        strcpy(name, token);

        token = strtok_r(NULL, " \n", &rest);
        strcpy(phone, token);
    }
    else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "SEARCH") == 0) {
        token = strtok_r(NULL, " \n", &rest);
        strcpy(name, token);
        phone[0] = '\0';
    } 
    else {
        name[0] = '\0';
        phone[0] = '\0';
    }
}

void addContact(struct Contact **head, char *name, char *phone) {

    struct Contact *newNode = malloc(sizeof(struct Contact));

    newNode->name = malloc(strlen(name) + 1);
    newNode->phone = malloc(strlen(phone) + 1);

    strcpy(newNode->name, name);
    strcpy(newNode->phone, phone);
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    struct Contact *mover = *head;
    while (mover->next != NULL)
        mover = mover->next;

    mover->next = newNode;
}

void deleteContact(struct Contact **head, char *name) {

    if (*head == NULL) return;

    while (*head != NULL && strcmp((*head)->name, name) == 0) {
        struct Contact *delNode = *head;
        *head = (*head)->next;
        free(delNode->name);
        free(delNode->phone);
        free(delNode);
    }

    if (*head == NULL) return;

    struct Contact *mover = *head;
    while (mover->next != NULL) {
        if (strcmp(mover->next->name, name) == 0) {
            struct Contact *delNode = mover->next;
            mover->next = delNode->next;
            free(delNode->name);
            free(delNode->phone);
            free(delNode);
        } else {
            mover = mover->next;
        }
    }
}

void searchContact(struct Contact *head, char *name) {

    int found = 0;
    while (head != NULL) {
        if (strcmp(head->name, name) == 0) {
            printf("%s -> %s\n", head->name, head->phone);
            found = 1;
        }
        head = head->next;
    }

    if (!found)
        printf("Not found.\n");
}

void printContacts(struct Contact *head) {

    if (head == NULL) {
        printf("No contacts.\n");
        return;
    }

    while (head != NULL) {
        printf("%s -> %s\n", head->name, head->phone);
        head = head->next;
    }
}

void sortContacts(struct Contact **head) {

    if (*head == NULL || (*head)->next == NULL)
        return;

    int swapped = 1;

    while (swapped) {
        swapped = 0;
        struct Contact *curr = *head;

        while (curr->next != NULL) {

            if (strcmp(curr->name, curr->next->name) > 0) {

                char *tempName = curr->name;
                curr->name = curr->next->name;
                curr->next->name = tempName;

                char *tempPhone = curr->phone;
                curr->phone = curr->next->phone;
                curr->next->phone = tempPhone;

                swapped = 1;
            }

            curr = curr->next;
        }
    }
}

void freeAll(struct Contact **head) {

    struct Contact *curr = *head;
    while (curr != NULL) {
        struct Contact *nextNode = curr->next;
        free(curr->name);
        free(curr->phone);
        free(curr);
        curr = nextNode;
    }

    *head = NULL;
}

int main() {

    struct Contact *head = NULL;
    char line[100], cmd[20], name[50], phone[20];

    printf("Contact Manager Ready (Commands: ADD, DELETE, SEARCH, SORT, PRINT, EXIT)\n");

    while (1) {
        printf("\n> ");
        fgets(line, sizeof(line), stdin);

        parseCommand(line, cmd, name, phone);

        if (strcmp(cmd, "ADD") == 0) {
            addContact(&head, name, phone);
        }
        else if (strcmp(cmd, "DELETE") == 0) {
            deleteContact(&head, name);
        }
        else if (strcmp(cmd, "SEARCH") == 0) {
            searchContact(head, name);
        }
        else if (strcmp(cmd, "PRINT") == 0) {
            printContacts(head);
        }
        else if (strcmp(cmd, "SORT") == 0) {
            sortContacts(&head);
        }
        else if (strcmp(cmd, "EXIT") == 0) {
            freeAll(&head);
            printf("Program terminated.\n");
            break;
        }
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
