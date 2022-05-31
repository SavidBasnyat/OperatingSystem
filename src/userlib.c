
int printString(char *str){
  return interrupt(0x21, 0x00, str, 0, 0);
}

int readString(char *buf) {
  return interrupt(0x21, 0x01, buf, 0, 0);
}

int readSector(char *buf, int absSector){
  return interrupt(0x21, 0x02, buf, absSector, 0);
}

int readChar(char *buf) {
  interrupt(0x21, 0x11, buf, 0, 0);
  return buf;
}

int readfile(char *filename, char *buf){
  return interrupt(0x21, 0x03, filename, buf, 0);
}

int executeProgram(char* name, int segment) {
  return interrupt(0x21, 0x04, name, segment, 0);
}

int deleteFile(char *fname){
  return interrupt(0x21, 0x07, fname, 0, 0);
}

int writeFile(char *fname, char *buffer, int sectors){
  return interrupt(0x21, 0x08, fname, buffer, sectors);
}

void terminate() {
  return interrupt(0x21, 0x05, 0, 0, 0);
}

void yield(){
  return interrupt(0x21, 0x09, 0, 0, 0);
}

void showProcesses(){
  return interrupt(0x21, 0x0A, 0, 0, 0);
}

int kill(int segment){
   return interrupt(0x21, 0x0B, segment, 0, 0);
}
