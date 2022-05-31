# OperatingSystem
Technical Report: 

16-bit Operating Sytem
Introduction/Overview:

As part of Operating System course, a 16-bit operating system in MacOS High Sierra was implemented. The OS was built by dividing it into 5 different parts, each building upon the previous ones. The OS provides basic functionalities including reading/writing a file, executing a user program and even has a command line interface through which users can interact with it. It also supports multiprogramming and time-sharing using a round-robin scheduling algorithm.

Implementation:
At the beginning of the project, some initial files were provided which included bootload.asm and kernel.asm. The bootload.asm contained assembly code for the bootloader which sets up the segment registers and the stack to memory address 0x10000 where the kernel is placed, reads 10 sectors (5120 bytes) from the disk starting at sector 3, puts them at address 0x10000 and lastly jumps to 0x10000, starting the kernel program. The kernel.asm file on the other hand, contains assembly language routines that will be used in the kernel.c file.

In part 1, two functions in kernel.c were implemented. The first one was the putChar function which displays a character in a specified color at a specified location on the screen. To implement this function, the putInMemory function provided by the assembly code was used. The second function that was added to kernel.c was putStr. This function displays a string in a specified color at a specified location on the screen and was implemented using the putChar function.

For part 2, several new methods in the kernel were implemented: printString, readChar and readString. To implement those functions, the ISR that was provided by function named interrupt in the kernel.asm file was utilized. The interrupt function takes five parameters: the interrupt number and the parameters for the interrupt that are to be passed in the AX, BX, CX, and DX registers, respectively. The printSting function prints out each character of the string to the terminal by calling interrupt 0x10, the readChar function reads a character typed into the terminal using the interrupt 0x16 and the readString function read characters into a character array until the ENTER key is pressed using functionalities of readChar and calling interrupt 0x10. readSector and writeSector functions were also added to read and write sectors from/to the disk respectively which was useful in creating functions in later projects. After that, handleInterrupt21 function was implemented to create system calls for functions such as readString, readChar, printString and ones implemented later. Doing so made it possible for user written programs to invoke these functions.

For part 3, readfile, executeProgram and terminate functions were implemented. The readFile function reads the disk directory and searches it for the indicated filename. If the file is in the disk directory, it reads each sector of the file and stores it in a buffer. Otherwise, if the file is not found, it returns –1. The executeProgram function executes a user program. It takes 2 parameters as input, the name of the file and the segment where it runs. The function uses readFile function to load the executable file from disk into a buffer and then transfers the executable program from the buffer into the memory segment using the putInMemory function where it will be executed. Lastly, the terminate function returns control to the operating system when a program completes executing.

In part 3, a Shell interface as a user program was also created. By the end of project 3, the command line shell recognized a few commands by the user: type and execute. The type command displays the contents of a file with the help of readFile function in the kernel whereas execute command executes the program specified by the user using the executeProgram function in the kernel. In the very end of this project, a user program library that provides functions that provide access to all the OS services provided by our kernel was also created.

For part 4, functions for deleting and writing files were created , and few more commands to the shell were added. The deleteFile function takes in the name of the file as input and deletes the file specified by the name. To do that, the function marks all the sectors allocated to the file as free in the DiskMap and sets the first character of the file name to 0x00. If the file does not exist in the disk directory, it simply just returns -1. The writeFile function takes in file name, buffer and sectors as input and writes sectors * 512 bytes of data from the buffer into the file with the specified name in the disk directory. If the file already exists, it overwrites the file with the new content. To do so, the function finds an empty entry in the Disk Directory and finds a free sector in the Disk Map for each sector making up the file. The data for each sector is then written into a free sector on the disk and the sector numbers that are used are entered into the Disk Directory entry for the file. As said earlier, several commands that the shell would recognize from the users were added: delete, copy and dir. The delete command deletes the filename specified by the user from the disk or prints “File not found” if the file does not exist. All of this is achieved using the deletFile function in the kernel. The copy command makes a copy of the file specified by the user as a new file which is also specified by the user. If the destination exists already, it is overwritten and if the source does not exist, it prints “File not found”. To implement this command, readFile is used to read the source file and store it into a buffer and writeFile is used after that to write the contents of the buffer into the destination file.

At the end of part 4, a simple line-based text editor that can be executed as a user program was also created. The text editor prompts the user to enter the name of the file and then reads lines of text from the
user, storing them into a buffer. Once the user presses Ctrl-D followed by the ENTER key, the text editor writes the content of the buffer into the file and terminates. If the filename already exists, the file is overwritten.

In the last part, the OS was enhanced so that it can support multiprogramming. To do so, mechanisms for memory management, time-sharing and process management were implemented. At the very beginning, the handleTimerInterrupt function was implemented that uses makeTimerInterrupt, timer_ISR and returnFromTimer functions from the kernel.asm file. The handleTimerInterrupt method is invoked each time a timer interrupt occurs. When it is called, the function takes the segment and the stack pointer of the process that was interrupted as input. It first saves the stack pointer into the PCB of the running process, marks that process as READY and adds it to the tail of the ready queue. After that, the PCB from the head of ready queue is removed, marked as running and the running variable points to it. Once this is done, the returnFromTimer method is called, which restores the context of the process. By implementing everything mentioned above, time-sharing was ensured. For ensuring memory/process management, memory segment maps were used to keep track of used/free segments and used PCBs defined as struct to save data related to processes. Various functions in pro.c file were implemented such as getFreeMemorSegment, getFreePCB, releaseMemorySegment, releaseFreePCB, addToReady and removeFromReady. Then executeProgram function was modified in kernel.c. The function executeProgram now finds a free memory segment for a process, uses a PCB to store data of the process, loads the program into the free memory segment and then places the PCB into the ready queue. The terminate function was also implemented which now frees the memory segment used by a process when it terminates, frees the PCB for the process and sets the state of the process to DEFUNCT.

Setup Process:
The Operating System runs in a host macOS (High Sierra or earlier). To set up the OS the user needs to have the following files/tools in a directory:

• bochs-2.4.2: directory containing the bochs x86

• emulator. dev86-0.16.17: directory containing the bcc, as86 and ld86 tools.

• Bootload.asm: assembly code for the boot loader.

• Kernel.asm: assembly language routines you will use in your kernel.

• Opsys.bxrc: bochs configuration file.

• PathSetter.bash: script file for setting some required environment variables.

• Kernel.c: code for the kernel which contains most of the

• floppya.img and dir.img: a Disk Directory image containing the kernel and a Disk Map with the kernel sectors marked as used.

• userlib.c and userlib.h: a user program library that provides functions that provide access to all the OS services provided by the kernel.

• proc.c and proc.h: definition/implementation of data structures and functions used to manage memory and processes

• Shell2.c: code for Command line interface that runs when OS starts

• CompileOS.bash: script file to assemble, compile and load the necessary files

Once the user has the above files, they will need to download XQuartz to their machine. Through the XQuartz terminal, the user will need to run the compileOS.bash file which contains code to load, compile and execute necessary files. Doing this will set up the project and run the OS console.

User commands supported:
The shell supports the following user programs:

1. type <file>: The contents of the file are printed on the screen and if the file does not exist on the disk an error message is printed out.
  
2. execute <file>: The file is loaded into memory and executed. If the file does not exist on the disk an error message is printed out.
  
3. delete <file>: The file is removed from the disk and if the file does not exist, it prints “File not found.”
  
  
4. copy <src> <dest>: The file specified in src is copied into a file specified by dest. If the source
file is not found, “File not found” is printed. If the destination file already exists, the file is overwritten.
  
5. dir: All files currently stored on the disk are displayed.
