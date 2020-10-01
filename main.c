#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>

#define MY_PRIORITY 51  // kernel is priority 50
#define BUFFER_MAX 300

struct period_info {
        struct timespec next_period;
        long period_ns;
        char *p;
};

struct sched_param param;

char buffer[BUFFER_MAX];

//increment period
static void inc_period(struct period_info *pinfo) 
{
        pinfo->next_period.tv_nsec += pinfo->period_ns;
 
        while (pinfo->next_period.tv_nsec >= 1000000000) {
                /* timespec nsec overflow */
                pinfo->next_period.tv_sec++;
                pinfo->next_period.tv_nsec -= 10000000000;
        }
}

void *Read(void *ptr)
{
	struct period_info *per_read = (struct period_info*)ptr;
	
	per_read->period_ns = 1500000;
	clock_gettime(CLOCK_MONOTONIC, &(per_read->next_period));
	
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	
	FILE *read_file = fopen(per_read->p, "r");
	
	while(fgets(buffer, BUFFER_MAX, read_file) != NULL)
	{
		puts(buffer);
		
		inc_period(per_read);
       	/* for simplicity, ignoring possibilities of signal wakes */
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_read->next_period, NULL);
        	
	}
	//fclose(read_file);
	return NULL;
}

void *Write(void *ptr)
{
	struct period_info *per_read = (struct period_info*)ptr;
	
	per_read->period_ns = 1000000;
	clock_gettime(CLOCK_MONOTONIC, &(per_read->next_period));
	
	
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	char *write;
	
	for(int i = 0; i < 20; i++)
	{
		write = buffer;
		printf("%s", write);
		inc_period(per_read);
       	/* for simplicity, ignoring possibilities of signal wakes */
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_read->next_period, NULL);
	}
	return NULL;
}

int main()
{
	struct period_info *info = malloc(sizeof(struct period_info)*3);
	info[0].p = "first.txt";
	info[1].p = "second.txt";
	
	int ret;
	
	pthread_t thread[3];
	pthread_create(&thread[0], NULL, Read, &info[0]);
	pthread_create(&thread[0], NULL, Read, &info[1]);
	pthread_create(&thread[0], NULL, Write, &info[2]);

	for(int i = 0; i < 3; i++)
	{
		pthread_join(thread[i], NULL);
	}
	
    return 0;
}
