#include "kernel.h"

#define MAX_LEN		12

void vfprintf(void (*printer)(char), const char *ctl, void **args) {

	uint_32 temp[MAX_LEN];
	int_32 dec;
	uint_32 hex;
	char *ch;
	int i;
	
	for (; *ctl != 0; ++ ctl)
	{
		if (*ctl != '%') printer(*ctl);
		else
			switch (*(++ ctl))
			{
				case 'd':	dec = *((int_32 *)args ++);
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

				case 'x':	hex = *((uint_32 *)args ++);
						for (i = 0; hex != 0; hex = hex >> 4, ++ i)
							temp[i] = hex & 0xF;
						for (-- i; i >= 0; -- i)
							if (temp[i] < 10)
								printer(temp[i] + 48);
							else
								printer(temp[i] - 10 + 65);
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

