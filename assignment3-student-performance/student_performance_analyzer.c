#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100

typedef struct {
    int rollNo;
    char name[100];
    int marks[3];
} StudentResult;

typedef enum {
    GRADE_A,
    GRADE_B,
    GRADE_C,
    GRADE_D,
    GRADE_F
} Grade;

int isDuplicateRollNumber(StudentResult students[], int count, int rollNo) {
    for (int i = 0; i < count; i++) {
        if (students[i].rollNo == rollNo) {
            return 1;
        }
    }
    return 0;
}

int getValidMark(int subjectNumber) {
    int mark;
    while (1) {
        printf("Enter marks in subject %d: ", subjectNumber);
        scanf("%d", &mark);
        if (mark >= 0 && mark <= 100) {
            return mark;
        }
        printf("Invalid Marks entered, Enter again\n");
    }
}

StudentResult inputStudentDetails(StudentResult students[], int count) {
    StudentResult student;
    const int numberOfSubjects = 3;

    while (1) {
        printf("\nEnter Student Roll no.: ");
        scanf("%d", &student.rollNo);
        getchar();

        if (isDuplicateRollNumber(students, count, student.rollNo)) {
            printf("Roll number already exists! Enter a unique roll number.\n");
        } else {
            break;
        }
    }

    printf("Enter Student name: ");
    fgets(student.name, sizeof(student.name), stdin);
    student.name[strcspn(student.name, "\n")] = '\0';

    printf("Enter Marks of student in all 3 subjects\n");
    for (int i = 0; i < numberOfSubjects; i++) {
        student.marks[i] = getValidMark(i + 1);
    }

    return student;
}

float averageMarks(int arr[], int n, int *totalMarks) {
    *totalMarks = 0;
    for (int i = 0; i < n; i++) {
        *totalMarks += arr[i];
    }
    return (float)(*totalMarks) / n;
}

Grade gradeAchieved(float average) {
    if (average >= 85) return GRADE_A;
    else if (average >= 70) return GRADE_B;
    else if (average >= 50) return GRADE_C;
    else if (average >= 35) return GRADE_D;
    else return GRADE_F;
}

void printStars(Grade grade) {
    switch (grade) {
        case GRADE_A: printf("*****\n"); break;
        case GRADE_B: printf("****\n"); break;
        case GRADE_C: printf("***\n"); break;
        case GRADE_D: printf("**\n"); break;
        default: printf("\n"); break;
    }
}

void displayStudentResult(StudentResult student, int numberOfSubjects) {
    int totalMarks;
    float average = averageMarks(student.marks, numberOfSubjects, &totalMarks);
    Grade grade = gradeAchieved(average);

    printf("\nRoll: %d\n", student.rollNo);
    printf("Name: %s\n", student.name);
    printf("Total: %d\n", totalMarks);
    printf("Average: %.2f\n", average);

    char gradeChar = "ABCDF"[grade];
    printf("Grade: %c\n", gradeChar);

    if (grade != GRADE_F) {
        printf("Performance: ");
        printStars(grade);
    }
}

void printRollNumbers(StudentResult students[], int index, int total) {
    if (index == total) return;
    printf("%d ", students[index].rollNo);
    printRollNumbers(students, index + 1, total);
}

int main() {
    int numberOfStudents;
    const int numberOfSubjects = 3;
    StudentResult students[MAX_STUDENTS];

    printf("Enter Number of Students: ");
    scanf("%d", &numberOfStudents);

    if (numberOfStudents < 1 || numberOfStudents > MAX_STUDENTS) {
        printf("Invalid number of students.\n");
        return 0;
    }

    for (int i = 0; i < numberOfStudents; i++) {
        printf("\n--- Enter Details for Student %d ---\n", i + 1);
        students[i] = inputStudentDetails(students, i);
    }

    for (int i = 0; i < numberOfStudents; i++) {
        displayStudentResult(students[i], numberOfSubjects);
    }

    printf("\nList of Roll Numbers: ");
    printRollNumbers(students, 0, numberOfStudents);
    printf("\n");

    return 0;
}
