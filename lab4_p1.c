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

int main()
{
	//open pipe
	int np = open("/tmp/N_pipe1", O_RDONLY);
	//create thread read bpe
	pthread readbpe;
	pthread_create(&readbpe, NULL, ReadBPE, NULL);
	pthread_join(readbpe, NULL);
	//while loop
	while(1)
	{
		//read from pipe and get the time stamp and save in global buffer
		numb = read(np, buffer[0], sizeof(int));
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
		int read1 = read;
		if(read1 == read)
		{
			usleep(10);
			read1 = read;
		}
		//interpolation
		m = (y2-y1)/(x2-x1);
		ybp = (m)(xbp-x1)+y1;
		printf();
	}
}
