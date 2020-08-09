// jnl0019
// COMP 3500 LAB1-3, GIVEN FORMAT
// ORIGINAL SUBMISSION JUNE 10, 2019
// UPDATED FOR LAB3, JUNE 20, 2019
// TA: KARTIK AIMA
// INSTRUCTOR: S. BIAZ
// MARKED: “TA” or "INS"

// Global Headers

#include "common.h"

// Global Data Types

typedef enum {
    TAT,
    RT,
    CBT,
    THGT,
    WT
} Metric;

// Global Definitions

#define MAX_QUEUE_SIZE 10 
#define FCFS 1
#define RR 3
#define MAXMETRICS 5

// Global Data Structures

// Global Data

Quantity NumberofJobs[MAXMETRICS]; // Number of Jobs for which metric was collected
Average SumMetrics[MAXMETRICS]; // Sum for each Metrics

// Function Prototypes

void ManageProcesses(void);
void NewJobIn(ProcessControlBlock whichProcess);
void BookKeeping(void);
Flag ManagementInitialization(void);
void LongtermScheduler(void);
void IO();
void CPUScheduler(Identifier whichPolicy);
ProcessControlBlock *SRTF();
void Dispatcher();

int main(int argc, char **argv) {
   if (Initialization(argc,argv)) {
     ManageProcesses();
   }
}

// Function: Monitor Sources and Process Events
void ManageProcesses(void) {
  ManagementInitialization();
  while (1) {
    IO();
    CPUScheduler(PolicyNumber);
    Dispatcher();
  }
}

// Function: (1) Move process on CPU to Waiting if CPUBurst is done, otherwise return to Ready
//           (2) Scans Waiting to find process with complete I/O then move process to Ready
void IO() {
  ProcessControlBlock *currentProcess = DequeueProcess(RUNNINGQUEUE); 
  if (currentProcess) {
    if (currentProcess->RemainingCpuBurstTime <= 0) { // Finished current CPU Burst
      currentProcess->TimeEnterWaiting = Now(); // Record when entered Waiting
      EnqueueProcess(WAITINGQUEUE, currentProcess); // Move to Waiting
        currentProcess->TimeIOBurstDone = Now() + currentProcess->IOBurstTime; // Record when I/O done
      currentProcess->state = WAITING;
    } else { // Must return to Ready
      currentProcess->JobStartTime = Now();
      EnqueueProcess(READYQUEUE, currentProcess); // Move back to Ready
      currentProcess->state = READY; // Update PCB state 
    }
  }

  // Scan Waiting Queue to find processes with complete I/O
  ProcessControlBlock *ProcessToMove;
  // Scan Waiting List to find processes that got complete IOs
  ProcessToMove = DequeueProcess(WAITINGQUEUE);
  if (ProcessToMove) {
    Identifier IDFirstProcess =ProcessToMove->ProcessID;
    EnqueueProcess(WAITINGQUEUE,ProcessToMove);
    ProcessToMove = DequeueProcess(WAITINGQUEUE);
    while (ProcessToMove) {
      if (Now()>=ProcessToMove->TimeIOBurstDone) {
	ProcessToMove->RemainingCpuBurstTime = ProcessToMove->CpuBurstTime;
	ProcessToMove->JobStartTime = Now(); // TA
	EnqueueProcess(READYQUEUE,ProcessToMove);
      } else {
	EnqueueProcess(WAITINGQUEUE,ProcessToMove);
      }
      if (ProcessToMove->ProcessID == IDFirstProcess) {
	break;
      }
      ProcessToMove = DequeueProcess(WAITINGQUEUE);
    } // while (ProcessToMove) INS
  } // if (ProcessToMove) INS
}

// Input: whichPolicy (1:FCFS, 2: SRTF, and 3:RR)
// Output: (none)
// Function: (selects process from Ready and puts on Running)
void CPUScheduler(Identifier whichPolicy) {
  ProcessControlBlock *selectedProcess;
  if ((whichPolicy == FCFS) || (whichPolicy == RR)) {
    selectedProcess = DequeueProcess(READYQUEUE);
  } else { // Shortest Remaining Time First
    selectedProcess = SRTF();
  }
  if (selectedProcess) {
    selectedProcess->state = RUNNING; // Process state becomes Running
    EnqueueProcess(RUNNINGQUEUE, selectedProcess); // Put process in Running Queue
  }
}

// Input: (none)
// Output: (pointer to process with shortest remaining time)
// Function: (returns PCB with SRTF)
ProcessControlBlock *SRTF() {
  // selects process with SRTF
  ProcessControlBlock *selectedProcess, *currentProcess = DequeueProcess(READYQUEUE);
  selectedProcess = (ProcessControlBlock *) NULL;
  if (currentProcess) {
    TimePeriod shortestRemainingTime = currentProcess->TotalJobDuration - currentProcess->TimeInCpu;
    Identifier IDFirstProcess = currentProcess->ProcessID;
    EnqueueProcess(READYQUEUE,currentProcess);
    currentProcess = DequeueProcess(READYQUEUE);
    while (currentProcess) {
      if (shortestRemainingTime >= (currentProcess->TotalJobDuration - currentProcess->TimeInCpu)) {
	EnqueueProcess(READYQUEUE,selectedProcess);
	selectedProcess = currentProcess;
	shortestRemainingTime = currentProcess->TotalJobDuration - currentProcess->TimeInCpu;
      } else {
	EnqueueProcess(READYQUEUE,currentProcess);
      }
      if (currentProcess->ProcessID == IDFirstProcess) {
	break;
      }
      currentProcess = DequeueProcess(READYQUEUE);
    } // while (ProcessToMove)
  } // if (currentProcess)
  return (selectedProcess);
}

// Input: (none)
// Output: (none)
// Function: (If Running process requires computation, place on CPU.
//           Else, move process from Running to Exit)
void Dispatcher() {
  double start;
  ProcessControlBlock *processOnCPU = Queues[RUNNINGQUEUE].Tail; // Pick Process on CPU
    
  if (!processOnCPU) { // No Process in Running Queue, i.e., on CPU
    return;
  }
    
  if (processOnCPU->TimeInCpu == 0.0) { // First time this process gets the CPU
    SumMetrics[RT] += Now()- processOnCPU->JobArrivalTime;
    NumberofJobs[RT]++;
    processOnCPU->StartCpuTime = Now(); // Set StartCpuTime
  }
  
  if (processOnCPU->TimeInCpu >= processOnCPU-> TotalJobDuration) { // Process Complete
    printf(" >>>>>Process # %d complete, %d Processes Completed So Far <<<<<\n",
    processOnCPU->ProcessID,NumberofJobs[THGT]);
    processOnCPU=DequeueProcess(RUNNINGQUEUE);
    EnqueueProcess(EXITQUEUE,processOnCPU);

    NumberofJobs[THGT]++;
    NumberofJobs[TAT]++;
    NumberofJobs[WT]++;
    NumberofJobs[CBT]++;
    SumMetrics[TAT] += Now() - processOnCPU->JobArrivalTime;
    SumMetrics[WT] += processOnCPU->TimeInReadyQueue;
      
  } else { // Process still needs computing, out it on CPU
    TimePeriod CpuBurstTime = processOnCPU->CpuBurstTime;
    processOnCPU->TimeInReadyQueue += Now() - processOnCPU->JobStartTime;
    if (PolicyNumber == RR) {
      CpuBurstTime = Quantum;
        if (processOnCPU->RemainingCpuBurstTime < Quantum) {
          CpuBurstTime = processOnCPU->RemainingCpuBurstTime;
        }
    }
    processOnCPU->RemainingCpuBurstTime -= CpuBurstTime;
    // INS
    TimePeriod StartExecution = Now();
    OnCPU(processOnCPU, CpuBurstTime); // INS uses CpuBurstTime instead of PCB->CpuBurstTime
    processOnCPU->TimeInCpu += CpuBurstTime; // INS uses CpuBurstTime instead of PCB->CpuBurstTimeu
    SumMetrics[CBT] += CpuBurstTime;
  }
}

// Input: (none)
// Output: (none)
// Function: (ran when job is added to Job Queue)
void NewJobIn(ProcessControlBlock whichProcess) {
  ProcessControlBlock *NewProcess;
  // adds job to Job Queue
  NewProcess = (ProcessControlBlock *) malloc(sizeof(ProcessControlBlock));
  memcpy(NewProcess,&whichProcess,sizeof(whichProcess));
  NewProcess->TimeInCpu = 0;
  NewProcess->RemainingCpuBurstTime = NewProcess->CpuBurstTime; // INS RR correction
  EnqueueProcess(JOBQUEUE,NewProcess);
  DisplayQueue("Job Queue in NewJobIn",JOBQUEUE);
  LongtermScheduler(); // admit job
}

// Input: (none)
// Output: (none)
// Function: (1) Call bookkeeping after process 250 arrives
//           (2) Compute and display metrics
void BookKeeping(void) {
  double end = Now(); // Total time for all processes to arrive
  Metric m;

  // computes averages and final results
  if (NumberofJobs[TAT] > 0) {
    SumMetrics[TAT] = SumMetrics[TAT]/ (Average) NumberofJobs[TAT];
  }
    
  if (NumberofJobs[RT] > 0) {
    SumMetrics[RT] = SumMetrics[RT]/ (Average) NumberofJobs[RT];
  }
    
  SumMetrics[CBT] = SumMetrics[CBT]/ Now();

  if (NumberofJobs[WT] > 0) {
    SumMetrics[WT] = SumMetrics[WT]/ (Average) NumberofJobs[WT];
  }

  printf("\n********* Processes Managemenent Numbers *********\n");
  printf("Policy Number = %d, Quantum = %.6f   Show = %d\n", PolicyNumber, Quantum, Show);
  printf("Number of Completed Processes = %d\n", NumberofJobs[THGT]);
  printf("ATAT=%f   ART=%f  CBT = %f  T=%f AWT=%f\n", 
	 SumMetrics[TAT], SumMetrics[RT], SumMetrics[CBT], 
	 NumberofJobs[THGT]/Now(), SumMetrics[WT]);
    
  exit(0);
}

// Input: (none)
// Output: (none)
// Function: (decides which process to admit to Ready, then moves it from Job)
void LongtermScheduler(void) {
  ProcessControlBlock *currentProcess = DequeueProcess(JOBQUEUE);
  while (currentProcess) {
    currentProcess->TimeInJobQueue = Now() - currentProcess->JobArrivalTime; // Set TimeInJobQueue
    currentProcess->JobStartTime = Now(); // Set JobStartTime
    EnqueueProcess(READYQUEUE,currentProcess); // Place process in Ready Queue
    currentProcess->state = READY; // Update process state
    currentProcess = DequeueProcess(JOBQUEUE);
  }
}

// Input: (none)
// Output: TRUE (if initialization is sucessful)
Flag ManagementInitialization(void) {
  Metric m;
  for (m = TAT; m < MAXMETRICS; m++) {
     NumberofJobs[m] = 0;
     SumMetrics[m] = 0.0;
  }
  return TRUE;
}
