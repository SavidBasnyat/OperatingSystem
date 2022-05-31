#!/bin/bash

source ../PathSetter.bash

#Creating Disk Image
dd if=/dev/zero of=floppya.img bs=512 count=2880

dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc seek=0

#Compile, Assemble and Link the Kernel
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
#ld86 -o kernel -d kernel.o kernel_asm.o

#Copy Kernel into disk
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

#Copy message.txt into disk
dd if=message.txt of=floppya.img bs=512 conv=notrunc seek=30

#Add file to delete
dd if=delete.txt of=floppya.img bs=512 conv=notrunc seek=30



# Add disk map and disk directory to disk image
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

# Compile loadFile.c
gcc -o loadFile loadFile.c

./loadFile message.txt
./loadFile delete.txt

#Assemble lib.asm, compile uprog1.c, link the two object files and place it on the disk
as86 lib.asm -o lib.o
bcc -ansi -c -o uprog1.o uprog1.c
ld86 -o uprog1 -d uprog1.o lib.o
./loadfile uprog1

#Compile uprog2.c, link the two object files and place it on the disk

bcc -ansi -c -o uprog2.o uprog2.c
ld86 -o uprog2 -d uprog2.o lib.o
./loadFile uprog2

#Compile shell2.c, link the three object files and place it on the disk
bcc -ansi -c -o shell2.o shell2.c
./loadFile shell2
bcc -ansi -c -o userlib.o userlib.c
ld86 -o shell2 -d shell2.o userlib.o lib.o

#Compile texted.c, link the three object files and place it on the disk
bcc -ansi -c -o uprog3.o uprog3.c
./loadFile uprog3
bcc -ansi -c -o uprog3.o uprog3.c
ld86 -o uprog3 -d uprog3.o userlib.o lib.o

#Compile proc.c and link it with kernel
bcc -ansi -c -o proc.o proc.c
ld86 -o kernel -d kernel.o proc.o kernel_asm.o

#Testing yield:
bcc -ansi -c –o uprog6.o uprog6.c
ld86 -o uprog6 -d uprog6.o lib.o userlib.o
./loadfile uprog6

bcc -ansi -c –o uprog7.o uprog7.c
ld86 -o uprog7 -d uprog7.o lib.o userlib.o
./loadfile uprog7


bochs -f opsys.bxrc




