#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 2003
#define MIN_SIZE 1
#define MAX_SIZE 1000

typedef struct LruEntry {
    int identifier;
    char *payload;
    struct LruEntry *previous;
    struct LruEntry *next;
} LruEntry;

typedef struct MapNode {
    int identifier;
    LruEntry *entryPtr;
    struct MapNode *chainNext;
} MapNode;

typedef struct LruSystem {
    int maxItems;
    int curItems;
    LruEntry *front; 
    LruEntry *rear;
    MapNode *table[TABLE_SIZE];
} LruSystem;

static int isNumber(const char *txt) {
    if (!txt || !txt[0]) return 0;
    for (int i = 0; txt[i]; i++)
        if (txt[i] < '0' || txt[i] > '9') return 0;
    return 1;
}

static bool checkKey(int k) {
    return k >= 0;
}

static bool checkValue(const char *v) {
    if (!v || !v[0]) return false;
    int n = strlen(v);
    for (int i = 0; i < n; i++)
        if ((v[i] >= 'A' && v[i] <= 'Z') || (v[i] >= 'a' && v[i] <= 'z'))
            return true;
    return false;
}

static char *cloneString(const char *src) {
    if (!src) return NULL;
    char *copy = malloc(strlen(src) + 1);
    if (copy) strcpy(copy, src);
    return copy;
}

static int hashCompute(int key) {
    unsigned int uk = (key < 0 ? -key : key);
    return uk % TABLE_SIZE;
}

static LruEntry *lookupNode(LruSystem *sys, int key) {
    int idx = hashCompute(key);
    MapNode *p = sys->table[idx];
    while (p) {
        if (p->identifier == key) return p->entryPtr;
        p = p->chainNext;
    }
    return NULL;
}

static bool addToMap(LruSystem *sys, int key, LruEntry *ent) {
    int idx = hashCompute(key);
    MapNode *nd = malloc(sizeof(MapNode));
    if (!nd) return false;
    nd->identifier = key;
    nd->entryPtr = ent;
    nd->chainNext = sys->table[idx];
    sys->table[idx] = nd;
    return true;
}

static bool removeFromMap(LruSystem *sys, int key) {
    int idx = hashCompute(key);
    MapNode *cur = sys->table[idx], *prev = NULL;
    while (cur) {
        if (cur->identifier == key) {
            if (!prev) sys->table[idx] = cur->chainNext;
            else prev->chainNext = cur->chainNext;
            free(cur);
            return true;
        }
        prev = cur;
        cur = cur->chainNext;
    }
    return false;
}

static void pushFront(LruSystem *sys, LruEntry *node) {
    node->previous = NULL;
    node->next = sys->front;
    if (sys->front) sys->front->previous = node;
    sys->front = node;
    if (!sys->rear) sys->rear = node;
}

static void detachNode(LruSystem *sys, LruEntry *node) {
    if (node->previous) node->previous->next = node->next;
    else sys->front = node->next;

    if (node->next) node->next->previous = node->previous;
    else sys->rear = node->previous;

    node->previous = node->next = NULL;
}

static void refreshToFront(LruSystem *sys, LruEntry *node) {
    if (sys->front == node) return;
    detachNode(sys, node);
    pushFront(sys, node);
}

static LruEntry *ejectRear(LruSystem *sys) {
    if (!sys->rear) return NULL;
    LruEntry *victim = sys->rear;
    detachNode(sys, victim);
    return victim;
}

static LruSystem *initCache(int cap) {
    if (cap < MIN_SIZE) return NULL;

    LruSystem *sys = malloc(sizeof(LruSystem));
    if (!sys) return NULL;

    sys->maxItems = cap;
    sys->curItems = 0;
    sys->front = sys->rear = NULL;

    for (int i = 0; i < TABLE_SIZE; i++) sys->table[i] = NULL;

    return sys;
}

static char *cacheGet(LruSystem *sys, int key) {
    if (!sys || !checkKey(key)) return NULL;

    LruEntry *hit = lookupNode(sys, key);
    if (!hit) return NULL;

    refreshToFront(sys, hit);
    return hit->payload;
}

static void cachePut(LruSystem *sys, int key, const char *val) {
    if (!sys || !checkKey(key) || !checkValue(val)) return;

    LruEntry *exist = lookupNode(sys, key);
    if (exist) {
        char *np = cloneString(val);
        if (!np) return;
        free(exist->payload);
        exist->payload = np;
        refreshToFront(sys, exist);
        return;
    }

    LruEntry *fresh = malloc(sizeof(LruEntry));
    if (!fresh) return;
    fresh->identifier = key;
    fresh->payload = cloneString(val);
    fresh->previous = fresh->next = NULL;

    if (!fresh->payload) {
        free(fresh);
        return;
    }

    if (!addToMap(sys, key, fresh)) {
        free(fresh->payload);
        free(fresh);
        return;
    }

    pushFront(sys, fresh);
    sys->curItems++;

    if (sys->curItems > sys->maxItems) {
        LruEntry *old = ejectRear(sys);
        if (old) {
            removeFromMap(sys, old->identifier);
            free(old->payload);
            free(old);
            sys->curItems--;
        }
    }
}

static void destroyCache(LruSystem *sys) {
    if (!sys) return;

    LruEntry *c = sys->front;
    while (c) {
        LruEntry *n = c->next;
        free(c->payload);
        free(c);
        c = n;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        MapNode *mp = sys->table[i];
        while (mp) {
            MapNode *nx = mp->chainNext;
            free(mp);
            mp = nx;
        }
    }
    free(sys);
}

static void stripLine(char *buf) {
    int len = strlen(buf);
    while (len > 0) {
        char ch = buf[len - 1];
        if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') {
            buf[len - 1] = '\0';
            len--;
        } else break;
    }
}

static char *dupLiteral(const char *t) {
    return cloneString(t);
}

int main() {
    LruSystem *system = NULL;
    char line[1024];

    char **responses = NULL;
    int respCount = 0, respCap = 0;

    while (1) {
        printf("Enter Command: \n");
        if (!fgets(line, sizeof(line), stdin)) break;

        stripLine(line);
        if (!line[0]) continue;

        char *cmd = strtok(line, " \t");
        if (!cmd) continue;

        if (strcmp(cmd, "createCache") == 0) {
            char *sz = strtok(NULL, " \t");
            if (!sz) {
                printf("createCache requires a positive integer capacity\n");
                continue;
            }
            if (!isNumber(sz)) {
                printf("capacity must be an integer\n");
                continue;
            }
            int s = atoi(sz);
            if (s < MIN_SIZE || s > MAX_SIZE) {
                printf("capacity must be between 1 to 1000\n");
                continue;
            }
            if (system) destroyCache(system);
            system = initCache(s);
            if (!system) printf("could not create cache (memory problem)\n");
            continue;
        }

        if (strcmp(cmd, "put") == 0) {
            char *k = strtok(NULL, " \t");
            char *d = strtok(NULL, "\n");
            if (!k || !d) {
                printf("put requires a key and a value\n");
                continue;
            }
            while (*d == ' ' || *d == '\t') d++;
            if (!isNumber(k)) {
                printf("key must be an integer\n");
                continue;
            }
            if (!checkValue(d)) {
                printf("value must be a string\n");
                continue;
            }
            if (!system) {
                printf("cache not initialized; use createCache <size>\n");
                continue;
            }
            cachePut(system, atoi(k), d);
            continue;
        }

        if (strcmp(cmd, "get") == 0) {
            char *k = strtok(NULL, " \t");
            if (!k) {
                printf("get requires a key\n");
                continue;
            }
            if (!isNumber(k)) {
                printf("key must be an integer\n");
                continue;
            }

            char *got = system ? cacheGet(system, atoi(k)) : NULL;
            char *toStore = dupLiteral(got ? got : "NULL");
            if (!toStore) {
                printf("memory allocation failed while recording get result\n");
                continue;
            }

            if (respCount + 1 > respCap) {
                int nc = (respCap == 0 ? 8 : respCap * 2);
                char **tmp = malloc(nc * sizeof(char *));
                if (!tmp) {
                    free(toStore);
                    printf("ERROR: memory allocation failed\n");
                    continue;
                }
                for (int i = 0; i < respCount; i++)
                    tmp[i] = responses[i];
                free(responses);
                responses = tmp;
                respCap = nc;
            }
            responses[respCount++] = toStore;
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            for (int i = 0; i < respCount; i++)
                printf("%s\n", responses[i]);
            for (int i = 0; i < respCount; i++)
                free(responses[i]);
            free(responses);
            destroyCache(system);
            return 0;
        }

        printf("unknown command\n");
    }

    destroyCache(system);
    return 0;
}
