clean:
	rm kbin/* ubin/* ksrc/*.o -f

CFLAGS=-std=c99 -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-stack-protector -c -masm=intel
KCFLAGS=-iquote ../kinc ${CFLAGS}
USERINCLUDES=-I./uinc/
USERCFLAGS=${USERINCLUDES} ${CFLAGS}

assembly:
	nasm -f elf -o kbin/aaa_loader_asm_o ksrc/loader.asm
	nasm -f elf -o kbin/helper_asm.o ksrc/helper.asm
	nasm -f elf -o kbin/panic_asm.o ksrc/panic.asm
	nasm -f elf -o kbin/isrs_asm.o ksrc/isrs.asm
	nasm -f elf -o kbin/task_asm.o ksrc/task.asm
	nasm -f elf -o kbin/syscall_asm.o ksrc/syscall.asm
# build the CRT
	nasm -f elf -o user/crt.o user/crt/csos.asm

# Usermode crap
usermode:
	gcc ${USERCFLAGS} user/init.c -o ubin/init.o
	ld -T user_linker.ld -o build/init.bin ubin/*.o

util:
	gcc -std=c99 -o util/bin2nasm util/bin2nasm.c

custom:
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

kernel:
	make -C ksrc kernel CFLAGS="${KCFLAGS}"
# Link
	ld -T kernel_linker.ld -o build/kernel.sys kbin/*.o ksrc/*.o
# Inject ktrace symbols @TODO: improve
	gcc -std=c99 -o util/inject_symbols util/inject_symbols.c
	perl util/find_symbols.pl build/kernel.sys | util/inject_symbols build/kernel.sys


HDDIMG=hdd.img
image:
# reset the disk image
	dd if=/dev/zero of=${HDDIMG} bs=33546240 count=1 status=noxfer
	parted --script ${HDDIMG} mklabel msdos 2> /dev/null
	parted --script ${HDDIMG} mkpartfs primary fat16 0 31 2> /dev/null
	parted --script ${HDDIMG} mkfs 1 fat16 2> /dev/null
	parted --script ${HDDIMG} set 1 boot on
# copy all files onto the system
	bash -c 'if [[ ! -x mnt ]]; then mkdir mnt --mode=0777; \
		else sudo umount mnt; fi; exit 0'
	sudo mount -o loop,offset=512 ${HDDIMG} mnt
	sudo cp -r build/* mnt
	bash -c 'fuser -a mnt; exit 0' # DEBUGGING
	sudo umount mnt
	sudo rm -rf mnt
# install bootloader
	grub --device-map=/dev/null --batch < grubscript #> /dev/null

all: assembly usermode util custom
	@echo "\n\n\n"
	@echo Now building the kernel
	@echo "\n\n\n"
	make -j1 kernel image

run:
	bochs -qf bochslinux.bxrc
