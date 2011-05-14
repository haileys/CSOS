#!/bin/bash

rm kbin/*

for f in $(find build | grep "/\."); do
	rm $f
done

CFLAGS="-std=c99 -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-stack-protector -c -masm=intel "
KCFLAGS="-iquote kinc $CFLAGS $1"
USERCFLAGS="-Iuinc $CFLAGS"

nasm -f elf -o kbin/aaa_loader_asm.o ksrc/loader.asm
nasm -f elf -o kbin/helper_asm.o ksrc/helper.asm
nasm -f elf -o kbin/aaa_panic_asm.o ksrc/panic.asm
nasm -f elf -o kbin/isrs_asm.o ksrc/isrs.asm
nasm -f elf -o kbin/task_asm.o ksrc/task.asm
nasm -f elf -o kbin/syscall_asm.o ksrc/syscall.asm

#build the CRT

rm ubin/* &> /dev/null
nasm -f elf -o ubin/crt.o user/crt/csos.asm
gcc $USERCFLAGS user/crt/stdlib.c -o ubin/stdlib.o
gcc $USERCFLAGS user/crt/stdio.c -o ubin/stdio.o
gcc $USERCFLAGS user/crt/string.c -o ubin/string.o
rm user/crt.a
ar q user/crt.a ubin/*.o

#user mode crap
	rm ubin/* &> /dev/null
	gcc $USERCFLAGS user/init.c -o ubin/init.o
	ld -T user_linker.ld -o build/init.bin ubin/*.o user/crt.a user/csoslua.a #user/pdclib/functions/stdlib/*.o user/pdclib/functions/_PDCLIB/*.o user/pdclib/functions/stdio/*.o user/pdclib/functions/string/*.o

gcc -std=c99 -o util/bin2nasm util/bin2nasm.c
# custom binaries to include in image!!

	# realmode.bin
	nasm -f bin -o kbin/realmode_asm.bin ksrc/realmode/realmode.asm
	util/bin2nasm code16 < kbin/realmode_asm.bin > kbin/tmp.asm
	nasm -f elf -o kbin/realmode_asm.o kbin/tmp.asm
	rm kbin/tmp.asm
	
	# ata.bin
	nasm -f bin -o kbin/realmode_ata.bin ksrc/realmode/ata.asm
	util/bin2nasm ata16 < kbin/realmode_ata.bin > kbin/tmp.asm
	nasm -f elf -o kbin/realmode_ata.o kbin/tmp.asm
	rm kbin/tmp.asm
	
	# int.bin
nasm -f bin -o kbin/realmode_int.bin ksrc/realmode/int.asm
	util/bin2nasm int16 < kbin/realmode_int.bin > kbin/tmp.asm
	nasm -f elf -o kbin/realmode_int.o kbin/tmp.asm
	rm kbin/tmp.asm

gcc $KCFLAGS ksrc/main.c -o kbin/main.o
gcc $KCFLAGS ksrc/console.c -o kbin/console.o
gcc $KCFLAGS ksrc/string.c -o kbin/string.o
gcc $KCFLAGS ksrc/io.c -o kbin/io.o
gcc $KCFLAGS ksrc/util.c -o kbin/util.o
gcc $KCFLAGS ksrc/gdt.c -o kbin/gdt.o
gcc $KCFLAGS ksrc/idt.c -o kbin/idt.o
gcc $KCFLAGS ksrc/io.c -o kbin/io.o
gcc $KCFLAGS ksrc/panic.c -o kbin/panic.o
gcc $KCFLAGS ksrc/kmalloc.c -o kbin/kmalloc.o
gcc $KCFLAGS ksrc/realmode.c -o kbin/realmode.o
gcc $KCFLAGS ksrc/ata.c -o kbin/ata.o
gcc $KCFLAGS ksrc/kb.c -o kbin/kb.o
gcc $KCFLAGS ksrc/part.c -o kbin/part.o
gcc $KCFLAGS ksrc/fs/fat.c -o kbin/fs_fat.o
gcc $KCFLAGS ksrc/fs/vfs.c -o kbin/fs_vfs.o
gcc $KCFLAGS ksrc/task.c -o kbin/task.o
gcc $KCFLAGS ksrc/trap.c -o kbin/trap.o
gcc $KCFLAGS ksrc/paging.c -o kbin/paging.o
gcc $KCFLAGS ksrc/syscall.c -o kbin/syscall.o

ld -T kernel_linker.ld -o build/kernel.sys kbin/*.o
gcc -std=c99 -o util/inject_symbols util/inject_symbols.c
perl util/find_symbols.pl build/kernel.sys | util/inject_symbols build/kernel.sys

HDDIMG=hdd.img

dd if=/dev/zero of=$HDDIMG bs=33546240 count=1 status=noxfer &>/dev/null

parted --script $HDDIMG mklabel msdos
parted --script $HDDIMG mkpartfs primary fat16 0 31 &> /dev/null
parted --script $HDDIMG mkfs 1 fat16 &> /dev/null
parted --script $HDDIMG set 1 boot on

rm -rf mnt
mkdir mnt
sudo chmod -R 0777 mnt
sudo mount -o loop,offset=512 $HDDIMG mnt
sudo chmod 0777 mnt
sudo cp -r build/* mnt
sudo umount mnt
sudo rm -rf mnt

grub --device-map=/dev/null --batch < grubscript &> /dev/null

#gzip -c -9 $HDDIMG > hdd.img.gz
