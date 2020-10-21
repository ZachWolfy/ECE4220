#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

char buffer[2];

int main()
{
	//open pipe
	int np = open("/tmp/N_pipe1", O_RDONLY);
	//create thread read bpe
	pthread readbpe;
	pthread_create(&readbpe, NULL, ReadBPE, NULL);
	//while loop
	while(1)
	{
		//read from pipe
		numb = read(np, int, sizeof(int));
		//get the time stamp and save in global buffer
		
	}
}

void *ReadBPE()
{
	//open pipe "/tmp/BP_pipe"
	int bp = open("/tmp/BP_pipe", O_RDONLY);
	int read;
	while(1)
	{
		//read from "/tmp/BP_pipe"
		read = read(bp, int, sizeof(int));
		//wait until global buffer is updated.
		
		//interpolation
		
		printf();
	}
}
