#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval, int *pval);
int isprime(int n);
void* findPrime(void* argt);

int lval = 1; //lower value
int uval = 100; //upper value
int nval=10;
int tval=4;
char *flagarr = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int num;
int count = 0;

int main (int argc, char **argv)
{
  // Parse arguments
  parseargs(argv, argc, &lval, &uval,&nval,&tval);
  if(nval<0)nval=10;
  if(tval<=0)tval=4;
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
  pthread_t threadIDs[tval];
  flagarr=(char*)malloc(sizeof(char)*(uval-lval+1));// Allocate flags
  if(flagarr==NULL)exit(1);
  if(pthread_mutex_init(&lock,NULL)<0)return 1;//Initializing mutex lock 
  num=lval;
  /********************************************************************
  *Creating the threads and executing them to target function findPrime
  ********************************************************************/
  for(int i=0; i<tval;i++)
  {
    pthread_create(&threadIDs[i],NULL,findPrime,NULL);
  }
  /********************************************************
  *Waiting for each thread to finish its task and join main
  ********************************************************/
  for (int i = 0; i < tval; i++)
  {
    pthread_join(threadIDs[i],NULL);
  }
   pthread_mutex_destroy(&lock);//Destroying the lock
  //Print results
  printf("Found %d primes%c\n", count, count ? ':' : '.');
  int current=1;
  count=fmin(count,nval);
  for (num = lval; num <= uval; num++)
  {
    if (flagarr[num - lval]&& current<=nval)
    {
    count--;
    current++;
    printf("%d%c", num, (current==nval || count)? ',':'\n'); 
    }
  }
  pthread_exit(NULL);//Safty check to check if there is any running thread
  free(flagarr);
  return 0;
}

void* findPrime(void* argt)
{
  while (num<=uval)
  {
    pthread_mutex_lock(&lock);//Lock on critical section 
    int check=num++;
    pthread_mutex_unlock(&lock);//Unlocking the lock
    if (isprime(check)&&check<=uval)
    {
      flagarr[check - lval] = 1;
      count ++; 
    } else {
      flagarr[check - lval] = 0;
    }  
  }
  pthread_cond_broadcast(&condition);//Realsing all threads from their lock
  return NULL;
}

void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval,int *tval)
{
  int ch;

  opterr = 0;
  while ((ch = getopt (argc, argv, "l:u:n:t:")) != -1)
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
      case 't':
        *tval=atoi(optarg);
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
  {
    if (primes[i]&&(n % primes[i]) == 0)
      return 0;
  }
    
  return 1;
}