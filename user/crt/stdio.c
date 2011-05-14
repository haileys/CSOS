#include <stdio.h>
#include <stdarg.h>
#include <csos.h>
#include <string.h>

FILE _stdin = {
	.fd = 0,
};
FILE _stdout = {
	.fd = 1,
};
FILE _stderr = {
	.fd = 1, // @TODO implement separate stderr stream
};

FILE* stdin = (FILE*)&stdin;
FILE* stdout = (FILE*)&stdout;
FILE* stderr = (FILE*)&stderr;

size_t fwrite(void* buff, size_t size, size_t count, FILE* stream)
{
	return sys_write(stream->fd, buff, size * count);
}

size_t fread(void* buff, size_t size, size_t count, FILE* stream)
{
	return sys_read(stream->fd, buff, size * count);
}

int fprintf(FILE* stream, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vfprintf(stream, fmt, va);
	va_end(va);
	return r;
}
int vfprintf(FILE* stream, char* fmt, va_list vargs)
{
	char o[1024];
	uint chars = vsnprintf(o, 1024, fmt, vargs);
	fwrite(o, chars, 1, stream);
	return chars;
}
int printf(char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = fprintf(stdout, fmt, va);
	va_end(va);
	return r;
}
int vprintf(char* fmt, va_list vargs)
{
	return vfprintf(stdout, fmt, vargs);
}
int sprintf(char* buff, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vsprintf(buff, fmt, va);
	va_end(va);
	return r;
}
int snprintf(char* buff, size_t len, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vsnprintf(buff, len, fmt, va);
	va_end(va);
	return r;
}
int vsprintf(char* buff, char* fmt, va_list vargs)
{
	return vsnprintf(buff, INT_MAX, fmt, vargs);
}
int vsnprintf(char* str, size_t len, char* fmt, va_list va)
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
				if(i < len - 2)
				{
					str[i++] = '%';
					str[i++] = *fmt;
				}
			}
		}
	}
	
	str[i] = 0;
	return i++;
}