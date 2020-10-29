#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

struct print()
{
	struct timeval x1, x2, xbp;
	int y1, y2, ybp;
}

char buffer[2];
int y;
struct timeval x;
int numb1;

void *ReadBPE()
{
	//open pipe "/tmp/BP_pipe"
	int np = open("/tmp/BP_pipe", O_RDONLY);
	int bpread;

	
	struct timeval button_buffer;

	while(1)
	{
		//read from "/tmp/BP_pipe"
		bpread = read(np, &button_buffer, sizeof(struct timeval));
		pthread_t child;
		pthread_create(&child, NULL, ChildThread, &button_buffer);
	}
}
void *ChildThread()
{
	int numb2;
	int x1, x2, xbp, y1, y2. ybp;
	numb2 = numb1;
	//get previous gps
	y1 = y;
	x1 = x;

	//wait until global buffer is updated.
	while(numb2 == numb1){}

	//when global is updated
	y2 = y;
	x2 = x;

	//interpolation
	ybp = (((y2-y1)/(x2-x1))*(xbp-x1))+y1;
}

void *PrintFunction()
{
	struct print data;
	
	int pd = open("/tmp/Print_pipe", O_RDONLY);
	read(pd, &data, size of(struct print));
	
	sem_wait(&mySem);
	printf("xbp: %d, ybp: %d", xbp, ybp);
	printf("x1: %d, y1: %d", x1, y1);
	printf("x2: %d, y2: %d", x2, y2);
	sem_post(&mySem);
}

int main()
{
	//open pipe for gps device
	int np = open("/tmp/N_pipe1", O_RDONLY);
  //open pipe for print pipe
  int pp = open("/tmp/P_pipe", O_RDONLY);
  
	//create thread read bpe
	pthread_t readbpe;
	pthread_create(&readbpe, NULL, ReadBPE, NULL);

	//while loop
	while(1)
	{
		//read from pipe 
		numb1 = read(np, buffer, sizeof(int));
		
		//get the time stamp and save in global buffer
		y = buffer[0];
		gettimeofday(&x, NULL);
	}
}
