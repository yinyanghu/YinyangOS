#include <stdio.h>
#include <string.h>

#define Kernel_Size	(1024*1024)
#define File_Size	(64*1024)

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Error!\n");
		return -1;
	}

	freopen("os.img", "w", stdout);		

	int i, counter;
	char c;


	//for kernel
	freopen(argv[1], "r", stdin);
	
	counter = 0;
	while (scanf("%c", &c) == 1)
	{
		++ counter;
		fprintf(stdout, "%c", c);
	}

	for (; counter != Kernel_Size; ++ counter)
		fprintf(stdout, "%c", 0);


	//for other files
	for (i = 2; i < argc; ++ i)
	{
		freopen(argv[i], "r", stdin);

		counter = 0;
		while (scanf("%c", &c) == 1)
		{
			++ counter;
			fprintf(stdout, "%c", c);
		}

		for (; counter != File_Size; ++ counter)
			fprintf(stdout, "%c", 0);
	}

	return 0;
}

