#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>

sem_t mySem;//semaphore
//full data struct to pass to print function
struct print
{
	struct timeval x1, x2, xbp;
	double y1, y2, ybp;
};

char buffer[2];//gps data buffer
struct timeval x;//timestamp data buffer

//print GPS data's and timestamp
void *PrintFunction(void *ptr)
{
	struct print data;
	printf("print thread");
	//open pipe to get print data
	int pd = open("/tmp/Print_pipe", O_RDONLY);
	read(pd, &data, sizeof(struct print));
	
	//lock and print
	sem_wait(&mySem);
	
	printf("xbp: %d:%d, ybp: %lf\n", data.xbp.tv_sec, data.xbp.tv_usec, data.ybp);
	printf("x1: %d:%d, y1: %lf\n", data.x1.tv_sec, data.x1.tv_usec, data.y1);
	printf("x2: %d:%d, y2: %lf\n\n\n", data.x2.tv_sec, data.x2.tv_usec, data.y2);
	
	sem_post(&mySem);//unlock when finish printing

	pthread_exit(NULL);
}
//thread to wait for buffer and get the interpolation
void *ChildThread(void *ptr)
{
	printf("child thread");
	struct print data;
	data.xbp = *(struct timeval*)ptr;//get button press timestamp
	
	//get previous gps
	data.y1 = buffer[0];
	data.x1 = x;

	//wait until global buffer is updated. if y1 != buffer means buffer has updated
	//and will exit the while loop
	printf("before wait");
	while(data.y1 == buffer[0]){}
	printf("after wait");
	//when global is updated
	data.y2 = buffer[0];
	data.x2 = x;

	//the difference of struct timeval sec and usec that is converted to milli sec for second gps data timestamp, x2 and first gps data timestamp, x1.
	int millisec = (data.x2.tv_sec - data.x1.tv_sec) * 1000 + ((int)data.x2.tv_usec - (int)data.x1.tv_usec)/1000;

	//the difference of struct timeval second and microsecond that is converted to milli sec for button press timestamp, xbp and first gps data timestamp, x1.
	int millisec2 = (data.xbp.tv_sec - data.x1.tv_sec) * 1000 + ((int)data.xbp.tv_usec - (int)data.x1.tv_usec)/1000;

	//interpolation
	data.ybp = (((data.y2-data.y1)/(millisec))*(millisec2))+data.y1;
	
	//open print pipe and create thread to the print pipe
	int pd = open("/tmp/Print_pipe", O_WRONLY);
	write(pd, &data, sizeof(struct print));
	close(pd);
	
	pthread_t printth;
	pthread_create(&printth, NULL, PrintFunction, NULL);
	

	pthread_exit(NULL);
}
//read the button timestamp pipe received
void *ReadBPE(void *pttr)
{
	//open pipe "/tmp/BP_pipe"
	int np = open("/tmp/BP_pipe", O_RDONLY);
	int bpread;
	struct timeval button_buffer;
	printf("readbpe");
	while(1)
	{
		//read from "/tmp/BP_pipe"
		bpread = read(np, &button_buffer, sizeof(struct timeval));
		pthread_t child;
		pthread_create(&child, NULL, ChildThread, &button_buffer);
	}
}
//main function to read gps data and get the timestamp of the gps data and store in global buffer
int main()
{
	//initialize semaphore
	sem_init(&mySem, 0, 1);
	
	//open pipe for gps device
	int np = open("/tmp/N_pipe1", O_RDONLY);
	
  	//make pipe for print pipe
  	mkfifo("/tmp/Print_pipe", 777);
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
