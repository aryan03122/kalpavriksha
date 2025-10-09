#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_INPUT_SIZE 100

int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int calculate(int a, int b, char op, int *error) {
    long long result = 0;

    switch (op) {
        case '+': result = (long long)a + b; break;
        case '-': result = (long long)a - b; break;
        case '*': result = (long long)a * b; break;
        case '/':
            if (b == 0) {
                *error = 1; 
                return 0;
            }
            result = a / b;
            break;
        default:
            *error = 2;
            return 0;
    }

    if (result > INT_MAX || result < INT_MIN) {
        *error = 3; 
        return 0;
    }

    return (int)result;
}

int evaluateExpression(char *expr, int *error) {
    int nums[MAX_INPUT_SIZE], numtop = -1;
    char operators[MAX_INPUT_SIZE];
    int optop = -1;

    int len = strlen(expr);
    int i = 0;
    int lastWasOperator = 1; 

    while (i < len) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

    
        if (isdigit(expr[i]) ||
            ((expr[i] == '-' || expr[i] == '+') && (lastWasOperator || i == 0))) {

            int sign = 1;
            if (expr[i] == '-') {
                sign = -1;
                i++;
            } else if (expr[i] == '+') {
                sign = 1;
                i++;
            }

            while (i < len && isspace(expr[i])) i++; 

            if (i >= len || !isdigit(expr[i])) {
                *error = 2;
                return 0;
            }

            long long val = 0;
            while (i < len && isdigit(expr[i])) {
                val = val * 10 + (expr[i++] - '0');
                if (val * sign > INT_MAX || val * sign < INT_MIN) {
                    *error = 3;
                    return 0;
                }
            }

            nums[++numtop] = (int)(val * sign);
            lastWasOperator = 0;
        }
        else if (isOperator(expr[i])) {
            if (lastWasOperator && expr[i] != '+' && expr[i] != '-') {
                *error = 2;
                return 0;
            }

            while (optop >= 0 && precedence(operators[optop]) >= precedence(expr[i])) {
                int b = nums[numtop--];
                int a = nums[numtop--];
                char op = operators[optop--];
                nums[++numtop] = calculate(a, b, op, error);
                if (*error) return 0;
            }

            operators[++optop] = expr[i++];
            lastWasOperator = 1;
        }
        else {
            *error = 2;
            return 0;
        }
    }

    if (lastWasOperator || numtop < 0) {
        *error = 2;
        return 0;
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

int main() {
    char expr[MAX_INPUT_SIZE];

    printf("Enter expression: ");
    fgets(expr, MAX_INPUT_SIZE, stdin);
    expr[strcspn(expr, "\n")] = '\0';

    int error = 0;
    int result = evaluateExpression(expr, &error);

    switch (error) {
        case 1:
            printf("Error: Division by zero is not allowed.\n");
            break;
        case 2:
            printf("Error: Invalid expression.\n");
            break;
        case 3:
            printf("Error: Integer overflow.\n");
            break;
        default:
            printf("Result: %d\n", result);
            break;
    }

    return 0;
}
