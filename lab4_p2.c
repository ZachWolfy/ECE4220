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

#define BASE_ADD	0x3F200000
#define VALUE		0b000000000000000010000000000000000

int main(void)
{
	//open file /dev/mem
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)
	{
		printf("unable to open /dev/mem\n");
		exit(fd);
	}
	//mmap GPFSEL0 and offset to GPEDS0
	unsigned long *GPFSEL0 = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BASE_ADD);
	unsigned long *GPEDS0 = GPFSEL0 + 0x10;

	//create a pipe /tmp/BP_pipe
	mkfifo("/tmp/BP_pipe", 777);
	//open pipe
	int bp = open("/tmp/BP_pipe", O_RDWR);
	
	//set scheduler
	struct sched_param param = {.schedule_priority = 50};
	sched_setschedular(0, SCHED_FIFO, &param);
	
	//get the current time
	clock_gettime(CLOCK_MONOTONIC, next_period);
	
	//set the period
	struct timespec period.tv_nsec = 60000000;
	
	while(1)
	{
		//if button is pressed
		if(*GPEDS0 == VALUE)
		{
			// get or detect time
			gettimeofday(&curr_time, NULL);
			//write onto pipe "/tmp/BP_pipe"
			int numb = write(bp, int, sizeof(int));
			//clear GPEDS0 register
			*GPEDS0 = VALUE;
		}
		//wait until next period
		period += period;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, NULL);
	}
	
	return 0;
}
