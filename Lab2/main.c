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

#define MY_PRIORITY 50  // kernel is priority 50
#define BUFFER_MAX 300

struct period_info {
        struct timespec next_period;
        long period_ns;
        char *p;
};

struct sched_param param;

char buffer[BUFFER_MAX];

void *Read(void *ptr)
{
	struct period_info *per_read = (struct period_info*)ptr;
	
	//param.sched_priority = MY_PRIORITY;
	//sched_setscheduler(0, SCHED_FIFO, &param);
	
	clock_gettime(CLOCK_MONOTONIC, &(per_read->next_period));
	FILE *read_file = fopen(per_read->p, "r");
	
	while(fgets(buffer, BUFFER_MAX, read_file) != NULL)
	{
		puts(buffer);
		
		per_read->next_period.tv_nsec += 400000;
       	/* for simplicity, ignoring possibilities of signal wakes */
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_read->next_period, NULL);
        	
	}
	//fclose(read_file);
	return NULL;
}

void *Write(void *ptr)
{
	struct period_info *per_write = (struct period_info*)ptr;
	
	//param.sched_priority = MY_PRIORITY;
	
	
	clock_gettime(CLOCK_MONOTONIC, &(per_write->next_period));

	char *write;
	
	for(int i = 0; i < 20; i++)
	{
		write = buffer;
		printf("%s", write);
		
		per_write->next_period.tv_nsec += 200000;
       	/* for simplicity, ignoring possibilities of signal wakes */
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_write->next_period, NULL);
	}
	return NULL;
}

int main()
{
	struct period_info *info = malloc(sizeof(struct period_info)*3);
	info[0].p = "first.txt";
	info[1].p = "second.txt";
	pthread_attr_t attr;
	pthread_t thread[3];
	int ret;
	
	info[0].period_ns = 100000;
	info[1].period_ns = 300000;
	info[1].period_ns = 200000;
        
        param.sched_priority = MY_PRIORITY;
        sched_setscheduler(0, SCHED_FIFO, &param);
        
	//create pthread
	ret = pthread_create(&thread[0], NULL, Read, &info[0]);
	if (ret) {
                printf("pthread create failed\n");
                return ret;
        }
	ret = pthread_create(&thread[1], NULL, Read, &info[1]);
	if (ret) {
                printf("pthread create failed\n");
                return ret;
        }
        
	ret = pthread_create(&thread[2], NULL, Write, &info[2]);
	if (ret) {
                printf("pthread create failed\n");
                return ret;
        }

	for(int i = 0; i < 3; i++)
	{
		ret = pthread_join(thread[i], NULL);
		if (ret) {
                	printf("pthread join failed\n");
                	return ret;
        	}
	}
	
    return 0;
}
