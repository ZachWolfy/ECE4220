#define _POSIX_C_SOURCE 199309L

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
#include <time.h>

#define BASE_ADD	0x3F200000		//GPIO base address of GPFSEL0
#define VALUE		0b10000000000000000	//BCM16 button

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
	unsigned long *GPEDS0 = GPFSEL0 + 0x40/4;
	
	struct timeval curr_time;

	//create a pipe /tmp/BP_pipe
	mkfifo("/tmp/BP_pipe", 777);
	int numb;

	//open pipe
	int bp = open("/tmp/BP_pipe", O_WRONLY);

	//set scheduler
	struct sched_param param = { .sched_priority = 50};
	sched_setscheduler(0, SCHED_FIFO, &param);
	
	//get the current time
	struct timespec period;
	//int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	clock_gettime(CLOCK_MONOTONIC, &period);
	
	//set the period
	period.tv_nsec = 60000000;
	
	while(1)
	{
		//if button is pressed
		if(*GPEDS0 == VALUE)
		{
			// get or detect time
			gettimeofday(&curr_time, NULL);
			
			//write onto pipe "/tmp/BP_pipe"
			numb = write(bp, &curr_time, sizeof(struct timeval));
			
			//clear gpeds
			*GPEDS0 = VALUE;
		}
		//wait until next period
		period.tv_nsec += 60000000;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, NULL);
	}
	
	return 0;
}
