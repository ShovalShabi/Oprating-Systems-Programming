#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <semaphore.h>


void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval,int *tval);
void* lookForPrimes(void *ptr);
int isprime(int n);

int lval = 1;
int uval = 100;
int nval = 10;
int tval = 4;
int num;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_malloc = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_writers, sem_readers,sem_inner_writers;
int countReaders = 0, countWriters = 0;
char *flagarr = NULL;
int *primes = NULL;
int count = 0;


int main (int argc, char **argv)
{
  int i;
  
  int now = 1;

  //Parse user options
  parseargs(argv, argc, &lval, &uval,&nval, &tval);
  pthread_t* threads =(pthread_t*)malloc(tval*sizeof(pthread_t));
  if(!threads)exit(1);
  if (uval < lval)
  {
    fprintf(stderr, "Upper bound should not be smaller then lower bound\n");
    exit(1);
  }    
  if (lval < 2)
  {
    lval = 2;
    uval = (uval > 1) ? uval : 1;
  }
  flagarr = (char *)malloc(sizeof(char) * (uval-lval+1));
  if (flagarr == NULL)exit(1);
  /******************************
   *Initializing semaphores locks
   * ***************************/
  sem_init(&sem_writers,0,1);
  sem_init(&sem_readers,0,1);
  sem_init(&sem_inner_writers,0,1);
  /******************************
  *Initializing mutex locks locks
  * ****************************/
  if(pthread_mutex_init(&lock,NULL) != 0)exit(1);
  if(pthread_mutex_init(&lock_malloc,NULL) != 0)exit(1);
  num = lval-1;
  /*****************************************
  *Creating threads and executing them mutex
  ******************************************/
  for (i = 0; i < tval; i++)
  {
    int state = pthread_create(&threads[i],NULL,lookForPrimes,NULL);   
    if (state)exit(1);
  } 
  /**********************************
  *Terminating threads by join method
  **********************************/
  for (i = 0; i < tval; i++)pthread_join(threads[i],NULL);

  // Print results
  printf("Found %d primes%c\n", count, count ? ':' : '.');
  count = fmin(count,nval);
  for (num = lval; num <= uval; num++)
  {
    if (flagarr[num - lval] && nval!=0)
    {   
      count--;         
      nval--;
      now++;
      printf("%d%c", num, (now == nval || count) ? ',' : '\n');  
    }
  }
  /********************
  *Cleaning environment
  ********************/ 
  pthread_exit(NULL);
  pthread_mutex_destroy(&lock);
  pthread_mutex_destroy(&lock_malloc);
  sem_destroy(&sem_readers);
  sem_destroy(&sem_writers);
  sem_destroy(&sem_inner_writers);
  free(flagarr);
  free(primes);
  free(threads);
  return 0;
}


void parseargs(char *argv[], int argc, int *lval, int *uval, int *nval, int *tval)
{
  int ch;
  opterr = 0;
  while ((ch = getopt(argc, argv,"l:u:n:t:")) != -1)
    switch (ch)
    {
      case 'l':  // Lower bound flag
        *lval = atoi(optarg);
        break;
      case 'u':  // Upper bound flag 
        *uval = atoi(optarg);
        break;
      case 'n': // maximum numbers to print
        *nval = atoi(optarg);
        break;
      case 't': // maximum threads to execute
        *tval = atoi(optarg);
        break;
      case '?':
        if ((optopt == 'l') || (optopt == 'u') || (optopt == 'n') || (optopt == 't'))
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
        break;
      default:
        exit(1);
    }    
}

void* lookForPrimes(void *ptr)
{
  while(num < uval)
  {
    if(num < uval){
      pthread_mutex_lock(&lock);
      int check=num++;
      pthread_mutex_unlock(&lock);
      if (isprime(check)&&check <= uval)
      {
        flagarr[check - lval] = 1;
        pthread_mutex_lock(&lock);        
        count ++; 
        pthread_mutex_unlock(&lock);   
      } 
      else 
      {
        flagarr[check - lval] = 0; 
      } 
    }    
  }
  while(1)
  {
    if(!sem_post(&sem_writers)&&!sem_post(&sem_readers)&&!sem_post(&sem_inner_writers))break;
  }
  return NULL;
}

int isprime(int n)
{
  static int size = 0;		// NOTE: static !
  static int maxprime;		// NOTE: static !
  int root;
  int i;


  if(primes==NULL)pthread_mutex_lock(&lock_malloc);
  if (primes == NULL)
  {
    primes = (int *)malloc(2*sizeof(int));
    if (primes == NULL)
      exit(1);
    size = 2;
    primes[0] = 2;
    primes[1] = 3;
    maxprime = 3;
    pthread_mutex_unlock(&lock_malloc);
  }
  

  root = (int)(sqrt((double)n));

  // Update primes array, if needed
  while (root > maxprime)
      for (i = maxprime + 2 ;  ; i+=2)
        if (isprime(i) && i>maxprime)
        {        
          /**********************
          *Enlarging primes array
          *Writers section
          ***********************/
          while (countReaders > 0 && countWriters)sem_wait(&sem_readers);
          while(countWriters>0)sem_wait(&sem_inner_writers);
          if(i>maxprime)
          {
            countWriters++;
            size++;
            primes = (int *)realloc(primes, size * sizeof(int));
            if (primes == NULL)
              exit(1);     
            primes[size-1] = i;
            maxprime = i; 
            countWriters--;
            if (!countWriters)sem_post(&sem_writers);
            else sem_post(&sem_inner_writers);
          } 
          break;
        } 

  // Check 'special' cases
  if (n <= 0)
    return -1;
  if (n == 1)
    return 0;
  /*********************
  *Checking prime number
  *Readers section
  *********************/
  if (countWriters > 0)sem_wait(&sem_writers);
  countReaders++;
  for (i = 0 ; ((i < size) && (root >= primes[i])) ; i++)
  {
    if (primes[i] && (n % primes[i]) == 0)
    {
      countReaders--;
      if (countReaders == 0)sem_post(&sem_writers);
      return 0;
    }
  }
  countReaders--;
  if (countReaders == 0)sem_post(&sem_writers);
  return 1;
}
