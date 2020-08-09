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
    FREEHOLES,
    PARKING
} MemoryQueue;

// Global Definitions

// Global Data Structures

typedef struct FreeMemoryHoleTag {
  Memory AddressFirstElement;           // address of first element
  Memory Size;                          // size of the hole
  struct FreeMemoryHoleTag *previous;   // previous element in linked list
  struct FreeMemoryHoleTag *next;       // next element in linked list
} FreeMemoryHole;

typedef struct MemoryQueueParmsTag {
  FreeMemoryHole *Head;
  FreeMemoryHole *Tail;
  Quantity NumberOfHoles;
} MemoryQueueParms;

// Global Data

MemoryQueueParms MemoryQueues[2]; // free holes and parking

// Function Prototypes

void EnqueueMemoryHole(MemoryQueue whichQueue, FreeMemoryHole *whichProcess);
FreeMemoryHole *DequeueMemoryHole(MemoryQueue whichQueue);

int main(int argc, char **argv) {
    
  FreeMemoryHole *NewMemoryHole;
    
  int i;
  // initializes the queues
  for (i = 0; i < 2; i++) {
    MemoryQueues[i].Tail = (FreeMemoryHole *) NULL;
    MemoryQueues[i].Head = (FreeMemoryHole *) NULL;
    MemoryQueues[i].NumberOfHoles = 0;
  }
 
  // creates initial large memory holes containing the full memory
  NewMemoryHole = (FreeMemoryHole *) malloc(sizeof(FreeMemoryHole));
  if (NewMemoryHole) { //malloc successful
    NewMemoryHole->AddressFirstElement = 0;
    NewMemoryHole->Size = MAXMEMORYSIZE;
    
    // INS
    EnqueueMemoryHole(PARKING,NewMemoryHole);

    // move what was in the parking into the queue of free holes
    FreeMemoryHole *aFreeMemoryHole;
    aFreeMemoryHole = DequeueMemoryHole(PARKING);
    EnqueueMemoryHole(FREEHOLES,aFreeMemoryHole);

    // testing:
    printf("Number of holes in Parking = %d\n",MemoryQueues[PARKING].NumberOfHoles);
      
    if (MemoryQueues[PARKING].NumberOfHoles) {
      printf("Parking should be empty");
      printf("Number of holes in Free Holes = %d\n",MemoryQueues[FREEHOLES].NumberOfHoles);
      }
      
    if (MemoryQueues[FREEHOLES].NumberOfHoles) {
      printf("Starting Address %d\n",MemoryQueues[FREEHOLES].Tail->AddressFirstElement);
      printf("Size in hexadecimal 0x%x\n",MemoryQueues[FREEHOLES].Tail->Size);
    }

  }
}

// Input: (queue where to enqueue and element to enqueue)
// Output: (updates head/tail accordingly)
// Function: (enqueues FIFO element in queue and updates tail/head
void EnqueueMemoryHole(MemoryQueue whichQueue, FreeMemoryHole *whichMemoryHole) {
  if (whichMemoryHole == (FreeMemoryHole *) NULL) {
    return;
  }

  MemoryQueues[whichQueue].NumberOfHoles++;

 // Enqueues the Process
    if (MemoryQueues[whichQueue].Head) {
    MemoryQueues[whichQueue].Head->previous = whichMemoryHole;
    whichMemoryHole->next = MemoryQueues[whichQueue].Head;
    whichMemoryHole->previous = NULL;
    MemoryQueues[whichQueue].Head = whichMemoryHole;
    }
    
    if (MemoryQueues[whichQueue].Tail == NULL) {
    MemoryQueues[whichQueue].Tail = whichMemoryHole;
    }
}

// Input: (destination queue and element to enqueue)
// Output: (returns tail)
// Function: (removes tail element and updates tail/head)
FreeMemoryHole *DequeueMemoryHole(MemoryQueue whichQueue) {
  FreeMemoryHole *HoleToRemove;
  HoleToRemove = MemoryQueues[whichQueue].Tail;
  if (HoleToRemove != (FreeMemoryHole *) NULL) {
    MemoryQueues[whichQueue].NumberOfHoles--;
    HoleToRemove->next = (FreeMemoryHole *) NULL;
    MemoryQueues[whichQueue].Tail = MemoryQueues[whichQueue].Tail->previous;
    HoleToRemove->previous =(FreeMemoryHole *) NULL;
    if (MemoryQueues[whichQueue].Tail == (FreeMemoryHole *) NULL) {
      MemoryQueues[whichQueue].Head = (FreeMemoryHole *) NULL;
    } else {
      MemoryQueues[whichQueue].Tail->next = (FreeMemoryHole *) NULL;
    }
  }
  return(HoleToRemove);
}
