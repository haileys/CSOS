#include <csos.h>
#include <stdio.h>
#include <string.h>

int main()
{
	/*
	char* msg = "Hello world from init! I am process number: ";
	write(stdout, msg, strlen(msg));
	
	uint pid = get_pid();
	char buff[16];
	char* bptr = (char*)buff + 16;
	uint len = 0;
	do
	{
		*--bptr = (pid % 10) + '0';
		pid /= 10;
		len++;
	}
	while(pid != 0);
	write(stdout, bptr, len);
    
	write(stdout, "\n", 1);
	
//	write(stdout, &pid, 1);
	*/
	
	printf("Hello world from init! I am process number: %d\n", get_pid());
}
