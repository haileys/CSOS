#!/bin/bash

rm bin/*

CFLAGS="-std=c99 -iquote kinc -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-stack-protector -c -masm=intel $1"

nasm -f elf -o bin/aaa_loader_asm.o ksrc/loader.asm
nasm -f elf -o bin/helper_asm.o ksrc/helper.asm
nasm -f elf -o bin/panic_asm.o ksrc/panic.asm
nasm -f elf -o bin/isrs.o ksrc/isrs.asm

gcc -std=c99 -o util/bin2nasm util/bin2nasm.c
# custom binaries to include in image!!

	# realmode.bin
	nasm -f bin -o bin/realmode_asm.bin ksrc/realmode/realmode.asm
	util/bin2nasm code16 < bin/realmode_asm.bin > bin/tmp.asm
	nasm -f elf -o bin/realmode_asm.o bin/tmp.asm
	rm bin/tmp.asm
	
	# ata.bin
	nasm -f bin -o bin/realmode_ata.bin ksrc/realmode/ata.asm
	util/bin2nasm ata16 < bin/realmode_ata.bin > bin/tmp.asm
	nasm -f elf -o bin/realmode_ata.o bin/tmp.asm
	rm bin/tmp.asm
	
	# int.bin
	nasm -f bin -o bin/realmode_int.bin ksrc/realmode/int.asm
	util/bin2nasm int16 < bin/realmode_int.bin > bin/tmp.asm
	nasm -f elf -o bin/realmode_int.o bin/tmp.asm
	rm bin/tmp.asm

gcc $CFLAGS ksrc/main.c -o bin/main.o
gcc $CFLAGS ksrc/console.c -o bin/console.o
gcc $CFLAGS ksrc/string.c -o bin/string.o
gcc $CFLAGS ksrc/io.c -o bin/io.o
gcc $CFLAGS ksrc/util.c -o bin/util.o
gcc $CFLAGS ksrc/gdt.c -o bin/gdt.o
gcc $CFLAGS ksrc/idt.c -o bin/idt.o
gcc $CFLAGS ksrc/io.c -o bin/io.o
gcc $CFLAGS ksrc/panic.c -o bin/panic.o
gcc $CFLAGS ksrc/kmalloc.c -o bin/kmalloc.o
gcc $CFLAGS ksrc/realmode.c -o bin/realmode.o
gcc $CFLAGS ksrc/ata.c -o bin/ata.o
gcc $CFLAGS ksrc/kb.c -o bin/kb.o
gcc $CFLAGS ksrc/part.c -o bin/part.o
gcc $CFLAGS ksrc/fs/fat.c -o bin/fs_fat.o
gcc $CFLAGS ksrc/fs/vfs.c -o bin/fs_vfs.o

ld -T linker.ld -o build/kernel.sys bin/*.o
gcc -std=c99 -o util/inject_symbols util/inject_symbols.c
perl util/find_symbols.pl build/kernel.sys | util/inject_symbols build/kernel.sys

dd if=/dev/zero of=hdd.img bs=33546240 count=1 status=noxfer &>/dev/null

parted --script hdd.img mklabel msdos
parted --script hdd.img mkpartfs primary fat16 0 31 &> /dev/null
parted --script hdd.img mkfs 1 fat16 &> /dev/null
parted --script hdd.img set 1 boot on

rm -rf mnt
mkdir mnt
mount -o loop,offset=512 hdd.img mnt
cp -r build/* mnt
umount mnt
rm -rf mnt

grub --device-map=/dev/null --batch < grubscript &> /dev/null