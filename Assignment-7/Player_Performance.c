#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "players_data.h"

#define NAME_CAP 50
#define P_CAP 1200
#define T_CAP 50

#define R_BAT 1
#define R_BOWL 2
#define R_AR 3

typedef struct PlayerRec {
    int pid;
    char pname[NAME_CAP];
    char pteam[NAME_CAP];
    int prole;
    int pruns;
    float pavg;
    float psr;
    int pwkts;
    float peco;
    float pperf;
} PlayerRec;

typedef struct Link {
    int idx;
    struct Link *nxt;
} Link;

typedef struct Team {
    int tid;
    char tname[NAME_CAP];
    int total;
    float avgSR;
    Link *any;
    Link *bat;
    Link *bowl;
    Link *ar;
    int bc;
    int wc;
    int ac;
} Team;

PlayerRec store[P_CAP];
Team group[T_CAP];

int gp = 0;
int gt = 0;

bool inrange(int x, int a, int b) {
    return (x >= a && x <= b);
}

float getperf(PlayerRec *p) {
    if (p->prole == R_BAT)
        return (p->pavg * p->psr) / 100.0f;
    if (p->prole == R_BOWL)
        return (p->pwkts * 2.0f) + (100.0f - p->peco);
    return (p->pavg * p->psr) / 100.0f + (p->pwkts * 2.0f);
}

int roleMap(const char *r) {
    if (!strcmp(r, "Batsman")) return R_BAT;
    if (!strcmp(r, "Bowler")) return R_BOWL;
    return R_AR;
}

Link *newNode(int i) {
    Link *x = (Link*)malloc(sizeof(Link));
    if (!x) return NULL;
    x->idx = i;
    x->nxt = NULL;
    return x;
}

void appendList(Link **h, int k) {
    Link *n = newNode(k);
    if (!n) return;
    if (!*h) {
        *h = n;
        return;
    }
    Link *c = *h;
    while (c->nxt) c = c->nxt;
    c->nxt = n;
}

void insertPerf(Link **h, int k) {
    Link *n = newNode(k);
    if (!n) return;
    if (!*h) {
        *h = n;
        return;
    }
    float pv = store[k].pperf;
    Link *c = *h, *p = NULL;
    while (c && store[c->idx].pperf >= pv) {
        p = c;
        c = c->nxt;
    }
    if (!p) {
        n->nxt = *h;
        *h = n;
    } else {
        p->nxt = n;
        n->nxt = c;
    }
}

void loadPlayers() {
    gp = 0;
    for (int i = 0; i < playerCount && i < P_CAP; i++) {
        store[i].pid = players[i].id;
        strncpy(store[i].pname, players[i].name, NAME_CAP - 1);
        strncpy(store[i].pteam, players[i].team, NAME_CAP - 1);
        store[i].pname[NAME_CAP-1] = '\0';
        store[i].pteam[NAME_CAP-1] = '\0';
        store[i].prole = roleMap(players[i].role);
        store[i].pruns = players[i].totalRuns;
        store[i].pavg = players[i].battingAverage;
        store[i].psr = players[i].strikeRate;
        store[i].pwkts = players[i].wickets;
        store[i].peco = players[i].economyRate;
        store[i].pperf = getperf(&store[i]);
        gp++;
    }
}

void setupTeams() {
    gt = 0;
    for (int i = 0; i < teamCount && i < T_CAP; i++) {
        group[i].tid = i + 1;
        strncpy(group[i].tname, teams[i], NAME_CAP - 1);
        group[i].tname[NAME_CAP-1] = '\0';

        group[i].total = 0;
        group[i].avgSR = 0;
        group[i].any = NULL;
        group[i].bat = NULL;
        group[i].bowl = NULL;
        group[i].ar = NULL;

        group[i].bc = group[i].wc = group[i].ac = 0;

        gt++;
    }

    for (int i = 0; i < gp; i++) {
        for (int t = 0; t < gt; t++) {
            if (!strcmp(store[i].pteam, group[t].tname)) {
                appendList(&group[t].any, i);
                group[t].total++;

                if (store[i].prole == R_BAT) {
                    insertPerf(&group[t].bat, i);
                    group[t].bc++;
                } else if (store[i].prole == R_BOWL) {
                    insertPerf(&group[t].bowl, i);
                    group[t].wc++;
                } else {
                    insertPerf(&group[t].ar, i);
                    group[t].ac++;
                }
                break;
            }
        }
    }

    for (int t = 0; t < gt; t++) {
        float s = 0;
        int c = 0;
        Link *m = group[t].any;
        while (m) {
            int x = m->idx;
            if (store[x].prole == R_BAT || store[x].prole == R_AR) {
                s += store[x].psr;
                c++;
            }
            m = m->nxt;
        }
        group[t].avgSR = (c > 0) ? (s / c) : 0;
    }
}

void swapInt(int a[], int i, int j) {
    int t = a[i];
    a[i] = a[j];
    a[j] = t;
}

void sortTeams(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int s = i;
        for (int j = i + 1; j < n; j++) {
            if (group[arr[j]].avgSR > group[arr[s]].avgSR)
                s = j;
        }
        if (s != i)
            swapInt(arr, i, s);
    }
}

int teamFind(int id) {
    int l = 0, h = gt - 1;
    while (l <= h) {
        int m = (l + h) / 2;
        if (group[m].tid == id) return m;
        if (group[m].tid < id) l = m + 1;
        else h = m - 1;
    }
    return -1;
}

void showTeam() {
    int id;
    printf("Enter Team ID (1..%d): ", gt);
    if (scanf("%d", &id) != 1) {
        while (getchar()!='\n');
        return;
    }
    if (!inrange(id, 1, gt)) return;

    int idx = teamFind(id);
    if (idx < 0) return;

    Team *T = &group[idx];
    printf("\nPlayers of Team %s (ID %d)\n", T->tname, T->tid);
    printf("====================================================================================\n");
    printf("ID   Name                      Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    Link *p = T->any;
    while (p) {
        PlayerRec *q = &store[p->idx];
        const char *r = (q->prole == R_BAT) ? "Batsman" : (q->prole == R_BOWL) ? "Bowler" : "All-Rounder";

        printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
            q->pid, q->pname, r, q->pruns, q->pavg, q->psr, q->pwkts, q->peco, q->pperf);
        p = p->nxt;
    }

    printf("====================================================================================\n");
    printf("Total Players: %d\n", T->total);
    printf("Average Batting Strike Rate: %.2f\n", T->avgSR);
}

void showTeamSort() {
    int arr[T_CAP];
    for (int i = 0; i < gt; i++) arr[i] = i;
    sortTeams(arr, gt);

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID   Team Name            Avg Bat SR   Total Players\n");
    printf("=========================================================\n");

    for (int i = 0; i < gt; i++) {
        int t = arr[i];
        printf("%d %s %.1f %d\n",
            group[t].tid, group[t].tname, group[t].avgSR, group[t].total);
    }
    printf("=========================================================\n");
}

void showTopK() {
    int id, ro, k;
    printf("Enter Team ID: ");
    if (scanf("%d", &id) != 1) { while(getchar()!='\n'); return; }

    if (!inrange(id,1,gt)) return;

    printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
    if (scanf("%d", &ro) != 1) { while(getchar()!='\n'); return; }

    printf("Enter number of players: ");
    if (scanf("%d", &k) != 1) { while(getchar()!='\n'); return; }

    int idx = teamFind(id);
    if (idx < 0) return;

    int buf[P_CAP];
    int c = 0;

    for (int i = 0; i < gp; i++) {
        if (store[i].prole == ro &&
            strcmp(store[i].pteam, group[idx].tname) == 0)
            buf[c++] = i;
    }

    if (c == 0) return;

    for (int i = 0; i < c - 1; i++) {
        int s = i;
        for (int j = i + 1; j < c; j++) {
            if (store[buf[j]].pperf > store[buf[s]].pperf)
                s = j;
        }
        if (s != i) swapInt(buf, i, s);
    }

    if (k > c) k = c;

    printf("Top %d players of Team %s:\n", k, group[idx].tname);
    printf("====================================================================================\n");
    printf("ID   Name                      Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    for (int i = 0; i < k; i++) {
        PlayerRec *q = &store[buf[i]];
        const char *r = (q->prole == R_BAT) ? "Batsman" : (q->prole == R_BOWL) ? "Bowler" : "All-Rounder";

        printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
            q->pid, q->pname, r, q->pruns, q->pavg, q->psr, q->pwkts, q->peco, q->pperf);
    }

    printf("====================================================================================\n");
}

void showRoleAll() {
    int ro;
    printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
    if (scanf("%d", &ro) != 1) { while(getchar()!='\n'); return; }

    int buf[P_CAP];
    int c = 0;

    for (int i = 0; i < gp; i++)
        if (store[i].prole == ro)
            buf[c++] = i;

    for (int i = 0; i < c - 1; i++) {
        int s = i;
        for (int j = i + 1; j < c; j++)
            if (store[buf[j]].pperf > store[buf[s]].pperf)
                s = j;
        if (s != i) swapInt(buf, i, s);
    }

    printf("====================================================================================\n");
    printf("ID   Name                      Team            Role        Runs   Avg   SR    Wkts  ER   Perf.Index\n");
    printf("====================================================================================\n");

    for (int i = 0; i < c; i++) {
        PlayerRec *q = &store[buf[i]];
        const char *r = (q->prole == R_BAT) ? "Batsman" : (q->prole == R_BOWL) ? "Bowler" : "All-Rounder";

        printf("%-4d %-25s %-10s %-6d %-5.1f %-5.1f %-5d %-4.1f %-8.2f\n",
            q->pid, q->pname, q->pteam, r, q->pruns, q->pavg, q->psr, q->pwkts, q->peco, q->pperf);
    }

    printf("====================================================================================\n");
}

void printMenu() {
    printf("\n1. Display Players of a Specific Team\n");
    printf("2. Display Teams by Average Batting Strike Rate\n");
    printf("3. Display Top K Players of a Specific Team by Role\n");
    printf("4. Display All Players of specific role Across All Teams by performance index\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    loadPlayers();
    setupTeams();

    while (1) {
        printMenu();
        int op;
        if (scanf("%d", &op) != 1) {
            while(getchar()!='\n');
            continue;
        }
        if (!inrange(op, 1, 5)) continue;

        if (op == 1) showTeam();
        else if (op == 2) showTeamSort();
        else if (op == 3) showTopK();
        else if (op == 4) showRoleAll();
        else if (op == 5) break;
    }
    return 0;
}