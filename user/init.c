#include <csos.h>

uint strlen(char* str)
{
	uint len = 0;
	while(*str++)
		len++;
	return len;
}

int main()
{
	char* msg = "Hello world from init! I am process number: ";
	write(stdout, "Hello world from init! I am process number: ", strlen(msg));
	
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
}