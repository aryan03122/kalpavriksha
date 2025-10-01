#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int rightoperator(char c){
    if (c=='+' || c== '-' || c=='*' || c=='/'){
        return 1;
    }
    else{
        return 0;
    }
}

int precedence(char op){
    if(op=='*' || op=='/'){
        return 2;
    }
    else if (op=='+' || op== '-'){
        return 1;
    }
    else{ 
        return 0;
    }
}

int calculate(int a , int b , char op ,  int *error){
    switch (op){
        case '+': 
        return a+b;
        case '-': 
        return a-b;
        case '*': 
        return a*b;
        case '/': 
        if (b == 0){
            *error=1;
            return 0;
        }
        else{
            return a/b;
        }
    }
    *error=2; 
    return 0;
}

int solve(char* expr , int *error){
    int nums[100], numtop= -1;
    char operators[100];
    int optop= -1;
    
    int len= strlen(expr);

    for (int i=0 ; i<len ; ){
        if(isspace(expr[i])){
            i++;
            continue;
        }
        if(isdigit(expr[i])){
            int val=0;
            while (i<len && isdigit(expr[i])){
                val= val*10 + (expr[i++]- '0');
            }
            nums[++numtop]= val;
        }
        else if (rightoperator(expr[i])){
            while (optop>=0 && precedence(operators[optop])>= precedence(expr[i])){
                int b= nums[numtop--];
                int a = nums[numtop--];
                char op = operators[optop--];
                nums[++numtop]= calculate(a , b , op , error);
                if (*error==1){
                    return 0;
                }
            }
            operators[++optop] = expr[i++];
            }
            else {
                *error=2;
                return 0;
            }
    }
     while (optop >= 0) {
        int b = nums[numtop--];
        int a = nums[numtop--];
        char op = operators[optop--];
        nums[++numtop] = calculate(a, b, op, error);
        if (*error) return 0;
    }

    return nums[numtop];
}

int main(void) {
    char expr[100];
    printf("Enter expression: ");
    fgets(expr, 100, stdin);
    expr[strcspn(expr, "\n")] = '\0'; 

    int error = 0;
    int result = solve(expr, &error);

    if (error == 1) {
        printf("Division by zero is not allowed");
    } else if (error == 2) {
        printf("Invalid expression");
    } else {
        printf("Result: %d", result);
    }

    return 0;
}
