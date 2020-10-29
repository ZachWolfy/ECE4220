#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

struct print
{
	struct timeval x1, x2, xbp;
	int y1, y2, ybp;
};

char buffer[2];
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
void *ChildThread(void *ptr)
{
	int numb2;
	numb2 = numb1;
	
	struct print data;
	struct data.xbp = (struct timeval*)ptr;
	
	//get previous gps
	data.y1 = buffer[0];
	data.x1 = x;

	//wait until global buffer is updated.
	while(numb2 == numb1){}

	//when global is updated
	data.y2 = y;
	data.x2 = x;

	//interpolation
	data.ybp = (((data.y2-data.y1)/(data.x2-data.x1))*(data.xbp-data.x1))+data.y1;
}

void *PrintFunction()
{
	struct print data;
	
	int pd = open("/tmp/Print_pipe", O_RDONLY);
	read(pd, &data, size of(struct print));
	
	sem_wait(&mySem);
	printf("xbp: %d, ybp: %d", data.xbp, data.ybp);
	printf("x1: %d, y1: %d", data.x1, data.y1);
	printf("x2: %d, y2: %d", data.x2, data.y2);
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
