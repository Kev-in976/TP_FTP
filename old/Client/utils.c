/*int nombre = 42;
char str[20];
snprintf(str, sizeof(str), "%d", nombre);*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char * int2string(int a)
{
	char *str; /*initializing a buffer int-sized*/
	str = malloc(sizeof(int));
	snprintf(str, sizeof(str), "%d", a);
	return str;
}

/*int main() {

	int a = 42;
	printf("42 en string : [%s]\n", int2string(a));
	return 0;
}*/
