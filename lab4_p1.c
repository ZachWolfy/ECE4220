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
struct timeval x;
int numb1;

void *ReadBPE()
{
	//open pipe "/tmp/BP_pipe"
	int np = open("/tmp/BP_pipe", O_RDONLY);
	int bpread;
	int numb2;
	struct timeval x1, x2, xbp;
	double y1, y2, ybp;
	

	while(1)
	{
		//read from "/tmp/BP_pipe"
		bpread = read(np, &xbp, sizeof(struct timeval));
		
		//get previous gps
		y1 = buffer[0];
		x1 = x;
		
		//wait until global buffer is updated.
		numb2 = numb1;
		while(numb2 == numb1){}
		
		//when global is updated
		y2 = buffer[0];
		x2 = x;
		
		//the difference of struct timeval sec and usec that is converted to milli sec for second gps data timestamp, x2 and first gps data timestamp, x1.
		int millisec = (x2.tv_sec - x1.tv_sec) * 1000 + ((int)x2.tv_usec - (int)x1.tv_usec)/1000;
		
		//the difference of struct timeval second and microsecond that is converted to milli sec for button press timestamp, xbp and first gps data timestamp, x1.
		int millisec2 = (xbp.tv_sec - x1.tv_sec) * 1000 + ((int)xbp.tv_usec - (int)x1.tv_usec)/1000;
		
		//interpolation
		ybp = (((y2-y1)/(millisec))*(millisec2))+y1;
		
		//print
		printf("x1: %d:%d, y1: %d\n", x1, y1);
		printf("x2: %d:%d, y2: %d\n", x2, y2);
		printf("xbp: %d:%d, ybp: %d\n", xbp, ybp);
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
		numb1 = read(np, &buffer[0], sizeof(int));
		
		//get the time stamp and save in global buffer
		gettimeofday(&x, NULL);
	}
}

