#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINELEN (80)

char *mygets(char *buf, int len);

int main(int argc, char *argv[])
{
	
	if (argc != 2)
	{
		return -1;
	}
	char str1[LINELEN+1];
	char str2[LINELEN+1];
	char exec[LINELEN+1]="./";//appending ./ for the execution for the child process
	strcat(exec,argv[1]);//appending ./ to the name of the executable for the child process
	char* argsCheck[4];
	while (1)
	{
		if (mygets(str1, LINELEN) == NULL)
			break;
		if (mygets(str2, LINELEN) == NULL)
			break;
		int cid = fork();//creating child process
		if(cid<0)return -1;
		else if(cid==0)//block that deals with child process
		{
			argsCheck[0]=exec;
			argsCheck[1]=str1;
			argsCheck[2]=str2;
			argsCheck[3]=NULL;
			execvp(argsCheck[0],argsCheck);
			return -2;
		}
		int status;
		waitpid(cid,&status,0);//waiting for child process
		printf("%d\n",WEXITSTATUS(status));
		fflush(stdout);
	}
	return 0;
}

char* mygets(char *buf, int len)
{
	char *retval;
	retval = fgets(buf, len, stdin);
	buf[len] = '\0';
	if (buf[strlen(buf) - 1] == 10) /* trim \r */
		buf[strlen(buf) - 1] = '\0';
	else if (retval) while (getchar() != '\n'); /* get to eol */
	return retval;
}