#include <string.h>
#include <ctype.h>

#define LINELEN (80)

// Comparing strings by number of digits
int mystrcmp(const char *str1, const char *str2)
{
	int countDig1=0;
	int countDig2=0;
	int length1= (int)strlen(str1);
	int length2= (int)strlen(str2);
	for(int i=0;i<length1;i++)
	{
		if(isdigit(*(str1+i)))countDig1++;
	}
	for(int i=0;i<length2;i++)
	{
		if(isdigit(*(str2+i)))countDig2++;
	}
	int val = countDig1-countDig2;
	if (val > 0)
		return 1;
	if (val < 0)
		return 2;
	if(val == 0)
		return 0;
	return -1;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		return -1;
	}
	return(mystrcmp(argv[1], argv[2]));
}