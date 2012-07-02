#include "oslib.h"

int main(int argc, char *argv[])
{
	printf("Child!\n");
	printf("argc %d : %x\n", argc, (unsigned int)&argc);
	printf("argv %x\n", (unsigned int)argv);
	int i;
	for (i = 0; i < argc; ++ i)
		printf("%d %x %s\n", i, (int)(argv[i]), argv[i]);
	return 0;
}
