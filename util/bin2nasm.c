// i wrote this because objdump was giving me the shits

#include <stdio.h>

// cat <somebinary> | bin2nasm <objname> | nasm -f elf -o <whatever> /proc/self/fd/0

int main(int argc, char** argv)
{
	int c;
	int len = 0;
	printf("section .data\nglobal bin_%s\nglobal bin_%s_len\n\nbin_%s:\n", argv[1], argv[1], argv[1]);
	while(1)
	{
		c = getchar();
		if(c == EOF)
			break;
		len++;
		printf("\tdb 0x%x\n", c);
	}
	printf("nop\n\nbin_%s_len dd %d\n", argv[1], len);
}