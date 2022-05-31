#include "proc.h"



void initializeProcStructures(){
  int i;
  for(i=0; i<8; i++){
    memoryMap[i] = FREE;
  }

  for(i=0; i<8; i++){
    pcbPool[i].name[0] = NULL;
    pcbPool[i].state = DEFUNCT;
    pcbPool[i].segment = 0x0000;
    pcbPool[i].stackPointer = 0x0000;
  }

  idleProc.name[0] = 'I';
  idleProc.name[1] = 'D';
  idleProc.name[2] = 'L';
  idleProc.name[3] = 'E';
  idleProc.name[4] = '\0';
  idleProc.name[5] = '\0';
  idleProc.name[6] = '\0';

  idleProc.state = READY;

  idleProc.segment = 0x0000;
  idleProc.stackPointer = 0x0000;

  running = &idleProc;

  readyHead = NULL;
  readyTail = NULL;

    
}


int getFreeMemorySegment(){
  int i;
  int j = -1;
  for(i=0; i<8; i++){
    if(memoryMap[i] == FREE){
      j=i;
      memoryMap[i] = USED;
      break;
    }
  }
  return j;
}


void releaseMemorySegment(int seg){
  memoryMap[seg] = FREE;
}

struct PCB *getFreePCB(){
  int j=-1;
  int i;
  for(i=0; i<8; i++){
    if(pcbPool[i].state == DEFUNCT){
      j=i;
      break;
    }
  }
  if(j!=-1){
  pcbPool[i].state = STARTING;
  return &pcbPool[i];
  }else{
    return NULL;
  }
}


void releasePCB(struct PCB *pcb){
  pcb -> state = DEFUNCT;
  pcb -> next = NULL;
  pcb -> prev = NULL;
  pcb -> name[0] = 0x00;
}

void addToReady(struct PCB *pcb){
  if(readyTail != NULL){
  pcb->prev = readyTail;
  readyTail-> next = pcb;
  readyTail = pcb;
  }else{
    readyHead = pcb;
    readyTail = pcb;
  }
}

struct PCB *removeFromReady(){
  struct PCB *PCBatHead;
  if(readyHead != NULL){
    if(readyHead -> next!= NULL){
      PCBatHead = readyHead;
      readyHead = PCBatHead -> next;
    }else{
      PCBatHead = readyHead;
      readyHead = NULL;
      readyTail = NULL;
    }
    return PCBatHead;
  }else{
    return NULL;
  }
}


