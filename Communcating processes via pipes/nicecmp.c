#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define LINELEN (80)
#define SYSCALL_FAILED -2

void killAllProcesses(char** killProc, int exitValue);
char *mygets(char *buf, int len);
int mygeti();

int main(int argc, char *argv[])
{
	/*******************************************************
	*returnVal: the returned value in the end 0 for success
	*-2 for failed syscall.
	*killProc: contains the command to kill all processes in
	*case of failiure or finishing the program.
	*buf:output of child processes within the pipe. 
	*******************************************************/
	int returnVal=0;
	char* killProc[]={"killall","-e","loopcmp",NULL};
	char *cmpstr[] = {"lexcmp", "lencmp"};
	int veclen = sizeof(cmpstr)/sizeof(char *);
	char str1[LINELEN + 1];
	char str2[LINELEN + 1];
	char buf[LINELEN+1];
	int index;
	int pfd1[2],pfd2[2],pfd3[2],pfd4[2];//0-input 1-output
	/******************************************************
	*Creating pipes for to communicate with each child
	*process.
	*each child gets one pipe for input and second pipe.
	*Parent procees hold the entire pips for communication.
	******************************************************/
	if(pipe(pfd1)==-1)return -2;
	if(pipe(pfd2)==-1)return -2;
	if(pipe(pfd3)==-1)return -2;
	if(pipe(pfd4)==-1)return -2;
	int pid2;
	int pid1=fork();
	if(pid1==0)//first child block
	{
		/*********************************************
		*Manging I/O of child process1 and excuting it
		*********************************************/
		char* myargs1[3];
		myargs1[0]="./loopcmp";
		myargs1[1]="lexcmp";
		myargs1[2]=NULL;
		dup2(pfd1[STDIN_FILENO],STDIN_FILENO);
		if(close(pfd1[STDOUT_FILENO])<0)
		{
			returnVal=SYSCALL_FAILED;
			killAllProcesses(killProc,returnVal);
		}
		if(dup2(pfd2[STDOUT_FILENO],STDOUT_FILENO)<0)
		{
			returnVal=SYSCALL_FAILED;
			killAllProcesses(killProc,returnVal);
		}
		close(pfd2[STDIN_FILENO]);
		execvp(myargs1[0],myargs1);
		printf("exec failed");
		returnVal=SYSCALL_FAILED;
		killAllProcesses(killProc,returnVal);
	}
	else if (pid1>0)//parent process block
	{
		pid2=fork();
		if(pid2==0)//second child block
		{
			/*********************************************
			*Manging I/O of child process2 and excuting it
			*********************************************/
			char* myargs2[2];
			myargs2[0]="./loopcmp";
			myargs2[1]="lencmp";
			myargs2[2]=NULL;
			dup2(pfd3[STDIN_FILENO],STDIN_FILENO);
			if (close(pfd3[STDOUT_FILENO])<0)
			{
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			dup2(pfd4[STDOUT_FILENO],STDOUT_FILENO);
			if(close(pfd4[STDIN_FILENO])<0)
			{
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			execvp(myargs2[0],myargs2);
			printf("exec failed");
			returnVal=SYSCALL_FAILED;
			killAllProcesses(killProc,returnVal);
		}
		else if(pid2<0)
		{
			printf("Unsuccessful fork 2");
			returnVal=SYSCALL_FAILED;
			killAllProcesses(killProc,returnVal);
		}
	}
	else
	{
		printf("Unsuccessful fork 1");
		return -2;
	}
	/*********************************************
	*closing read end for parent process in pipe1
	*closing write end for parent process in pipe2
	*********************************************/
	if(close(pfd1[STDIN_FILENO])<0)
	{
		printf("closing pipe failed");
		returnVal=SYSCALL_FAILED;
		killAllProcesses(killProc,returnVal);
	}
	if(close(pfd2[STDOUT_FILENO]))
	{
		printf("closing pipe failed");
		returnVal=SYSCALL_FAILED;
		killAllProcesses(killProc,returnVal);
	}
	/*********************************************
	*closing read end for parent process in pipe3
	*closing write end for parent process in pipe4
	*********************************************/
	if(close(pfd3[STDIN_FILENO])<0)
	{
		printf("closing pipe failed");
		returnVal=SYSCALL_FAILED;
		killAllProcesses(killProc,returnVal);
	}
	if(close(pfd4[STDOUT_FILENO]))
	{
		printf("closing pipe failed");
		returnVal=SYSCALL_FAILED;
		killAllProcesses(killProc,returnVal);
	}
	while (1)
	{
		printf("Please enter first string:\n");
		if (mygets(str1, LINELEN) == NULL)
			break;

		printf("Please enter second string:\n");
		if (mygets(str2, LINELEN) == NULL)
			break;
		do {
			printf("Please choose:\n");
			for (int i=0 ; i < veclen ; i++)
				printf("%d - %s\n", i, cmpstr[i]);
			index = mygeti();
		} while ((index < 0) || (index >= veclen));
		printf("%s(%s,%s)==",cmpstr[index],str1,str2);
		fflush(stdout);//printing the previous line first to screen
		//appending \n for triming the string for mygets in child process
		strcat(str1,"\n");
		strcat(str2,"\n");
		if(index==0)
		{
			if(write(pfd1[STDOUT_FILENO],str1,strlen(str1))<0)
			{
				printf("failed writing lex1");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			if(write(pfd1[STDOUT_FILENO],str2,strlen(str2))<0)
			{
				printf("failed writing lex2");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			if(read(pfd2[STDIN_FILENO],buf,strlen(buf)+1)<0)
			{
				printf("failed reading lex");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
		}
		else
		{
			if(write(pfd3[STDOUT_FILENO],str1,strlen(str1))<0)
			{
				printf("failed writing len1");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			if(write(pfd3[STDOUT_FILENO],str2,strlen(str2))<0)
			{
				printf("failed writing len2");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
			if(read(pfd4[STDIN_FILENO],buf,strlen(buf)+1)<0)
			{
				printf("failed reading len");
				returnVal=SYSCALL_FAILED;
				killAllProcesses(killProc,returnVal);
			}
		}
		printf("%s\n",buf);
	}
	killAllProcesses(killProc,returnVal);
	return 0;
}

void killAllProcesses(char** killProc,int exitValue)
{
	execvp(killProc[0],killProc);
	_exit(exitValue);
}


char *mygets(char *buf, int len)
{
	char *retval;

	retval = fgets(buf, len, stdin);
	buf[len] = '\0';
	if (buf[strlen(buf) - 1] == 10) /* trim \r */
		buf[strlen(buf) - 1] = '\0';
	else if (retval) 
		while (getchar() != '\n'); /* get to eol */

	return retval;
}

int mygeti()
{
	int ch;
	int retval=0;

	while(isspace(ch=getchar()));
	while(isdigit(ch))
	{
		retval = retval * 10 + ch - '0';
		ch = getchar();
	}
	while (ch != '\n')
		ch = getchar();
	return retval;
}