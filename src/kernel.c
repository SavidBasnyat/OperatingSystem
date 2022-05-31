
/*@author: Savid
  @author: katya
*/


#define MAIN

#include "proc.h"

void putChar(char ch, int row, int col, char color);
void putStr(char str[], int row, int col, char color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int handleInterrupt21(int ax, int bx, inct cx, int dx);
int readfile(char *filename, char *buf);
int executeProgram(char *name);
void terminate();
int writeSector(char *buffer, int sector);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
void kStrCopy(char *src, char *dest, int len);
void yield();
void showProcesses();
void printHelp(int i);
int kill(int segment);



typedef char byte;

struct dirEntry{
  char name[6];
  byte sectors[26];
};
struct directory{
  struct dirEntry entries[16];
};

void main(){
 
  char buffer[13312];

  initializeProcStructures();

  makeInterrupt21();

  executeProgram("shell2\0");
  
  makeTimerInterrupt();

  while(1){}
}


/* Function to print a character acoording to the given column, row and color */
void putChar(char ch, int row, int col, char color){
  int offset;
  int segment;
  offset = 0x8000 + 80*(row-1)*2 + (col-1)*2;
  segment = 0xB000;
  putInMemory(segment, offset, ch);
  putInMemory(segment , offset+1, color);
}


/* Function to print a string acoording to the given column, row and color */
void putStr(char str[], int row, int col, char color){
  int i=0;
  while(str[i] != '\0'){
    putChar(str[i], row, col, color);
    col=col+1;
    i++;
  }
}


/* Function to print a string */
int printString(char *str){
  char al;
  char ah;
  int i = 0;
  int ax;

  while(str[i] != '\0'){
    al = str[i];
    ah = 0x0E;
    ax = ah * 256 + al;
    interrupt(0x10, ax, 0, 0, 0);
    i++;
  }
  return i;
}


/* Function to read a character */
int readChar(){
  return interrupt(0x16, 0, 0, 0, 0);
}


/* Function to read a character to a buffer */
int readString(char *buf){
  char ch;
  char null = 0x0D;
  int i = 0;
  char charRead[2];
  
  charRead[1] = 0x00;
  ch = 0x00;
  ch = readChar();
  
  while(ch!=null){
    if(ch == 0x08){ //backspace key is handled correctly.
      if(i>0){
	interrupt(0x10, 0x0E*256+0x08, 0, 0, 0);
	i--;
	buf[i]=' ';
	interrupt(0x10, 0x0E*256+buf[i], 0, 0, 0);
	interrupt(0x10, 0x0E*256+0x08, 0, 0, 0);
      }
    }else{
      buf[i] = ch;
      charRead[0] = ch;
      printString(charRead);
      i++;
    }
    ch = readChar();
  }
  buf[i] = 0x00;
  return i;
}


/* Function to calculate the remainder */
int mod(int number, int divisor){
  while(number >= divisor){
    number = number - divisor;
  }
  return number;
}


/* Function to read from a sector from the disk */
int readSector(char *buf, int absSector){
  char ah;
  char al;
  int ax;
  int bx;
  char ch;
  char cl;
  int cx;
  char dh;
  char dl;
  int dx;

  ah = 0x02;
  al = 0x01;
  ax = ah * 256 + al;
  bx = buf;
  ch = absSector/36;
  cl = mod(absSector, 18) +1;
  cx = ch * 256 + cl;
  dh = mod((absSector/18), 2);
  dl = 0x00;
  dx = dh *256 + dl;

  interrupt(0x13, ax, bx, cx, dx);
  return 1;
}


/* Function to write to a sector in the disk from a buffer */
int writeSector(char *buffer, int sector){
  char ah;
  char al;
  int ax;
  int bx;
  char ch;
  char cl;
  int cx;
  char dh;
  char dl;
  int dx;

  ah = 0x03;
  al = 0x01;
  ax = ah * 256 + al;
  bx = buffer;
  ch = sector/36;
  cl = mod(sector, 18) +1;
  cx = ch * 256 + cl;
  dh = mod((sector/18), 2);
  dl = 0x00;
  dx = dh *256 + dl;

  interrupt(0x13, ax, bx, cx, dx);
  return 1;
}


/* Function to handle interrupt 21 */
int handleInterrupt21(int ax, int bx, int cx, int dx){
  char *b;
  if(ax == 0x00) {
    b = bx;
    return printString(b);
  }
  else if(ax == 0x11) {
    b = bx;
    b[0] = readChar();
    return 1;
  }
  else if(ax == 0x01){
    return readString(bx);
  }
  else if(ax ==0x02){
    return readSector(bx, cx);
  }
  else if(ax ==0x03){
    return readfile(bx, cx);
  }
  else if(ax == 0x04){
    return executeProgram(bx);
  }
  else if(ax == 0x05){
    return terminate();
  }
  else if(ax == 0x07){
    return deleteFile(bx);
  }
  else if(ax == 0x08){
    b = bx;
    return writeFile(b, cx, dx);
  }else if(ax == 0x09){
    return yield();
  }else if(ax == 0x0A){
    return showProcesses();
  }else if(ax == 0x0B){
    return kill(bx);
  }else{
    return -1;
  }
}


/* Find length of file name */
int findLength(char* filename){
  int count = 0;
  int i = 0;
  for(i = 0; filename[i] != '\0'; i++){
    count++;
  }
  return count;
}



/* Function toRead contents of a file into a buffer. 
   The filename parameter will be  the name of the file to be read and the buf parameter is where the contents of the file are to be placed. */

int readfile(char *filename, char *buf){
  int i;
  int j;
  int k;
  int found;
  int length;
  int sectorPtr;

  struct directory diskDir;
 
  readSector(&diskDir, 2);

  length = findLength(filename);

  for(i=0; i<16; i++){
    found =1;
    for(j = 0; j < 6; j++){
      if(diskDir.entries[i].name[j] != filename[j]){
	found = 0;
	break;
      }
    }
    if(j==length){
      k = i;
      break;
    }
  }
  if(found == 0){
    return -1;
  }else{
    i = 0;
    sectorPtr = 0;
    while(diskDir.entries[k].sectors[sectorPtr] != 0  && i< 26){
      readSector(buf, diskDir.entries[k].sectors[sectorPtr]);
      buf += 512;
      sectorPtr++;
      i++;
    }
  }
  return sectorPtr;
}


/* Function to delete a file */
int deleteFile(char *fname){
  int i;
  int j;
  int entry;
  int found;
  int length;
  int sectorPtr;
  struct directory diskDir;
  char diskMap[512];

  readSector(&diskDir, 2);
  readSector(diskMap, 1);

  length = findLength(fname);

  for(i=0; i<16; i++){
    found =1;
    for(j = 0; j < 6; j++){
      if(diskDir.entries[i].name[j] != fname[j]){
	found = 0;
	break;
      }
    }
    if(j==6){
      entry = i;
      break;
    }
  }
  
  if(found == 0){
    return -1;
  }else{
    diskDir.entries[entry].name[0] = 0x00;
    i=0;
    sectorPtr = 0;
    while(diskDir.entries[entry].sectors[sectorPtr] !=0 && i<26){
      diskMap[diskDir.entries[entry].sectors[sectorPtr]] = 0x00;
      sectorPtr++;
      i++;
    }
    writeSector(diskMap,1);
    writeSector(&diskDir, 2);

    return 1;
  }
 
}


/* Function to write a file, overrides if the the file already exists */
int writeFile(char *fname, char *buffer, int sectors) {
  char diskMap[512];
  struct directory diskDir;
  int i;
  int j;
  int y;
  int entry = -1;
  int sec;
  
  deleteFile(fname);
  
  if (sectors > 26) {
    sectors = 26;
    sec = sectors;
  }

	
  readSector(&diskMap, 1);
  readSector(&diskDir, 2);

  for (i=0; i<16; i++){
    if (!diskDir.entries[i].name[0]) {
      entry = i;
      break;		
    }			
  }
	
  if (entry == -1) {
    printString("Disk directory Full");
    return -1;
  }

  for (i=0; i<6; i++) {
    diskDir.entries[entry].name[i] = 0x00;
  }

  i = 0;
  while(i<6 && fname[i] != '\0'){
    diskDir.entries[entry].name[i] = fname[i];
    i++;
  }
	
	
  for(i=0;i<26;i++){
    diskDir.entries[entry].sectors[i] = 0x00;
  }

	
  for(i=0;i<sectors;i++){
    y = -1;
    for(j=0;j<512;j++){
      if(diskMap[j] == 0x00){
	y = j;
	break;
      }
    }
	  
    if (y == -1){
      printString("Disk map full.");
      return -2;
    }
    diskMap[y] = 0xFF;
    diskDir.entries[entry].sectors[i] = y;
    writeSector(buffer, y);
    buffer = buffer + 512;
  }	

  writeSector(diskMap, 1);
  writeSector(&diskDir, 2);

  return sec;
}



/* Function to execute user programs */
int executeProgram(char* name) {
  char buffer[13312];
  int i = 0;
  int j;
  int offset= 0x0000;
  int segment;
  int res,seg;
  struct PCB *tempPCB;

  res = readfile(name, buffer);

  if(res == -1) {
    return -1; //FNF
  }
  setKernelDataSegment();
  seg = getFreeMemorySegment();
  restoreDataSegment();
  if(seg == -1){
    return -2; //NFS
  }
 
  segment = (2 + seg ) * 0x1000; //corrected

  while(i < 13312) { //corrected
    putInMemory(segment,offset,buffer[i]);
    i++;
    offset++;
  }
  
  setKernelDataSegment();
  tempPCB = getFreePCB();

  if(tempPCB == NULL){
    printString("No free PCB\n");
  }

  tempPCB->state = READY; //added
  tempPCB ->segment = segment;
  tempPCB ->stackPointer = 0xFF00;

  addToReady(tempPCB);
  restoreDataSegment();
  kStrCopy(name, tempPCB->name, 7);

  initializeProgram(segment);
  
  //launchProgram(segment);
  return 1;
}


/* Function to terminate a user program*/
void terminate(){
  

  setKernelDataSegment();
  
  releaseMemorySegment((running->segment/0x1000)-2);
  releasePCB(running);
  running->state = DEFUNCT;
  running=NULL;
  restoreDataSegment();

  while(1){}

  
  
}

//Function invoked each time a timer interrupt occurs.
void handleTimerInterrupt(int segment, int stackPointer) {
  
  struct PCB *tempPCB;
  int seg;
  int stPointer;

  setKernelDataSegment();

  
  running -> stackPointer = stackPointer;
  running -> segment = segment;
  running -> state = READY;
  
      
  if (running != &idleProc) {
    addToReady(running);
  }

  running=NULL;

  tempPCB = removeFromReady();
    
  if(tempPCB == NULL){
    tempPCB=&idleProc;
  }
  

	
  seg=tempPCB->segment;
  stPointer=tempPCB->stackPointer;
  running=tempPCB;

  restoreDataSegment();
  
  returnFromTimer(seg, stPointer);
}

/* kStrCopy(char *src, char *dest, int len) copy at most len
 * characters from src which is addressed relative to the current * data segment into dest which is addressed relative to the
 * kernel's data segment (0x1000).
 */
void kStrCopy(char *src, char *dest, int len) {
  int i=0;
  for (i=0; i<len; i++) {
    putInMemory(0x1000, dest+i, src[i]);
    if (src[i] == 0x00) {
      return; }
  }
}

//Function causes the executing process to give up the remainder of its time slice and be put back into the ready queue
void yield(){
  interrupt(0x08, 0, 0, 0);
  
}

//Function displays a list of the names and memory segment indices of all of the currently executing processes.
void showProcesses(){
  int i;
  struct PCB *tempPCB;

  setKernelDataSegment();

  for(i=0; i<8; i++){
    if(memoryMap[i] == USED){
      tempPCB = &pcbPool[i];
      printString("Process: ");
      printString(tempPCB->name);
      printString(" at index: ");
      printHelp(i);	
      printString("\r\n\0");
      
    }
  }
  restoreDataSegment();
}

//Helper method for showProcesses
void printHelp(int i){
  if(i==0){
    printString("Zero");
    
  }
  if(i==1){
    printString("One");
    
  }
  if(i==2){
    printString("Two");
    
  }
  if(i==3){
    printString("Three");
    
  }
  if(i==4){
    printString("Four");
    
  }
  if(i==5){
    printString("Five");
    
  }
  if(i==6){
    printString("Six");
    
  }
  if(i==7){
    printString("Seven");
    
  }
}


//Function to kill the process that is executing in the segment with the specified index.
int kill(int segment){

  if(memoryMap[segment] == USED){
    setKernelDataSegment();
    running = NULL;
    releaseMemorySegment(segment);
    releasePCB(segment);
    restoreDataSegment();
    return 1;
  }else{
    return -1;
  }
}

