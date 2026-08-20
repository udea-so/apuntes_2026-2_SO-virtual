// Step 1: Include Necessary Libraries
#include <stdio.h>
#include <stdlib.h>

// Step 2: Define the Process and Queue Structures
typedef struct {
    int id;
    int duration;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
} Process;

typedef struct {
    Process *processes[100];
    int front, rear;
    int time_quantum;
} Queue;


// Step 3: Initialize the Queue
void initializeQueue(Queue *q, int time_quantum) {
    q->front = q->rear = -1;
    q->time_quantum = time_quantum;
}

// Step 4: Implement Queue Operations
void enqueue(Queue *q, Process *p) {
    if(q->rear == 99) {
        printf("Queue is full!\n");
        return;
    }
    q->processes[++q->rear] = p;
    if(q->front == -1) {
        q->front = 0;
    }
    printf("Process %d enqueued in queue with time quantum %d\n", p->id, q->time_quantum);
}

Process* dequeue(Queue *q) {
    if(q->front == -1) {
        return NULL;
    }
    Process *p = q->processes[q->front];
    if(q->front == q->rear) {
        q->front = q->rear = -1;
    } else {
        q->front++;
    }
    printf("Process %d dequeued from queue with time quantum %d\n", p->id, q->time_quantum);
    return p;
}

// Step 5: Implement the MLFQ Scheduling Logic
void mlfq_scheduling(Queue *high_priority_q, Queue *medium_priority_q, Queue *low_priority_q, int n) {
    int total_time = 0;
    while(1) {
        Process *p = dequeue(high_priority_q);
        if(p != NULL) {
            printf("Process %d is running in high priority queue\n", p->id);
            if(p->remaining_time <= high_priority_q->time_quantum) {
                total_time += p->remaining_time;
                p->remaining_time = 0;
                p->waiting_time = total_time - p->duration;
                p->turnaround_time = total_time;
                printf("Process %d finished execution\n", p->id);
            } else {
                p->remaining_time -= high_priority_q->time_quantum;
                total_time += high_priority_q->time_quantum;
                enqueue(medium_priority_q, p);
            }
        } else {
            p = dequeue(medium_priority_q);
            if(p != NULL) {
                printf("Process %d is running in medium priority queue\n", p->id);
                if(p->remaining_time <= medium_priority_q->time_quantum) {
                    total_time += p->remaining_time;
                    p->remaining_time = 0;
                    p->waiting_time = total_time - p->duration;
                    p->turnaround_time = total_time;
                    printf("Process %d finished execution\n", p->id);
                } else {
                    p->remaining_time -= medium_priority_q->time_quantum;
                    total_time += medium_priority_q->time_quantum;
                    enqueue(low_priority_q, p);
                }
            } else {
                p = dequeue(low_priority_q);
                if(p != NULL) {
                    printf("Process %d is running in low priority queue\n", p->id);
                    if(p->remaining_time <= low_priority_q->time_quantum) {
                        total_time += p->remaining_time;
                        p->remaining_time = 0;
                        p->waiting_time = total_time - p->duration;
                        p->turnaround_time = total_time;
                        printf("Process %d finished execution\n", p->id);
                    } else {
                        p->remaining_time -= low_priority_q->time_quantum;
                        total_time += low_priority_q->time_quantum;
                        enqueue(low_priority_q, p);
                    }
                } else {
                    break;
                }
            }
        }
    }
}

// Step 6: Implement the main() Function
int main() {
    // Step 6.1: Initialize the Queues
    Queue high_priority_q, medium_priority_q, low_priority_q;
    initializeQueue(&high_priority_q, 2);  // High priority queue with time quantum of 2
    initializeQueue(&medium_priority_q, 4); // Medium priority queue with time quantum of 4
    initializeQueue(&low_priority_q, 8);    // Low priority queue with time quantum of 8

    // Step 6.2: Take User Input for Number of Processes
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Step 6.3: Allocate Memory for Processes and Take Their Input
    Process *processes = (Process *)malloc(n * sizeof(Process));
    for(int i = 0; i < n; i++) {
        printf("Enter duration for process %d: ", i+1);
        scanf("%d", &processes[i].duration);
        processes[i].id = i+1;
        processes[i].remaining_time = processes[i].duration;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        enqueue(&high_priority_q, &processes[i]);  // Enqueue all processes to high priority queue initially
    }

    // Step 6.4: Invoke the MLFQ Scheduling Function
    mlfq_scheduling(&high_priority_q, &medium_priority_q, &low_priority_q, n);

    // Step 6.5: Display the Results
    printf("Process\tDuration\tWaiting Time\tTurnaround Time\n");
    for(int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].duration, processes[i].waiting_time, processes[i].turnaround_time);
    }

    // Step 6.6: Free the Allocated Memory
    free(processes);
    return 0;
}
