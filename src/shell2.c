#include "userlib.h"


int killHelp();

typedef char byte;

struct dirEntry{
  char name[6];
  byte sectors[26];
};
struct directory{
  struct dirEntry entries[16];
};

main() {
  char input[512];
  char buffer[1000];
  int i;
  int j;
  int k;
  int result;
  char fileToCopy[100];
  char fileToCopyTo[100];
  char fileInDir[130];
  char diskMap[512];
  struct directory diskDir;

  enableInterrupts();
 

  //while (1) {
  /* Print 'Shell' prompt */
  //interrupt(0x21,0x00, "Shell>\0", 0, 0);
  printString("Shell>\0");
  
  //interrupt(0x21,0x01,input,0,0);
  readString(input);
  
  //interrupt(0x21, 0x00, "\n\r\0", 0, 0);
  printString("\n\r\0");
  
  /*Type Command */
  
  if(input[0] == 't' && input[1] == 'y' && input[2] == 'p' && input[3] == 'e' && input[4] == ' '){
   
    //result = interrupt(0x21, 0x03, input+5, buffer, 0);
    result = readfile(input+5, buffer);
   
    if(result == -1){
      //interrupt(0x21,0x00, "Error File not Found", 0, 0);
      printString("Error File not Found");
	
    }else{
      //interrupt(0x21,0x00, buffer, 0, 0);
      printString(buffer);
    }
  }

  /*Execute Command */
  else if(input[0] == 'e' && input[1] == 'x'&& input[2] == 'e' && input[3] == 'c' && input[4] == 'u' && input[5] == 't' && input[6] == 'e'){
    
    //result = interrupt(0x21, 0x04, input+8, 0x2000, 0);
    result = executeProgram(input+8, 0x2000);
    
    if(result == -1){
      //interrupt(0x21, 0x00, "Error File not Found", 0, 0);
      printString("Error File not Found");
    }
	  
  }
  /*Delete Command */
  else if(input[0] == 'd' && input[1] == 'e' && input[2] == 'l' && input[3] == 'e' && input[4] == 't' && input[5] == 'e'){
    
    result = deleteFile(input+7);
    
    if(result == -1){
      printString("Error File not Found");
    }
 
  }

  else if(input[0] == 'c' && input[1] == 'o' && input[2] == 'p' && input[3] == 'y'){
    j=0;
    for(i=5; i<11; i++){
      fileToCopy[j] = input[i];
      j++;
    }
    j=0;
    for(i=12; i<18; i++){
      fileToCopyTo[j] = input[i];
      j++;
    }

    result = readfile(fileToCopy, buffer);
    
    if(result == -1){
      printString("Error File not Found");
    }else{
      result = writeFile(fileToCopyTo, buffer, result);
      if(result == -1){
	printString("Disk directory is full.");
      }else if(result == -2){
	printString("Disk is full.");
      }
     
    }

  }

  /*dir Command */
  else if(input[0] == 'd' && input[1] == 'i' && input[2] == 'r'){
    readSector(&diskDir,2);
    k=0;
    j=0;
    for(i=0; i<16; i++){
      if(diskDir.entries[i].name[0] != 0x00){
	while(diskDir.entries[i].name[j] != 0x00 && j<6){
	  fileInDir[k] = diskDir.entries[i].name[j];
	  j++;
	  k++;
	}
	fileInDir[k] = ' ';
	k++;
	j=0;
      }
    }
    printString(fileInDir);
    printString("\n\r\0"); 
  }
 
  /*ps command to show processes that are executing.*/
  else if(input[0] == 'p' && input[1] == 's'){
    showProcesses();
  }

  /*kill coomand to kill the executing process in the given index */
  else if(input[0] == 'k' && input[1] == 'i' && input[2] == 'l' && input[3] == 'l'){
    i = killHelp(input[5]);
    result = kill(i);
    if(result == 1){
      printString("Succesfully killed!");
    }else{
      printString("Unsuccesfully killed!");
    }
  }


  else{
    //interrupt(0x21, 0x00, "Unrecognized command.", 0, 0);
    printString("Unrecognized command.");
  }
  // }
}

/*Helper for KILL command*/
int killHelp(char ch){
  if(ch=='0'){
    return 0;
    
  }
  if(ch=='1'){
    return 1;
    
  }
  if(ch=='2'){
    return 2;
    
  }
  if(ch=='3'){
    return 3;
    
  }
  if(ch=='4'){
    return 4;
    
  }
  if(ch=='5'){
    return 5;
    
  }
  if(ch=='6'){
    return 6;
    
  }
  if(ch=='7'){
    return 7;
    
  }
}
