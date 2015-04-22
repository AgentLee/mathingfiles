#include <stdio.h>
#include <string.h>
#include <math.h>

char *add(char *a, char *b)
{

	double c = strtod(a, NULL);
	double d = atof(b);

	char sum[1000] = "";
	sprintf(sum, "%f\n", c + d);

	return sum;
}

int main()
{
	char *a = "2.0";
	char *b = "2.0";
	
	printf("%s\n", add(a, b));

	return 0;
}
