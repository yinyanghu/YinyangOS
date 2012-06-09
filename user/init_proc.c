int *c;

int main(void)
{
	c = (int *)0xC01E3704;
	while (1)
	{
		*c = (*c + 1) % 10;
	}
	return 0;
}
