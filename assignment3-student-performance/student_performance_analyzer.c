#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>

typedef struct{
    int rollNo;
    char name[100];
    int marks[3];

} studentResult;

studentResult inputStudentDetails(){
    studentResult student;
    printf("\nEnter Student Roll no.: ");
    scanf("%d" , &student.rollNo);
    getchar();
    printf("\nEnter Student name: ");
    fgets(student.name , sizeof(student.name) , stdin);
    student.name[strcspn(student.name , "\n")] = '\0';
    printf("\nEnter Marks of students in all 3 subjects");
    for (int i=0 ; i<3 ; i++){
        printf("\nEnter marks in subject %d: " , i+1);
        int validMark=-1;
        scanf("%d" , &validMark);
        if(validMark >= 0 && validMark<=100 ){
            student.marks[i]= validMark;
        }
        else{
            printf("\nInvalid Marks entered, Enter again");
            i--;
        }
    }
    return student;
}

float averageMarks(int arr[] , int n , int *totalMarks){
float average;
*totalMarks=0;
for(int i=0 ; i<n ; i++){
    *totalMarks+=arr[i];
}
average= (float)(*totalMarks)/n;
return average;
}

char gradeAchieved(float average){
    if(average>=85 && average <=100){
        return 'A';
    }
    else if(average>=70 && average <85){
        return 'B';
    }
    else if(average>=50 && average <70){
        return 'C';
    }
    else if(average>=35 && average <50){
        return 'D';
    }
    else{
        return 'F';
    }
}

void printStar(float average){
    char x = gradeAchieved(average);
    if (x=='A'){
        printf("*****\n");
    }
    else if (x=='B'){
        printf("****\n");
    }
    else if (x=='C'){
        printf("***\n");
    }
    else if (x=='D'){
        printf("**\n");
    }
    else{
        printf("\n");
    }

}

void printRollno(studentResult student[] , int index , int N){
    if (index==N){
        return;
    }
    printf("%d " , student[index].rollNo);
    printRollno(student , index+1 , N);
}

int main(){
    int N;
    printf("Hello , Enter Number of Students: ");
    scanf("%d" , &N);
    if(N<1 || N>100){
        printf("\nWrong Number entered");
        return 0;
    }
    studentResult students[N];
    for(int i=0 ; i<N ; i++){
        printf("\n\nEnter Details For student %d --", i+1);
        students[i] = inputStudentDetails();
    }

    for(int i=0 ; i<N ; i++){
        int totalMarks=0;
        float average;
        average=  averageMarks(students[i].marks , 3 , &totalMarks);
        char grade = gradeAchieved(average);
        printf("Roll: %d\n", students[i].rollNo);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", totalMarks);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (grade == 'F') {
            continue;  // skip printing stars as told in problem statement, I have also made same case in function also.
        }
        printf("Performance: ");
        printStar(average);
        printf("\n\n");
    }

    printf("Roll Numbers are: ");
    printRollno(students , 0 , N);
    
}