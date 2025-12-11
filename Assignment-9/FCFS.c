#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 100
#define NAME_MAX 50
#define LINE_MAX 120
#define NUM_MAX 8

typedef enum {
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED,
    STATE_FORCED
} ProcessState;

typedef struct ProcessControlBlock {
    int pid;
    char name[NAME_MAX];
    int arrival;
    int total_cpu;
    int remaining_cpu;
    int executed_cpu;
    int io_start;
    int io_duration;
    int remaining_io;
    int current_io;
    int completion_time;
    ProcessState state;
    int ticks_executed;
    int io_enqueue_flag;
    bool forced;
    struct ProcessControlBlock *next_in_table;
} ProcessControlBlock;

typedef struct QueueNode {
    ProcessControlBlock *proc;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

typedef struct SchedulerEvent {
    int target_pid;
    int time;
    struct SchedulerEvent *next;
} SchedulerEvent;

ProcessControlBlock *ProcessTable[TABLE_SIZE];
SchedulerEvent *EventHead = NULL;

int hash_index(int pid) {
    int idx = pid % TABLE_SIZE;
    if (idx < 0) idx += TABLE_SIZE;
    return idx;
}

void table_insert(ProcessControlBlock *pcb) {
    int idx = hash_index(pcb->pid);
    pcb->next_in_table = ProcessTable[idx];
    ProcessTable[idx] = pcb;
}

ProcessControlBlock *table_lookup(int pid) {
    int idx = hash_index(pid);
    ProcessControlBlock *cur = ProcessTable[idx];
    while (cur) {
        if (cur->pid == pid) return cur;
        cur = cur->next_in_table;
    }
    return NULL;
}

void queue_init(Queue *q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

void enqueue(Queue *q, ProcessControlBlock *pcb) {
    QueueNode *node = malloc(sizeof(QueueNode));
    node->proc = pcb;
    node->next = NULL;
    if (!q->rear) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
}

ProcessControlBlock *dequeue(Queue *q) {
    if (!q->front) return NULL;
    QueueNode *node = q->front;
    ProcessControlBlock *pcb = node->proc;
    q->front = node->next;
    if (!q->front) q->rear = NULL;
    free(node);
    q->size--;
    return pcb;
}

int remove_from_queue(Queue *q, int pid) {
    QueueNode *cur = q->front;
    QueueNode *prev = NULL;
    while (cur) {
        if (cur->proc->pid == pid) {
            if (!prev) q->front = cur->next;
            else prev->next = cur->next;
            if (cur == q->rear) q->rear = prev;
            free(cur);
            q->size--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

ProcessControlBlock *create_process(char *name, int pid, int cpu, int ioStart, int ioDur) {
    ProcessControlBlock *p = malloc(sizeof(ProcessControlBlock));
    strcpy(p->name, name);
    p->pid = pid;
    p->arrival = 0;
    p->total_cpu = cpu;
    p->remaining_cpu = cpu;
    p->executed_cpu = 0;
    p->io_start = ioStart;
    p->io_duration = ioDur;
    p->remaining_io = 0;
    p->current_io = 0;
    p->completion_time = 0;
    p->state = STATE_READY;
    p->ticks_executed = 0;
    p->io_enqueue_flag = 0;
    p->forced = false;
    p->next_in_table = NULL;
    return p;
}

bool all_digits(char *s) {
    if (!s || *s == '\0') return false;
    for (int i = 0; s[i]; i++) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

bool dash_or_digits(char *s) {
    if (strcmp(s, "-") == 0) return true;
    return all_digits(s);
}

bool valid_name(char *s) {
    if (!s) return false;
    int i = 0;
    while (s[i] == ' ') i++;
    return s[i] != '\0';
}

bool validate_process_fields(char *name, char *sPid, char *sCpu, char *sIoStart, char *sIoDur) {
    if (!valid_name(name)) return false;
    if (!all_digits(sPid)) return false;
    if (!all_digits(sCpu)) return false;
    if (!dash_or_digits(sIoStart)) return false;
    if (!dash_or_digits(sIoDur)) return false;
    return true;
}

bool validate_kill_fields(char *sPid, char *sTime) {
    if (!all_digits(sPid)) return false;
    if (!all_digits(sTime)) return false;
    return true;
}

bool non_empty_line(char *ln) {
    return ln && ln[0] != '\0';
}

void read_processes(int total, Queue *ready) {
    int count = 0;
    char line[LINE_MAX];
    while (count < total) {
        if (!fgets(line, LINE_MAX, stdin)) continue;
        line[strcspn(line, "\n")] = '\0';
        if (!non_empty_line(line)) continue;
        char pname[NAME_MAX], sPid[NUM_MAX], sCpu[NUM_MAX], sIoStart[NUM_MAX], sIoDur[NUM_MAX];
        int scanned = sscanf(line, "%s %s %s %s %s", pname, sPid, sCpu, sIoStart, sIoDur);
        if (scanned != 5) continue;
        if (!validate_process_fields(pname, sPid, sCpu, sIoStart, sIoDur)) continue;
        int pid = atoi(sPid);
        int cpu = atoi(sCpu);
        int ioStart = (strcmp(sIoStart, "-") == 0) ? -1 : atoi(sIoStart);
        int ioDur = (strcmp(sIoDur, "-") == 0) ? 0 : atoi(sIoDur);
        ProcessControlBlock *p = create_process(pname, pid, cpu, ioStart, ioDur);
        table_insert(p);
        enqueue(ready, p);
        count++;
    }
}

void insert_event(int pid, int time) {
    SchedulerEvent *ev = malloc(sizeof(SchedulerEvent));
    ev->target_pid = pid;
    ev->time = time;
    ev->next = NULL;
    if (!EventHead || EventHead->time > time) {
        ev->next = EventHead;
        EventHead = ev;
        return;
    }
    SchedulerEvent *cur = EventHead;
    while (cur->next && cur->next->time <= time) cur = cur->next;
    ev->next = cur->next;
    cur->next = ev;
}

void read_kill_events(int total) {
    int count = 0;
    char line[LINE_MAX];
    while (count < total) {
        if (!fgets(line, LINE_MAX, stdin)) continue;
        line[strcspn(line, "\n")] = '\0';
        if (!non_empty_line(line)) continue;
        char sPid[NUM_MAX], sTime[NUM_MAX];
        int scanned = sscanf(line, "KILL %s %s", sPid, sTime);
        if (scanned != 2) continue;
        if (!validate_kill_fields(sPid, sTime)) continue;
        int pid = atoi(sPid);
        int time = atoi(sTime);
        insert_event(pid, time);
        count++;
    }
}

void process_kill_events(int current_time, Queue *ready, Queue *waiting, Queue *terminated, ProcessControlBlock **running) {
    SchedulerEvent *cur = EventHead;
    SchedulerEvent *prev = NULL;
    while (cur) {
        if (cur->time == current_time) {
            ProcessControlBlock *target = table_lookup(cur->target_pid);
            if (target && !target->forced && target->state != STATE_TERMINATED) {
                target->forced = true;
                target->state = STATE_FORCED;
                target->completion_time = current_time;
                remove_from_queue(ready, target->pid);
                remove_from_queue(waiting, target->pid);
                if (*running && (*running)->pid == target->pid) *running = NULL;
                enqueue(terminated, target);
            }
            SchedulerEvent *del = cur;
            if (!prev) {
                EventHead = cur->next;
                cur = EventHead;
            } else {
                prev->next = cur->next;
                cur = prev->next;
            }
            free(del);
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
}

void handle_waiting_io(Queue *waiting, Queue *ready) {
    QueueNode *cur = waiting->front;
    QueueNode *prev = NULL;
    while (cur) {
        ProcessControlBlock *p = cur->proc;
        QueueNode *next = cur->next;
        if (p->io_enqueue_flag) {
            p->io_enqueue_flag = 0;
        } else if (p->remaining_io > 0) {
            p->remaining_io--;
        }
        if (p->remaining_io == 0) {
            if (!p->forced) {
                p->state = STATE_READY;
                enqueue(ready, p);
            }
            if (!prev) waiting->front = next;
            else prev->next = next;
            if (cur == waiting->rear) waiting->rear = prev;
            free(cur);
            waiting->size--;
            cur = next;
            continue;
        }
        prev = cur;
        cur = next;
    }
}

void schedule(Queue *ready, Queue *waiting, Queue *terminated) {
    int time = 0;
    ProcessControlBlock *running = NULL;
    while (ready->size > 0 || waiting->size > 0 || running != NULL) {
        process_kill_events(time, ready, waiting, terminated, &running);
        if (!running && ready->size > 0) {
            ProcessControlBlock *p = dequeue(ready);
            if (p && !p->forced) running = p;
            else if (p) enqueue(terminated, p);
        }
        if (running) {
            running->executed_cpu++;
            running->ticks_executed++;
            running->remaining_cpu--;
            if (running->executed_cpu == running->io_start && running->io_duration > 0) {
                running->state = STATE_WAITING;
                running->remaining_io = running->io_duration;
                running->current_io = 0;
                running->io_enqueue_flag = 1;
                enqueue(waiting, running);
                running = NULL;
            } else if (running->remaining_cpu <= 0) {
                running->completion_time = time + 1;
                running->state = STATE_TERMINATED;
                enqueue(terminated, running);
                running = NULL;
            }
        }
        handle_waiting_io(waiting, ready);
        time++;
    }
}

void print_report(Queue *terminated) {
    int total = terminated->size;
    if (total == 0) return;
    ProcessControlBlock *arr[total];
    QueueNode *cur = terminated->front;
    int idx = 0;
    while (cur) {
        arr[idx++] = cur->proc;
        cur = cur->next;
    }
    for (int i = 0; i < total - 1; i++)
        for (int j = 0; j < total - i - 1; j++)
            if (arr[j]->pid > arr[j + 1]->pid) {
                ProcessControlBlock *t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
    printf("\n%-5s %-10s %-5s %-5s %-15s %-12s %-8s\n", "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");
    for (int i = 0; i < total; i++) {
        ProcessControlBlock *p = arr[i];
        int cpu = p->total_cpu;
        int io = p->io_duration;
        if (p->state == STATE_FORCED) {
            printf("%-5d %-10s %-5d %-5d KILLED at %-7d %-12s %-8s\n", p->pid, p->name, cpu, io, p->completion_time, "-", "-");
        } else {
            int tat = p->completion_time - p->arrival;
            int wt = tat - cpu;
            printf("%-5d %-10s %-5d %-5d OK%-12s %-12d %-8d\n", p->pid, p->name, cpu, io, "", tat, wt);
        }
    }
}

void free_queue_nodes(Queue *q) {
    QueueNode *cur = q->front;
    while (cur) {
        QueueNode *n = cur->next;
        free(cur);
        cur = n;
    }
    q->front = q->rear = NULL;
    q->size = 0;
}

void cleanup(Queue *ready, Queue *waiting, Queue *terminated) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ProcessControlBlock *cur = ProcessTable[i];
        while (cur) {
            ProcessControlBlock *n = cur->next_in_table;
            free(cur);
            cur = n;
        }
        ProcessTable[i] = NULL;
    }
    SchedulerEvent *ev = EventHead;
    while (ev) {
        SchedulerEvent *n = ev->next;
        free(ev);
        ev = n;
    }
    EventHead = NULL;
    free_queue_nodes(ready);
    free_queue_nodes(waiting);
    free_queue_nodes(terminated);
}

int main() {
    for (int i = 0; i < TABLE_SIZE; i++) ProcessTable[i] = NULL;
    Queue ready, waiting, terminated;
    queue_init(&ready);
    queue_init(&waiting);
    queue_init(&terminated);
    int n, k;
    printf("Enter number of processes: ");
    if (scanf("%d", &n) != 1) return 0;
    getchar();
    read_processes(n, &ready);
    printf("Enter number of kill events: ");
    if (scanf("%d", &k) != 1) return 0;
    getchar();
    if (k > 0) read_kill_events(k);
    schedule(&ready, &waiting, &terminated);
    print_report(&terminated);
    cleanup(&ready, &waiting, &terminated);
    return 0;
}
