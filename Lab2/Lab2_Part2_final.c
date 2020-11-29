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

//struct timespec
struct read_data{
	char *file_name;
};

char buffer[300];

void *Read(void *ptr)
{
	struct timespec next_period;

	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	
	struct read_data *filename = (struct read_data*)ptr;

	next_period.tv_sec = 1;
	
	clock_gettime(CLOCK_MONOTONIC, &next_period);
	
	FILE *read_file = fopen((*filename).file_name, "r");
	
	while(fgets(buffer, BUFFER_MAX, read_file) != NULL)
	{
		puts(buffer);
		
		next_period.tv_sec += 2;
		//next_period.tv_nsec += 2000000;

        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, NULL);
        	
	}
	fclose(read_file);
	return NULL;
}

void *Write(void *ptr)
{
	struct timespec next_period;

	char *write;
	
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);

	next_period.tv_sec = 1;
	
	clock_gettime(CLOCK_MONOTONIC, &next_period);

	for(int i = 0; i < 20; i++)
	{
		write = buffer;

		printf("%s", buffer);
		
		next_period.tv_sec += 1;
		//per_write.next_period.tv_nsec += 1000000; //prints duplicates
		//next_period.tv_nsec += 100000000; //prints fine

        	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, NULL);
	}
	return NULL;
}

int main()
{
	struct read_data *file = malloc(sizeof(struct read_data)*3);
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
