// “User Command Interpreter with Data History”

// You are building a small command-based system like a CLI.

// Each input line will look like one of these:

// ADD 105
// ADD 210
// ADD 320
// REMOVE
// UNDO
// PRINT

// Commands Meaning:
// Command	Action
// ADD <number>	Add the number to a dynamic array (like history)
// REMOVE	Remove the last added number
// UNDO	Reverse the last change (ADD → undo remove, REMOVE → put number back)
// PRINT	Print all numbers from oldest to newest
// 🔥 Requirements:

// Store the numbers in a dynamic array (not linked list).

// Use malloc, realloc, and free.

// Keep a stack of operations to support UNDO.

// Use:

// Single pointer in parsing input

// Double pointer for modifying the dynamic array (size + memory)

// Triple pointer for UNDO stack manipulation

// 🧩 Data Structures:
// struct History {
//     int *arr;       // dynamic array of numbers
//     int size;
//     int capacity;
// };

// struct Action {
//     char type[10];  // "ADD" or "REMOVE"
//     int value;      // value affected
// };

// struct UndoStack {
//     struct Action *actions; 
//     int top;
//     int capacity;
// };

// 🧠 Your Tasks

// You will fill these functions:

// 1️⃣ Parse Input (single pointer logic)
// void parseCommand(char *line, char *cmd, int *value) {
//     // Extract command and optional number using strtok_r (single pointer logic)
// }

// 2️⃣ Add Value (double pointer for array resizing)
// void addValue(struct History *h, int value) {
//     // Use realloc if full
//     // Insert value
// }

// 3️⃣ Remove Last Value (double pointer)
// int removeValue(struct History *h) {
//     // Remove last element and return it
//     // If empty return -1
// }

// 4️⃣ Push Action into Undo Stack (double + realloc)
// void pushUndo(struct UndoStack *st, char *type, int value) {
//     // Push new action into stack
// }

// 5️⃣ Pop Action from Undo Stack (triple pointer style)
// int popUndo(struct UndoStack **st, struct Action *out) {
//     // Pop last action
//     // Return 1 if valid, 0 if empty
// }

// 6️⃣ Undo Last Operation
// void undo(struct History *h, struct UndoStack **st) {
//     // If last action was ADD → remove value
//     // If last action was REMOVE → add value back
// }

// 7️⃣ Print History
// void printHistory(struct History *h) {
//     // Print values cleanly
// }

// 🧪 Example Input:
// ADD 10
// ADD 20
// ADD 30
// REMOVE
// UNDO
// PRINT

// Output:
// 10 20 30

// ✨ Small Comment Explanations (for your code)
// Feature	Comment Summary
// Parsing	Splits command and optional number from input text
// Dynamic Array	Resizes using realloc when full
// Undo Stack	Stores history of operations to reverse later
// Triple Pointer	Allows modifying stack structure from function
// Undo	Replays reverse action based on stored history
// 🧯 BONUS Challenge
// Add one extra command:
// CLEAR
// Which deletes all history in O(1) using pointer reset (not loops).

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct History {
    int *arr;       // dynamic array of numbers
    int size;
    int capacity;
};

struct Action {
    char type[10];  // "ADD" or "REMOVE"
    int value;      // value affected
};

struct UndoStack {
    struct Action *actions; 
    int top;
    int capacity;
};

void parseCommand(char *line, char *cmd, int *value) {
    // Extract command and optional number using strtok_r (single pointer logic)
char *buffer = line;
char *token;
token = strtok_r(buffer , " " , &buffer);
//*cmd = token; (miskake, in string we use function to assign)
strcpy(cmd , token);

//if(*cmd == "Add"){    (Again same mistake, we need to use function)
    if(strcmp(cmd , "ADD") == 0){
    token = strtok_r(NULL , " " , &buffer);
    *value = atoi(token); 
    }

    else{
    *value = -1;
    }
}

void addValue(struct History *h, int value) {
    // Use realloc if full
    // Insert value

    if(h -> size == h -> capacity){
        h -> capacity *=2;
        int *temp = realloc(h -> arr , h -> capacity * sizeof(int));
        if(!temp){
            return;
        }
        h -> arr = temp;
    }
    h -> arr[h -> size] = value;
    h -> size++;
}

int removeValue(struct History *h) {
    if(h -> size == 0){
        return -1;
    }
    int val = h -> arr[h -> size-1];
    h -> size--;
    return val;
}

void pushUndo(struct UndoStack *st, char *type, int value) {     

    if(st -> top ==  st -> capacity){
        st -> capacity *= 2;
        struct Action *temp = realloc(st -> actions , st -> capacity * sizeof(struct Action));
        if(!temp){
            printf("Memory Allocation Failed");
            return ;
        }
        st -> actions = temp;
    }
    strcpy(st -> actions[st -> top]. type ,  type);
    st -> actions[st -> top].value = value;

    st -> top++;
}

int popUndo(struct UndoStack *st, struct Action *out) {  //chatgpt

    if (st->top == 0) return 0; 

    st->top--;
    *out = st->actions[st->top];
    return 1;
}


void undo(struct History *h, struct UndoStack *st) {

    struct Action action;
    
    if (!popUndo(st, &action)) {
        printf("Nothing to undo.\n");
        return;
    }

    if (strcmp(action.type, "ADD") == 0) {
        removeValue(h); 
    } 
    else if (strcmp(action.type, "REMOVE") == 0) {
        addValue(h, action.value);
    }
}


void printHistory(struct History *h) {
    for (int i = 0; i < h->size; i++)
        printf("%d ", h->arr[i]);
    printf("\n");
}

int main() {       //chatgpt

    struct History history = {malloc(2 * sizeof(int)), 0, 2};
    struct UndoStack stack = {malloc(2 * sizeof(struct Action)), 0, 2};

    char input[50], cmd[10];
    int val;

    printf("\nCommands:\n");
    printf("ADD <num> | REMOVE | UNDO | PRINT | EXIT\n\n");

    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);

        if (strcmp(input, "EXIT\n") == 0) break;

        parseCommand(input, cmd, &val);

        if (strcmp(cmd, "ADD") == 0) {
            addValue(&history, val);
            pushUndo(&stack, "ADD", val);
        }
        else if (strcmp(cmd, "REMOVE") == 0) {
            int removed = removeValue(&history);
            if (removed != -1)
                pushUndo(&stack, "REMOVE", removed);
        }
        else if (strcmp(cmd, "UNDO") == 0) {
            undo(&history, &stack);
        }
        else if (strcmp(cmd, "PRINT") == 0) {
            printHistory(&history);
        }
        else {
            printf("Invalid command.\n");
        }
    }

    free(history.arr);
    free(stack.actions);

    printf("\nProgram terminated.\n");
    return 0;
}
