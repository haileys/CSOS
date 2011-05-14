#include <stdlib.h>

uint rand_MT[624];
uint rand_index = 0;

void srand(uint seed)
{
	rand_MT[0] = seed;
	for(uint i = 1; i < 624; i++)
	{
		rand_MT[i] = (1812433253 * (rand_MT[i - 1] ^ (rand_MT[i - 1] >> 30)) + 1) & 0xffffffff;
	}
}

static void rand_gen()
{
	for(int i = 0; i < 624; i++)
	{
		uint y = (rand_MT[i] >> 31) + ((rand_MT[(i+1) % 624]) & 0x7fffffff);
		rand_MT[i] = rand_MT[(i + 397) % 624] ^ (y >> 1);
		if(y % 2 != 0)
		{
			rand_MT[i] = rand_MT[i] ^ 2567483615;
		}
	}
}

int rand()
{
	if(rand_index == 0)
		rand_gen();
	
	int y = rand_MT[rand_index];
	y = y ^ (y >> 11);
	y = y ^ ((y << 7) & 2636928640);
	y = y ^ ((y << 15) & 4022730752);
	y = y ^ (y >> 18);
	
	rand_index = (rand_index + 1) % 624;
	return y;
}

int abs(int a)
{
	if(a < 0)
		return -a;
	return a;
}
long int labs(long int a)
{
	if(a < 0)
		return -a;
	return a;
}

div_t div(int a, int b)
{
	div_t r = {
		.quot = a / b,
		.rem = a % b,
	};
	return r;
}

ldiv_t ldiv(long int a, long int b)
{
	ldiv_t r = {
		.quot = a / b,
		.rem = a % b,
	};
	return r;
}

void exit(int status)
{
	sys_exit(status);
}