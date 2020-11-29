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

char buffer[2];//gps buffer
struct timeval x;//timestamp buffer

//read the button timestamp pipe received
//thread to wait for buffer and get the interpolation
//print GPS data's and timestamp
void *ReadBPE(void* ptr)
{
	//open pipe "/tmp/BP_pipe"
	int np = open("/tmp/BP_pipe", O_RDONLY);
	int bpread;
	struct timeval xbp;
	xbp.tv_sec = 0;
	xbp.tv_usec = 0;
	struct timeval x1, x2;
	double y1, y2, ybp;
	

	while(1)
	{
		//read from "/tmp/BP_pipe"
		bpread = read(np, &xbp, sizeof(struct timeval));
		
		//get previous gps
		y1 = buffer[0];
		x1 = x;
		
		//wait until global buffer is updated.
		while(y1 == buffer[0])
		{}
		
		//when global is updated
		y2 = buffer[0];
		x2 = x;
		
		//the difference of struct timeval sec and usec that is converted to milli sec for second gps data timestamp, x2 and first gps data timestamp, x1.
		int millisec = (x2.tv_sec - x1.tv_sec) + ((int)x2.tv_usec - (int)x1.tv_usec)/1000000;
		
		//the difference of struct timeval second and microsecond that is converted to milli sec for button press timestamp, xbp and first gps data timestamp, x1.
		int millisec2 = (xbp.tv_sec - x1.tv_sec)+ ((int)xbp.tv_usec - (int)x1.tv_usec)/1000000;
		
		//interpolation
		ybp = (((y2-y1)/(millisec))*(millisec2))+y1;
		
		//print
		printf("x1: %d:%d, y1: %lf\n", x1.tv_sec, x1.tv_usec, y1);
		printf("x2: %d:%d, y2: %lf\n", x2.tv_sec, x1.tv_usec, y2);
		printf("xbp: %d:%d, ybp: %lf\n\n\n", xbp.tv_sec, xbp.tv_usec, ybp);
	}
}
//main function to read gps data and get the timestamp of the gps data and store in global buffer
int main()
{
	//open pipe
	int np = open("/tmp/N_pipe1", O_RDONLY);
	int numb;
	
	//create thread read bpe
	pthread_t readbpe;
	pthread_create(&readbpe, NULL, ReadBPE, NULL);
	
	//while loop
	while(1)
	{
		//read from pipe 
		numb = read(np, buffer, sizeof(char));
		buffer[1] = '\0';
		
		//get the time stamp and save in global buffer
		gettimeofday(&x, NULL);
	}
}

