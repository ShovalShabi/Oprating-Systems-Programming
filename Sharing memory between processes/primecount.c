#define _DEFAULT_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/mman.h>


void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval, int *pval);
int isprime(int n); 

int main (int argc, char **argv)
{
  int lval = 1; //lower value
  int uval = 100; //upper value
  int nval=10; //maximum number to print
  int pval=4; //number of processes
  char *flagarr = NULL;
  int num;
  int count = 0;
  int index;
  parseargs(argv, argc, &lval, &uval,&nval,&pval);// Parse arguments
  if(nval<0)nval=10;
  if(pval<=0)pval=4;
  if (uval < lval)
  {
    fprintf(stderr, "Upper bound should not be smaller then lower bound\n");
    exit(1);
  }    
  if (lval < 2)
  {
    lval = 2;
    uval = (uval > 1) ? uval : 1; // if uvl bigger than 1 return uvl else return 1
  }
  // Allocate flags
  flagarr=mmap(NULL,sizeof(char)*(uval-lval+1),PROT_WRITE|PROT_READ,MAP_SHARED | MAP_ANONYMOUS,-1,0);//sharing flags for prime numbers
  if(flagarr==MAP_FAILED)
  {
    exit(1);
  }
  /*****************************************************************************************
  *Marked the pid as result of getpid() for the parent process, child processes will get a 0
  *****************************************************************************************/
  int pid=getpid();
  for (index = 0; index < pval; index++)
  {
    pid=fork();
    if (pid==0)break;
    else if(pid<0)exit(1);
  }
  /************************************************
  *Seting flagarr by offsets of the child processes
  ************************************************/
  if(pid==0)
  {
    for (num = lval+index; num <= uval; num+=pval)
    {
      if (isprime(num))
      {
        flagarr[num - lval] = 1;
        count ++; 
      } else {
        flagarr[num - lval] = 0; 
      }
    }
    return count;
  }
  
  if (pid>0)
  {
    int status=0;
    while(wait(&status)>0)//waiting to all child processes to exit and summing their exit status
    {
      count+=WEXITSTATUS(status);
    }
    //Print results
    printf("Found %d primes%c\n", count, count ? ':' : '.');
    int current=1;
    count=fmin(count,nval);
    for (num = lval; num <= uval; num++)
    if (flagarr[num - lval]&& current<=nval)
    {
      count--;
      current++;
      printf("%d%c", num, (current==nval || count)? ',':'\n'); 
      
    }
  }
  return 0;
}
/****************************************************
*Parsing the values that the user chose (parsing argv)
*****************************************************/
void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval,int *pval)
{
  int ch;

  opterr = 0;
  while ((ch = getopt (argc, argv, "l:u:n:p:")) != -1)
    switch (ch)
    {
      case 'l':  // Lower bound flag
        *lval = atoi(optarg);
        break;
      case 'u':  // Upper bound flag 
        *uval = atoi(optarg);
        break;
      case 'n': //max number to show
        *nval=atoi(optarg);
        break;
      case 'p': //number of processes
        *pval=atoi(optarg);
        break;
      case '?':
        if ((optopt == 'l') || (optopt == 'u') || (optopt == 'n') || (optopt == 'p'))
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
      default:
        exit(1);
    }    
}

int isprime(int n)
{
  static int *primes = NULL; 	// NOTE: static !
  static int size = 0;		// NOTE: static !
  static int maxprime;		// NOTE: static !
  int root;
  int i;

  // Init primes array (executed on first call)
  if (primes == NULL)
  {
    primes = (int *)malloc(2*sizeof(int));
    if (primes == NULL)
      exit(1);
    size = 2;
    primes[0] = 2;
    primes[1] = 3;
    maxprime = 3;
  }

  root = (int)(sqrt(n));

  // Update primes array, if needed
  while (root > maxprime)
    for (i = maxprime + 2 ;  ; i+=2)
      if (isprime(i))
      {
        size++;
        primes = (int *)realloc(primes, size * sizeof(int));
        if (primes == NULL)
          exit(1);
        primes[size-1] = i;
        maxprime = i;
        break;
      }

  // Check 'special' cases
  if (n <= 0)
    return -1;
  if (n == 1)
    return 0;

  // Check prime
  for (i = 0 ; ((i < size) && (root >= primes[i])) ; i++)
    if ((n % primes[i]) == 0)
      return 0;
  return 1;
}
