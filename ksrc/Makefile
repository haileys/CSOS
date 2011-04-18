default: kernel

kernel: main.o console.o string.o io.o util.o gdt.o idt.o io.o panic.o kmalloc.o realmode.o ata.o kb.o part.o task.o trap.o paging.o syscall.o 
	make -C fs
