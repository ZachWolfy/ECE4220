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
};

struct read_data{
	char *file_name;
};

char buffer[300];

void *Read(void *ptr)
{
	struct period_info per_read;
	
	struct read_data *filename = (struct read_data*)ptr;
	
	per_read.period_ns = 1000000;
	
	clock_gettime(CLOCK_MONOTONIC, &(per_read.next_period));
	
	FILE *read_file = fopen((*filename).file_name, "r");
	
	while(fgets(buffer, BUFFER_MAX, read_file) != NULL)
	{
		puts(buffer);
		
		per_read.next_period.tv_nsec += 2000000;
		
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_read.next_period, NULL);
        	
	}
	fclose(read_file);
	return NULL;
}

void *Write(void *ptr)
{
	struct period_info per_write;
	
	char *write;
	
	per_write.period_ns = 1000000;
	
	clock_gettime(CLOCK_MONOTONIC, &(per_write.next_period));

	for(int i = 0; i < 20; i++)
	{
		write = buffer;
		printf("%s", buffer);
		
		per_write.next_period.tv_nsec += 1000000; //prints duplicates
		//per_write.next_period.tv_nsec += 100000000; //prints fine
       	
        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &per_write.next_period, NULL);
	}
	return NULL;
}

int main()
{
	struct read_data *file = malloc(sizeof(struct read_data)*2);
	file[0].file_name = "first.txt";
	file[1].file_name = "second.txt";
	
	pthread_t thread[3];
        
	//create pthread
	pthread_create(&thread[0], NULL, Read, &file[0]);
	pthread_create(&thread[1], NULL, Read, &file[1]);
	pthread_create(&thread[2], NULL, Write, NULL);
	

	for(int i = 0; i < 3; i++)
	{
		pthread_join(thread[i], NULL);
	}
	
    return 0;
}
