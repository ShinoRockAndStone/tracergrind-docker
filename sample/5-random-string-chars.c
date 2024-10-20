#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* reference = "potato";

int rrand(int from, int to)
{
	int delta = to - from;
	int value = rand() % delta;
	
	return from + value;
}


int main()
{
	srand(time(NULL));

	char* final = malloc(6);
	final[5] = '\0';

	for (int i = 0; i < 5; i++)
	{
	final[i] = reference[rrand(0, 5)];
	}

	printf("%s\n", final);
}


