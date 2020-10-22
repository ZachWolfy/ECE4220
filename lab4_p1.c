#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

char buffer[2];
int y;
struct timeval x;
int numb1;

void *ReadBPE()
{
	//open pipe "/tmp/BP_pipe"
	int np = open("/tmp/BP_pipe", O_RDONLY);
	int bpread;
	int numb2;
	int x1, x2, xbp, y1, y2. ybp;
	
	struct timeval button_buffer;

	while(1)
	{
		//read from "/tmp/BP_pipe"
		bpread = read(np, &button_buffer, sizeof(struct timeval));
		
		//get previous gps
		y1 = y;
		x1 = x;

		//wait until global buffer is updated.
		numb2 = numb1;
		while(numb2 == numb1){}
		
		//when global is updated
		y2 = y;
		x2 = x;
		
		//interpolation
		ybp = (((y2-y1)/(x2-x1))*(xbp-x1))+y1;
		printf("xbp: %d, ybp: %d", xbp, ybp);
		printf("x1: %d, y1: %d", x1, y1);
		printf("x2: %d, y2: %d", x2, y2);
	}
}

int main()
{
	//open pipe
	int np = open("/tmp/N_pipe1", O_RDONLY);

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
