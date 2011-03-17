#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
	char magic[] = { 0xff, 0xff, 0xff, 0xff, 0xef, 0xbe, 0xad, 0xde, 0xde, 0x00, 0xdf, 0xba, 0xff, 0xff, 0xff, 0xff };
	int magic_len = 16;
	
	FILE* f = fopen(argv[1], "r");
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* buff = (char*)malloc(len);
	fread(buff, len, 1, f);
	fclose(f);
	
	// find magic bit
	int buffoffset = 0;
	for(int i = 0; i < len; i++)
	{
		int j = 0;
		if(buff[i] == magic[j])
		{
			for(j = 1; j < magic_len; j++)
			{
				if(buff[++i] != magic[j])
					break;
			}
			if(j == magic_len)
			{
				buffoffset = i + 1;
				break;
			}
		}
	}
	if(buffoffset == 0)
	{
		printf("*** Couldn't find magic bit\n");
		return 1;
	}
	
	while(!feof(stdin))
	{
		int offset;
		char symbol[28];
		memset(symbol, 0, 28);
		scanf("%x %s\n", &offset, &symbol);
		*(int*)(buff + buffoffset) = offset;
		memcpy(buff + buffoffset + 4, symbol, 28);
		buffoffset += 32;
	}
	
	f = fopen(argv[1], "w");
	fwrite(buff, len, 1, f);
	fclose(f);
}