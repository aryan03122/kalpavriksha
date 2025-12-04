// 🔥 Problem Statement

// Write a program that takes input in this format:

// Name: Aryan, Age: 21, Score: 90
// Name: Aakansha, Age: 22, Score: 88
// Name: Ajay, Age: 20, Score: 95


// You must:

// Extract the name, age, and score from each line.

// Convert age and score from string → integer.

// Compute the average score.

// Encode each name so every letter shifts by +1 in ASCII.

// 👉 Example:

// Aryan → Bszbo
// Ajay → Bkbz


// Finally output:

// Encoded Name | Age | Score
// --------------------------
// Bszbo         21     90
// Bblblotib     22     88
// Bkbz          20     95

// Average Score: 91.00

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

struct Node{
    char name[100];
    int age;
    int score;
    struct Node *next;
};

void insert(struct Node **head , char *name , int age , int score){
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    strcpy(newNode -> name , name);
    newNode -> age = age;
    newNode -> score = score;
    newNode -> next = NULL;

    if(*head == NULL){
        *head = newNode;
        return;
    }
    struct Node *temp = *head;

    while(temp -> next != NULL){
        temp = temp -> next;
    }
    temp -> next = newNode;
}

void encode(char *s){
    for(int i = 0 ; s[i]!= '\0' ; i++){
        if(isalpha(s[i])){
            s[i]++;
        }
    }
}

void printList(struct Node *head) {
    printf("\nEncoded Name | Age | Score\n");
    printf("----------------------------------\n");

    while (head) {
        printf("%-12s   %d      %d\n", head->name, head->age, head->score);
        head = head->next;
    }
}

void freeList(struct Node *head){
    while(head){
        struct Node *temp = head;
        head = head -> next;
        free(temp);
    }
}

float computeAvg(struct Node *head){
    int count = 1;
    int sum = 0;
    struct Node *temp = head;
    while(temp -> next != NULL){
        sum = temp -> score;
        count++;
        temp = temp -> next;
    }
    float score = (float)sum / count;
    return score;
}

void printList(struct Node *head) {
    while (head) {
        printf("%-12s   %d      %d\n", head->name, head->age, head->score);
        head = head->next;
    }
}

void reverseNames(char *ch){
    int n = strlen(ch);
    int start = 0 , end = n-1;
    while(start < end){
        char temp = ch[start];
        ch[start] = ch[end];
        ch[end] = temp;
        start++;
        end--;
    }
}

void sortByMarks(struct Node *head) {      //by chatgpt
    if (!head) return;

    struct Node *i, *j;
    for (i = head; i->next != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->score < j->score) {
                // swap entire node data instead of pointers
                int tempScore = i->score;
                i->score = j->score;
                j->score = tempScore;

                int tempAge = i->age;
                i->age = j->age;
                j->age = tempAge;

                char tempName[100];
                strcpy(tempName, i->name);
                strcpy(i->name, j->name);
                strcpy(j->name, tempName);
            }
        }
    }
}

void findDuplicateNames(struct Node *head) {
    struct Node *i, *j;
    printf("Duplicate Encoded Names:\n");
    int found = 0;
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (strcmp(i->name, j->name) == 0) {
                printf("%s\n", i->name);
                found = 1;
            }
        }
    }

    if (!found) printf("None\n");
}

void encryptScores(struct Node *head) {
    while (head) {
        head->score = (head->score * 135) % 100;  
        head = head->next;
    }
}

int main(){
    char line[200];
    struct Node *ll = (struct Node *)malloc(sizeof(struct Node));
    while(fgets(line , sizeof(line) , stdin)){
        char name[100];
        int age , score;
        char *rest = line;    //for strtok_r
        char *token;          //for strtok_r
        token = strtok_r(rest, ":," , &rest);
        token = strtok_r(NULL , ":," , &rest);
        while(*token ==' '){
            token++;
        }
        strcpy(name , token);

        token = strtok_r(NULL , ":," , &rest);
        token = strtok_r(NULL , ":," , "&rest");
        age = atoi(token);
        
        token = strtok_r(NULL, ":,", &rest);
        token = strtok_r(NULL, ":,", &rest);
        score = atoi(token);

        encode(name);
        insert(&ll, name, age, score);
    }


    printList(ll);
    printf("\nAverage Score: %.2f\n", computeAverage(ll));

    freeList(ll);
    return 0;
}


// ✅ Why do we use strtok_r()?

// strtok_r() is used to split a string into smaller parts (tokens) based on delimiters.
// It is a safer version of strtok() because it keeps its own internal position using a separate pointer, making it reentrant and suitable for multiple parsing operations.

// ✅ Why do we pass the original string only once and NULL afterward?

// On the first call, we pass the original string so tokenization can start.
// On subsequent calls, we pass NULL because strtok_r() already stored where it stopped earlier using saveptr, and continues from that position.

// ✅ What does saveptr (rest) do?

// saveptr acts like a bookmark.
// It remembers the remaining part of the string after each token is extracted, so the next call continues parsing from the correct position.

// ✅ Why do we trim spaces only for name but not for age/score?

// String fields keep whitespace, so we manually remove leading spaces from the name.
// However, atoi() automatically ignores leading spaces, so trimming is not required for numeric fields like age and score.