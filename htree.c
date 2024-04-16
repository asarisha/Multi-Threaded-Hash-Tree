
#include <stdio.h>	 
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h> 	// for EINTR
#include <fcntl.h>	 
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/mman.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "common_threads.h"
#include "common.h"

// Print out the usage of the program and exit.
void Usage(char*);
uint32_t jenkins_one_at_a_time_hash(const uint8_t* , uint64_t );
void* htree(void *arg);

// declare a struct that represents a thread
struct Thread {
    uint32_t nblocks;
    uint64_t threadNum;
    uint64_t nThreads;
  uint32_t hashVal;
};

// block size
#define BSIZE 4096

char* map;

int
main(int argc, char** argv)
{
  int32_t fd;
  uint32_t nblocks;

  // input checking
  if (argc != 3)
	Usage(argv[0]);

  // open input file
  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
	perror("open failed");
	exit(EXIT_FAILURE);
  }
 
  // use fstat to get file size
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
	perror("stat failed");
	exit(EXIT_FAILURE);
  }
  size_t fsize = sb.st_size;
 
  // calculate nblocks
  nblocks = fsize/BSIZE;
  printf(" no. of blocks = %u \n", nblocks);

  double start = GetTime();

  // calculate hash value of the input file
 
  map = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
  pthread_t root;
  struct Thread rootThread = {nblocks, 0, atoi(argv[2])}; // constructing thread 0
  Pthread_create(&root, NULL, htree, &rootThread);
  Pthread_join(root, NULL);
  uint32_t hash = rootThread.hashVal;
 
  double end = GetTime();
  printf("hash value = %u \n", hash);
  printf("time taken = %f \n", (end - start));
  close(fd);
  return EXIT_SUCCESS;
}

uint32_t
jenkins_one_at_a_time_hash(const uint8_t* key, uint64_t length)
{
  uint64_t i = 0;
  uint32_t hash = 0;

  while (i != length) {
	hash += key[i++];
	hash += hash << 10;
	hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}


void
Usage(char* s)
{
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}

void* htree(void *arg) {
  struct Thread *thread = (struct Thread *)arg; 
  
  uint64_t threadNum = thread->threadNum;
  uint32_t nblocks = thread->nblocks;
  uint64_t nThreads = thread->nThreads;  
 
  // calculate the hash value of the thread's assigned nodes
  uint64_t start = threadNum*(nblocks/nThreads);
  uint32_t blocksPerThread = nblocks/nThreads;
  uint8_t subarray[blocksPerThread * sizeof(uint8_t)];
  memcpy(subarray, map + start, blocksPerThread);
  thread->hashVal = jenkins_one_at_a_time_hash(subarray, blocksPerThread);
  // if thread is a leaf node, return the hash value
  if ((2*threadNum)+1 >= nThreads) { 
	pthread_exit(thread);
  }
  
  pthread_t p1;
  pthread_t p2;
 
  // create left child
  struct Thread leftThread = {nblocks, (2*threadNum)+1, nThreads};
  // create a left child with an appropriate thread number
  Pthread_create(&p1, NULL, htree, &leftThread);
  Pthread_join(p1, NULL); 
  
 
  //store the hash value as a string in curr_hv_str
  char curr_hv_str[3*(sizeof(uint32_t) + 1)];
  sprintf(curr_hv_str, "%u", thread->hashVal);
 /* concatenate left thread hash value to end of current thread hash
   * value string, or curr_hv_str
	*/
  char left_hv_str[sizeof(uint32_t) + 1];
  sprintf(left_hv_str, "%u", leftThread.hashVal);
  strcat(curr_hv_str, left_hv_str);
 
 
  // if thread has right child, then create rightThread
  if ((2*(threadNum)+2) < nThreads) {
	struct Thread rightThread = {nblocks, (2*threadNum)+2, nThreads}; 
	Pthread_create(&p2, NULL, htree, &rightThread);
	Pthread_join(p2, NULL);
 
  /*concatenate the right thread hash value to the end of the
  *current concatenated string
  */
	char right_hv_str[sizeof(uint32_t) + 1];
	sprintf(right_hv_str, "%u", rightThread.hashVal);
	strcat(curr_hv_str, right_hv_str);
  }
  // replace current thread hash value with that of the concatenated string
  thread->hashVal = jenkins_one_at_a_time_hash((uint8_t*)curr_hv_str, strlen(curr_hv_str));
  return NULL;
}
