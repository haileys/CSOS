#include "string.h"
#include "stdarg.h"
#include "util.h"
#include "panic.h"
#include "stdbool.h"

void* memcpy(void* dest, void* src, size_t n)
{
	char* d = dest;
	char* s = src;
	while(n-- > 0)
		*d++ = *s++;
	
	return dest;
}

void* memmove(void* dest, void* src, size_t n)
{
	char* d = dest;
	char* s = src;
	if(d > s)
	{
		while(n > 0)
		{
			d[n - 1] = s[n - 1];
			n--;
		}
	}
	else
	{
		for(size_t i = 0; i < n; i++)
		{
			d[i] = s[i];
		}
	}	
	return dest;
}

void* memset(void* mem, char c, size_t len)
{
	for(size_t i = 0; i < len; i++)
	{
		((char*)mem)[i] = c;
	}
	return mem;
}

size_t strlen(char* str)
{
	size_t len = 0;
	while(*str++)
		len++;
	return len;
}

char* strcpy(char* dest, char* src)
{
	char* d = dest;
	while(*src)
		*d++ = *src++;
	*d = 0;
	return dest;
}

char* strncpy(char* dest, char* src, size_t len)
{
	char* d = dest;
	for(size_t i = 0; *src && i < len - 1; i++)
		*d++ = *src++;
	*d = 0;
	return dest;
}

int memcmp(void* a, void* b, size_t len)
{
	char* _a = (char*)a;
	char* _b = (char*)b;
	
	for(uint i = 0; i < len; i++)
	{
		if(_a[i] < _b[i])
			return -1;
		if(_a[i] > _b[i])
			return 1;
	}
	return 0;
}

int strcmp(char* a, char* b)
{
	while(*a && *b)
	{
		a++;
		b++;
	}
	
	if(*a < *b)
		return -1;
	if(*a > *b)
		return 1;
	return 0;
}

char* strntrim(char* buff, size_t len, char* str)
{
	bool frontspaces = true;
	uint di, si;
	for(di = 0, si = 0; (di < len) && (str[si] != 0); si++)
	{
		if(str[si] != ' ')
			frontspaces = false;
		if(!frontspaces || str[si] != ' ')
			buff[di++] = str[si];
	}
	while(buff[--di] == ' ')
		buff[di] = 0;
	return buff;
}

char* vsnprintf(char* str, size_t len, char* fmt, va_list va)
{
	size_t i = 0;
	str[len - 1] = 0;
	
	if(str[0] == '1' && str[1] == 's' && str[2] == 't')
	{
		__asm__ volatile("nop");
	}
	
	for(; *fmt && i < len - 1; fmt++)
	{
		if(*fmt != '%')
		{
			str[i++] = *fmt;
			continue;
		}
		
		fmt++;
		switch(*fmt)
		{
			case 's':
			{
				char* s = va_arg(va, char*);
				while(*s && i < len - 1)
					str[i++] = *s++;
			}
			break;
			case 'c':
			{
				str[i++] = va_arg(va, char);
				break;
			}	
			case 'd':
			{
				char tmp[20];
				int n = va_arg(va, int);
				if(n < 0)
				{
					str[i++] = '-';
					n = -n;
				}
				int j = 0;
				if(n == 0)
					tmp[j++] = '0';
				while(n != 0)
				{
					tmp[j++] = n % 10 + '0';
					n /= 10;
				}
				while(j > 0 && i < len - 1)
					str[i++] = tmp[--j];
			}
			break;
			case 'x':
			{
				char* charset = "0123456789abcdef";
				char tmp[20];
				uint n = va_arg(va, uint);
				int j = 0;
				if(n == 0)
					tmp[j++] = '0';
				while(n != 0)
				{
					tmp[j++] = charset[n % 16];
					n /= 16;
				}
				while(j > 0 && i < len - 1)
					str[i++] = tmp[--j];
			}
			break;
			default:
			{
				char x[] = { *fmt, 0 };
				panicf("unknown format specifier %s", x);
				break;
			}
		}
	}
	
	str[i] = 0;
	return str;
}

char* snprintf(char* str, size_t len, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	return vsnprintf(str, len, fmt, va);
}

char* strtoupper(char* dest, char* src)
{
	for(uint i = 0; src[i]; i++)
	{
		if(src[i] >= 'a' && src[i] <= 'z')
			dest[i] = 'A' + (src[i] - 'a');
		else
			dest[i] = src[i];
	}
	return dest;
}