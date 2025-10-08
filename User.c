#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct{
    int user_id;
    char name[100];
    int age;
} User;

int valid_id(int id){
    User user;
    FILE *fp= fopen("Users.txt" , "r");
    if (!fp){
        printf("File does not exist");
        return 2;
    }
    while (fscanf(fp , "%d %s %d", &user.user_id , user.name , &user.age) != EOF){
        if(user.user_id == id){
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1;
}

void addUser(){
    FILE *fp = fopen("Users.txt" , "a");
    if(!fp){
        printf("Error in file opening");
        return;
    }
    User user;
    printf("\nEnter user id: \n");
    int id;
    scanf("%d" , &id);
    if (valid_id(id)==0){
        printf("User already exists");
        return;
    }
    else if (valid_id(id)==1){
        user.user_id = id;
    }
    else{
        printf("Error");
        return;
    }
    printf("\nEnter Name: \n");
    scanf("%s" , user.name);
    printf("\nEnter age: \n");
    scanf("%d" , &user.age);
    if(user.age <=0){
        printf("Age cannot be negative");
        fclose(fp);
        return;
    }

    fprintf(fp , "%d %s %d\n" , user.user_id , user.name , user.age);
    fclose(fp);
}

void displayUsers(){
    FILE *fp = fopen("Users.txt" , "r");
    if (!fp){
        printf("Error in File Opening");
        return;
    }
    User user;
    while (fscanf(fp , "%d %s %d", &user.user_id , user.name , &user.age)!=EOF){
        printf("\nid = %d , name= %s , age = %d\n", user.user_id , user.name , user.age);
    }
    fclose(fp);
}

void modifyUserById(){
    FILE *fp = fopen("Users.txt" , "r");
    FILE *temp = fopen("Temp.txt" , "a");
    if (!fp || !fp){
        printf("Error in file Opening");
        return;
    }
    int id;
    User user;
    int found=0;
    printf("Enter User id to modify: \n");
    scanf("%d" , &id);
    while(fscanf(fp , "%d %s %d" , &user.user_id , user.name , &user.age)!= EOF){
        if(user.user_id == id){
            printf("Enter New Name:\n");
            scanf("%s" , user.name);
            printf("\nEnter New Age: ");
            scanf("%d" , &user.age);
            if(user.age <=0){
            printf("Error cannot be negative");
            fclose(fp);
            fclose(temp);
            return;
    }
            found=1;
        }
        fprintf(temp , "%d %s %d\n" , user.user_id , user.name , user.age);
}
    fclose(fp);
    fclose(temp);
    remove("Users.txt");
    rename("Temp.txt" , "Users.txt");
    if(found == 0){
        printf("\nInvalid id");
    }
    else{
        printf("User modified successfully");
    }
}

void deleteUserById(){
    FILE *fp = fopen("Users.txt" , "r");
    FILE *temp = fopen("Temp.txt" , "a");
    if (!fp || !fp){
        printf("Error in file Opening");
        return;
    }
    int id;
    User user;
    int found=0;
    printf("Enter User id to delete: \n");
    scanf("%d" , &id);
    while(fscanf(fp , "%d %s %d" , &user.user_id , user.name , &user.age)!= EOF){
        if(user.user_id == id){
            continue;
            found=1;
        }
           
        fprintf(temp , "%d %s %d\n" , user.user_id , user.name , user.age);
}
    fclose(fp);
    fclose(temp);
    remove("Users.txt");
    rename("Temp.txt" , "Users.txt");
    if(found == 0){
        printf("\nInvalid id");
    }
    else{
        printf("User deleted successfuly");
    }
}

int main() {
    int choice;
    do {
        printf("\n Choose operation which you want to perform\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Modify User by ID\n");
        printf("4. Delete User by ID\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addUser(); break;
            case 2: displayUsers(); break;
            case 3: modifyUserById(); break;
            case 4: deleteUserById(); break;
            case 5: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);

    return 0;
}