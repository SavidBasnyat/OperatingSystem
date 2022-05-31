#include "userlib.h"

void main(){
  char input[100]; //File name
  char content[13312]; //Content that is to be written
  char arr[2]; //Holds character that is typed by user
  char space[2];
  int i = 0;

  void enableInterrupts();

  arr[1] = '\0'; 

  space[0] = 32;
  space[1] = 0x00;
  
  printString("Enter the file name:");
  readString(input);
  printString("\n\r\0");

  while(1){
    readChar(arr);
    /* Ctrl+D is typed */
    if(arr[0] == 0x04){
      readChar(arr);
      /* Return is typed */ 
      if(arr[0] == 0x0D){
	content[i] = 0x00;
	writeFile(input, content, i/512+1);
	terminate();
      }
    }
    /* Backspace is typed */
    else if(arr[0] == 0x08){
      if(i>0){
	printString(arr);
	printString(space);
	printString(arr);
	i--;
	content[i] = 0x00;
	
      }
    }
    
    else if (i <13312){
      printString(arr);
      content[i] = arr[0];
      i++;
    }
  }
  
}
