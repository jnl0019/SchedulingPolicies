// jnl0019
// COMP 3500 LAB1-3, GIVEN FORMAT
// ORIGINAL SUBMISSION JUNE 10, 2019
// UPDATED FOR LAB3, JUNE 20, 2019
// TA: KARTIK AIMA
// INSTRUCTOR: S. BIAZ
// MARKED: “TA” or "INS"

// Global System Headers

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// Global Data Types

typedef int Identifier;
typedef int Priority;
typedef short Flag;
typedef unsigned Status;

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    DONE
} State;

typedef enum {
    JOBQUEUE,
    READYQUEUE,
    RUNNINGQUEUE,
    WAITINGQUEUE,
    EXITQUEUE
} Queue;

typedef double Timestamp;
typedef double TimePeriod;
typedef int Quantity;
typedef double Average;
typedef unsigned Memory;

// Global Definitions

// Constants for Process Management

#define MAXQUEUES 5         // Max queues (Job, Ready, Run...)
#define MAX_PRIORITIES 4    // Priority Levels
#define FALSE 0             // False condition
#define TRUE 1              // True condition

#define RATIO_TOTAL_CPUBURST 4.0      // Ratio of TotalJobDuration to CpuBurstTime
#define RATIO_IOBURST_CPUBURST 0.25   // Ratio of IOBurst to CPU Burst
#define CONTEXTSWITCHOVERHEAD 0.0001  // For calculating Overhead due to Context Switch

// Constants for Memory Management

#define MAXMEMORYSIZE 0x100000 // 1 MB
#define MAXPROCMEMORY 0x10000 // 64KB Max Process Size (32KB average)
#define MINPROCMEMORY 0x100 // 256 Bytes Min Process Size


// Global Data Structures

typedef struct ProcessControlBlockTag {
  Identifier ProcessID;
  State state;
  Priority priority;
  Timestamp JobArrivalTime;                // Time when job first entered job queue
  TimePeriod TotalJobDuration;             // Total CPU time job requires
  TimePeriod TimeInCpu;                    // Total time process spent thus far on CPU
  TimePeriod CpuBurstTime;                 // Length of typical CPU burst for job
  TimePeriod RemainingCpuBurstTime;        // Remaing time of current CPU burst
  TimePeriod IOBurstTime;                  // Length of typical I/O burst for job
  TimePeriod TimeIOBurstDone;              // Time when current I/O will be done
  Timestamp JobStartTime;                  // Time when job first entered ready queue
  Timestamp StartCpuTime;                  // Time when job was first placed on CPU
  Timestamp TimeEnterWaiting;              // Last time Job Entered the Waiting Queue
  Timestamp JobExitTime;                   // Time when job first entered exit queue
  TimePeriod TimeInReadyQueue;             // Total time process spent in ready queue
  TimePeriod TimeInWaitQueue;              // Total time process spent in wait queue
  TimePeriod TimeInJobQueue;               // Total time process spent in job queue
  Memory TopOfMemory;                      // Address of top of allocated memory block
  Memory MemoryAllocated;                  // Amount of Allocated memory in bytes
  Memory MemoryRequested;                  // Amount of requested memory in bytes
  struct ProcessControlBlockTag *previous; // Previous element in linked list
  struct ProcessControlBlockTag *next;     // Next element in linked list
} ProcessControlBlock;

typedef struct QueueParmsTag {
  ProcessControlBlock *Head;
  ProcessControlBlock *Tail;
} QueueParms;

// Global Data

extern Flag Show;
extern QueueParms Queues[MAXQUEUES];
extern Identifier PolicyNumber; // (1:FCFS) , (2:SRTF), (3:RR)
extern TimePeriod Quantum;
extern Memory AvailableMemory; // Total Available Memory

// Function Prototypes

// Input: none
// Output: (returns current system time)
extern Timestamp Now(void);

// Input: (destination queue and element to add)
// Output: (head and tail updated)
// Function: (enqueues FIFO element and updates tail/head)
extern void EnqueueProcess(Queue whichQueue, ProcessControlBlock *whichProcess);

// Input: (queu from which to dequeue)
// Output: (tail of designated queue)
// Function: (removes tail element and updates tail/head)
extern ProcessControlBlock *DequeueProcess(Queue whichQueue);

// CPU executes whichProcess for CPUBurst
extern void OnCPU(ProcessControlBlock *whichProcess, TimePeriod CPUBurst);
extern void NewJobIn(ProcessControlBlock whichProcess);
extern void BookKeeping(void);
extern Flag Initialization(int argc, char **argv);
extern void DisplayProcess(char c,ProcessControlBlock *whichProcess);
extern void DisplayQueue(char *str, Queue whichQueue);
