#include "kernel.h"

#define MAX_LEN 10

void
vfprintf(void (*printer)(char), const char *ctl, void **args) {

	int temp[MAX_LEN];
	int dec;
	char *ch;
	int i;
	
	for (; *ctl != 0; ++ ctl)
	{
		if (*ctl != '%') printer(*ctl);
		else
			switch (*(++ ctl))
			{
				case 'd':	dec = *((int *)args ++);
						if (dec == 0) printer('0');
						else
						{
							if (dec < 0)
							{
								printer('-');
								dec = -dec;
							}
							for (i = 0; dec != 0; dec /= 10, ++ i)
								temp[i] = dec % 10;	
							for (-- i; i >= 0; -- i)
								printer(temp[i] + 48);
						}
						break;

				case '%':	printer('%');
						break;

				case 'c':	printer(*((char *)args ++));
						break;

				case 's':	for (ch = (char *)(*(args ++)); *ch != 0; ++ ch)
							printer(*ch);
						break; 

			}
	}


}

