#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
    int age;
} User;

void createuser(){
    FILE *fp = fopen("users.txt", "a");
    if (!fp){
        printf("Error opening file");
        return;
    }
    User user;
    printf("Enter User id: ");
    scanf("%d" , &user.id);
    printf("\nEnter Name: ");
    scanf("%s" , user.name);
    printf("\nEnter age: ");
    scanf("%d" , &user.age);

    fprintf(fp , "%d %s %d\n", user.id , user.name , user.age);
    fclose(fp);

    printf("User is added");
}

void readuser(){
    FILE *fp = fopen("users.txt" , "r");
    if(!fp){
        printf("Error Opening File");
        return;
    }
    User user;
    printf("Users are as following:\n");
    while(fscanf(fp , "%d %s %d" , &user.id , user.name , &user.age)!= EOF){
         printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
}

void updateuser() {
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Error opening file!");
        return;
    }

    int id, found = 0;
    User user;
    printf("Enter User id to update: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) != EOF) {
        if (user.id == id) {
            found = 1;
            printf("Enter new Name: ");
            scanf("%s", user.name);
            printf("Enter new Age: ");
            scanf("%d", &user.age);
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }

    fclose(fp);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found==1)
        printf("User updated");
    else
        printf("User not found", id);
}

void deleteuser() {
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Error in file opening");
        return;
    }

    int id, found = 0;
    User user;
    printf("Enter User ID to delete: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) != EOF) {
        if (user.id == id) {
            found = 1;
            continue; 
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }

    fclose(fp);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted");
    else
        printf("User with ID %d not found!", id);
}

int main() {
    int choice;
    do {
        printf("\nWhat operation do you want to do in user system\n");
        printf("1. Add User\n");
        printf("2. Display users\n");
        printf("3. Update user\n");
        printf("4. Delete user\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createuser(); break;
            case 2: readuser(); break;
            case 3: updateuser(); break;
            case 4: deleteuser(); break;
            case 5: printf("Exit done"); break;
            default: printf("Invalid vhoice!");
        }
    } while (choice != 5);

    return 0;
}

//Writing this for generating pull req


