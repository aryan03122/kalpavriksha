#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TOTAL_BLOCKS 1024
#define SECTOR_SIZE 512
#define MAX_FILE_SECTORS 32
#define NAME_LENGTH 50
#define INPUT_LENGTH 1024
#define DIR_DEPTH_LIMIT 128

typedef struct StorageUnit {
    int sectorId;
    struct StorageUnit *previous;
    struct StorageUnit *next;
} StorageUnit;

typedef enum { FILE_KIND, DIR_KIND } NodeType;

typedef struct Node {
    char label[NAME_LENGTH + 1];
    NodeType type;
    struct Node *next;
    struct Node *prev;
    struct Node *parent;
    union {
        struct { struct Node *firstChild; } dir;
        struct { int size; int sectors[MAX_FILE_SECTORS]; } file;
    };
} Node;

char diskArray[TOTAL_BLOCKS][SECTOR_SIZE];
Node *rootDir = NULL;
Node *activeDir = NULL;
StorageUnit *freeStart = NULL;
StorageUnit *freeEnd = NULL;
int freeCount = 0;

void bootFileSystem();
void releaseAllMemory();
void purgeTree(Node *n);
void restoreSector(int id);
Node *locateNode(char *name);
bool detachNode(Node *n);
void composePath(char *buf, size_t size);
int identifyCommand(char *cmd);
int tokenizeCommand(char *input, char *cmd, char *arg);
void execMakeDir(char *arg);
void execCreateFile(char *arg);
void execWriteFile(char *arg);
void execReadFile(char *arg);
void execRemoveFile(char *arg);
void execRemoveDir(char *arg);
void execList();
void execChangeDir(char *arg);
void execPrintDir();
void execDiskUsage();

int identifyCommand(char *cmd) {
    if (!strcmp(cmd, "mkdir")) return 1;
    if (!strcmp(cmd, "create")) return 2;
    if (!strcmp(cmd, "write")) return 3;
    if (!strcmp(cmd, "read")) return 4;
    if (!strcmp(cmd, "delete")) return 5;
    if (!strcmp(cmd, "rmdir")) return 6;
    if (!strcmp(cmd, "ls")) return 7;
    if (!strcmp(cmd, "cd")) return 8;
    if (!strcmp(cmd, "pwd")) return 9;
    if (!strcmp(cmd, "df")) return 10;
    if (!strcmp(cmd, "exit")) return 11;
    return 0;
}

int tokenizeCommand(char *input, char *cmd, char *arg) {
    int idx = strcspn(input, " ");
    int len = (idx > NAME_LENGTH) ? NAME_LENGTH : idx;
    strncpy(cmd, input, len); cmd[len] = '\0';
    char *a = input + idx;
    while (*a == ' ') a++;
    strcpy(arg, a);
    return idx;
}

Node *locateNode(char *name) {
    Node *h = activeDir->dir.firstChild;
    if (!h) return NULL;
    Node *cur = h;
    do {
        if (!strcmp(cur->label, name)) return cur;
        cur = cur->next;
    } while (cur != h);
    return NULL;
}

bool detachNode(Node *n) {
    if (!n->parent) return false;
    Node *p = n->parent;
    Node *pr = n->prev;
    Node *nx = n->next;
    if (n == nx) { p->dir.firstChild = NULL; return true; }
    pr->next = nx; nx->prev = pr;
    if (p->dir.firstChild == n) p->dir.firstChild = nx;
    return true;
}

void restoreSector(int id) {
    StorageUnit *blk = malloc(sizeof(StorageUnit));
    blk->sectorId = id;
    blk->next = NULL;
    if (!freeEnd) { blk->previous = NULL; freeStart = freeEnd = blk; }
    else { blk->previous = freeEnd; freeEnd->next = blk; freeEnd = blk; }
    freeCount++;
}

void composePath(char *buf, size_t size) {
    if (activeDir == rootDir) { snprintf(buf, size, "/"); return; }
    Node *path[DIR_DEPTH_LIMIT];
    int depth = 0;
    Node *cur = activeDir;
    while (cur != rootDir && depth < DIR_DEPTH_LIMIT) { path[depth++] = cur; cur = cur->parent; }
    char *ptr = buf; char *end = buf + size;
    for (int i = depth - 1; i >= 0; i--) {
        int rem = end - ptr;
        if (rem <= 1) break;
        int w = snprintf(ptr, rem, "/%s", path[i]->label);
        if (w < 0 || w >= rem) break;
        ptr += w;
    }
    if (ptr == buf) snprintf(buf, size, "/");
}

void execDiskUsage() {
    int used = TOTAL_BLOCKS - freeCount;
    printf("Total: %d\nUsed: %d\nFree: %d\nUsage: %.2f%%\n", TOTAL_BLOCKS, used, freeCount, (float)used * 100 / TOTAL_BLOCKS);
}

void execPrintDir() {
    char p[INPUT_LENGTH]; composePath(p, sizeof(p)); printf("%s\n", p);
}

void execList() {
    Node *h = activeDir->dir.firstChild;
    if (!h) { printf("(empty)\n"); return; }
    Node *cur = h;
    do {
        printf("%s%s\n", cur->label, cur->type == DIR_KIND ? "/" : "");
        cur = cur->next;
    } while (cur != h);
}

void execMakeDir(char *arg) {
    if (!arg || !*arg) { printf("Missing name.\n"); return; }
    if (strcmp(arg, ".") == 0 || strcmp(arg, "..") == 0 || strchr(arg, '/')) { printf("Invalid name.\n"); return; }
    if (locateNode(arg)) { printf("Exists.\n"); return; }
    Node *d = malloc(sizeof(Node));
    strncpy(d->label, arg, NAME_LENGTH); d->label[NAME_LENGTH] = '\0';
    d->type = DIR_KIND; d->parent = activeDir; d->dir.firstChild = NULL;
    Node *h = activeDir->dir.firstChild;
    if (!h) {
        activeDir->dir.firstChild = d;
        d->next = d->prev = d;
    } else {
        Node *t = h->prev;
        t->next = d; d->prev = t;
        d->next = h; h->prev = d;
    }
    printf("Directory '%s' created.\n", arg);
}

void execCreateFile(char *arg) {
    if (!arg || !*arg) { printf("Missing name.\n"); return; }
    if (strcmp(arg, ".") == 0 || strcmp(arg, "..") == 0 || strchr(arg, '/')) { printf("Invalid name.\n"); return; }
    if (locateNode(arg)) { printf("Exists.\n"); return; }
    Node *f = malloc(sizeof(Node));
    strncpy(f->label, arg, NAME_LENGTH); f->label[NAME_LENGTH] = '\0';
    f->type = FILE_KIND; f->parent = activeDir; f->file.size = 0;
    for (int i = 0; i < MAX_FILE_SECTORS; i++) f->file.sectors[i] = -1;
    Node *h = activeDir->dir.firstChild;
    if (!h) {
        activeDir->dir.firstChild = f;
        f->next = f->prev = f;
    } else {
        Node *t = h->prev;
        t->next = f; f->prev = t;
        f->next = h; h->prev = f;
    }
    printf("File '%s' created.\n", arg);
}

void execWriteFile(char *arg) {
    char name[NAME_LENGTH + 1], content[INPUT_LENGTH];
    if (!arg || !*arg) { printf("Missing args.\n"); return; }
    char *space = strchr(arg, ' ');
    if (!space) { printf("Invalid format.\n"); return; }
    int len = space - arg;
    strncpy(name, arg, len); name[len] = '\0';
    char *text = space + 1;
    if (*text == '"' || *text == '\'') {
        char quote = *text;
        char *end = strchr(text + 1, quote);
        if (!end) { printf("Unclosed quote.\n"); return; }
        int clen = end - text - 1;
        strncpy(content, text + 1, clen); content[clen] = '\0';
    } else { printf("Quoted content required.\n"); return; }
    Node *f = locateNode(name);
    if (!f) { printf("Not found.\n"); return; }
    if (f->type == DIR_KIND) { printf("Cannot write directory.\n"); return; }
    for (int i = 0; i < MAX_FILE_SECTORS; i++) {
        if (f->file.sectors[i] != -1) restoreSector(f->file.sectors[i]);
        else break;
        f->file.sectors[i] = -1;
    }
    int size = strlen(content);
    int need = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    if (need > MAX_FILE_SECTORS) { printf("Too large.\n"); return; }
    if (freeCount < need) { printf("No space.\n"); return; }
    int offset = 0;
    for (int i = 0; i < need; i++) {
        StorageUnit *blk = freeStart;
        freeStart = blk->next;
        if (freeStart) freeStart->previous = NULL; else freeEnd = NULL;
        int id = blk->sectorId;
        f->file.sectors[i] = id;
        int bytes = (size - offset > SECTOR_SIZE) ? SECTOR_SIZE : size - offset;
        memcpy(diskArray[id], content + offset, bytes);
        offset += bytes;
        free(blk);
        freeCount--;
    }
    f->file.size = size;
    printf("Written %d bytes.\n", size);
}

void execReadFile(char *arg) {
    Node *f = locateNode(arg);
    if (!f) { printf("Not found.\n"); return; }
    if (f->type == DIR_KIND) { printf("Cannot read directory.\n"); return; }
    if (f->file.size == 0) { printf("(empty)\n"); return; }
    int rem = f->file.size;
    for (int i = 0; i < MAX_FILE_SECTORS; i++) {
        int id = f->file.sectors[i];
        if (id == -1) break;
        int bytes = (rem > SECTOR_SIZE) ? SECTOR_SIZE : rem;
        fwrite(diskArray[id], 1, bytes, stdout);
        rem -= bytes;
        if (rem <= 0) break;
    }
    printf("\n");
}

void execRemoveFile(char *arg) {
    Node *f = locateNode(arg);
    if (!f) { printf("Not found.\n"); return; }
    if (f->type == DIR_KIND) { printf("Use rmdir.\n"); return; }
    for (int i = 0; i < MAX_FILE_SECTORS; i++) {
        if (f->file.sectors[i] != -1) restoreSector(f->file.sectors[i]);
        else break;
    }
    detachNode(f);
    free(f);
    printf("File removed.\n");
}

void execRemoveDir(char *arg) {
    Node *d = locateNode(arg);
    if (!d) { printf("Not found.\n"); return; }
    if (d->type == FILE_KIND) { printf("Use delete.\n"); return; }
    if (d->dir.firstChild) { printf("Not empty.\n"); return; }
    detachNode(d);
    free(d);
    printf("Directory removed.\n");
}

void execChangeDir(char *arg) {
    if (!arg || !*arg) { printf("Missing name.\n"); return; }
    if (strcmp(arg, "/") == 0) { activeDir = rootDir; printf("Moved to /\n"); return; }
    if (strcmp(arg, "..") == 0) {
        if (activeDir->parent) activeDir = activeDir->parent;
        char p[INPUT_LENGTH]; composePath(p, sizeof(p)); printf("Moved to %s\n", p); return;
    }
    Node *t = locateNode(arg);
    if (!t) { printf("Not found.\n"); return; }
    if (t->type == FILE_KIND) { printf("File, not directory.\n"); return; }
    activeDir = t;
    char p[INPUT_LENGTH]; composePath(p, sizeof(p)); printf("Moved to %s\n", p);
}

void purgeTree(Node *n) {
    if (!n) return;
    if (n->type == DIR_KIND && n->dir.firstChild) {
        Node *c = n->dir.firstChild;
        c->prev->next = NULL;
        while (c) {
            Node *next = c->next;
            purgeTree(c);
            c = next;
        }
    }
    free(n);
}

void releaseAllMemory() {
    purgeTree(rootDir);
    StorageUnit *cur = freeStart;
    while (cur) { StorageUnit *n = cur->next; free(cur); cur = n; }
    printf("Memory freed.\n");
}

void bootFileSystem() {
    memset(diskArray, 0, sizeof(diskArray));
    rootDir = malloc(sizeof(Node));
    if (!rootDir) { printf("Memory error.\n"); exit(1); }
    strcpy(rootDir->label, "/");
    rootDir->type = DIR_KIND;
    rootDir->parent = NULL;
    rootDir->next = rootDir->prev = rootDir;
    rootDir->dir.firstChild = NULL;
    activeDir = rootDir;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        StorageUnit *blk = malloc(sizeof(StorageUnit));
        blk->sectorId = i;
        blk->next = NULL;
        if (!freeStart) {
            blk->previous = NULL;
            freeStart = freeEnd = blk;
        } else {
            blk->previous = freeEnd;
            freeEnd->next = blk;
            freeEnd = blk;
        }
        freeCount++;
    }
}

int main() {
    printf("Compact VirtualFS ready. Type 'exit' to quit.\n");
    bootFileSystem();
    char cmd[NAME_LENGTH + 1], arg[INPUT_LENGTH], buf[INPUT_LENGTH], path[INPUT_LENGTH];
    int code;
    do {
        composePath(path, sizeof(path));
        printf("%s> ", path);
        cmd[0] = arg[0] = '\0';
        if (!fgets(buf, sizeof(buf), stdin)) {
            code = 11;
            printf("\n");
        } else {
            if (!strchr(buf, '\n')) {
                printf("Input overflow.\n");
                int c; while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            buf[strcspn(buf, "\n")] = 0;
            if (buf[0] == '\0') continue;
            tokenizeCommand(buf, cmd, arg);
            code = identifyCommand(cmd);
        }
        switch (code) {
            case 1: execMakeDir(arg); break;
            case 2: execCreateFile(arg); break;
            case 3: execWriteFile(arg); break;
            case 4: execReadFile(arg); break;
            case 5: execRemoveFile(arg); break;
            case 6: execRemoveDir(arg); break;
            case 7: execList(); break;
            case 8: execChangeDir(arg); break;
            case 9: execPrintDir(); break;
            case 10: execDiskUsage(); break;
            case 11: break;
            default: printf("Unknown command: %s\n", cmd);
        }
    } while (code != 11);
    releaseAllMemory();
    return 0;
}