main() {
void enableInterrupts();
interrupt(0x21,0x00,"Tired!\r\n\0",0,0);
interrupt(0x21,0x05,0,0,0);

}
