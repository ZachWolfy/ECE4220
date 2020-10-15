#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphores.h>
#includue <sched.h>

#define BASE_ADD	0x3F200000
#define RED			8
#define YELLOW		9
#define GREEN		7
#define VALUE 0b00000000000011111000000000000000

struct data{
	int led;
	struct sched_param param;
}

sem_t mySem;

void *LED_Thread(void *ptr)
{
	struct data *led = (struct data*)ptr;
	
	sched_setschedular(0, SCHED_FIFO, led.param);
	
	while(1)
	{
		sem_wait(&mySem);
		digitalWrite(led.led, HIGH); delay(500); digitalWrite(led.led, LOW);
		sem_post(&mySem);
		usleep(10);
	}
}

void *Ped_Thread(void *ptr)
{
	struct data *led = (struct data*)ptr;
	
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)
	{
		printf("unable to open /dev/mem\n");
		exit(fd);
	}
	unsigned long *GPFSEL0 = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BASE_ADD);
	unsigned long *GPEDS0 = GPFSEL0 + 0x10;
	
	sched_setschedular(0, SCHED_FIFO, led.param);
	
	while(1)
	{
		if(*GPEDS0 == VALUE)
		{
			sem_wait(&mySem);
			digitalWrite(led.led, HIGH); delay(500); digitalWrite(led.led, LOW);
			*GPEDS0 = VALUE;
			sem_post(&mySem);
			usleep(10);
		}
	}
}

int main(void)
{
	struct sched_param param = 50;
	
	if(sched_setschedular(0, SCHED_FIFO, &param) == -1)
	{
		printf("Error setting up sched_setschedular\n");
	}
	
	sem_init(&mySem,0,1);

	printf("%lu\n", GPFSEL0);
	printf("%lu\n", GPFSEL0+1);
	printf("%lu\n", GPEDS0);
	printf("gpeds : %x\n", *GPEDS0);
	printf("%x\n", VALUE);

	wiringPiSetup();
	pinMode(RED,OUTPUT);
	pinMode(YELLOW,OUTPUT);
	pinMode(GREEN,OUTPUT);
	
	struct data *p = malloc(sizeof(struct data)*3);
	p[0].led = RED;
	p[0].param = 50;
	p[1].led = YELLOW;
	p[1].param = 50;
	p[2].led = GREEN;
	p[2].param = 50;
	
	pthread_t thread[3];
	
	pthread_create(&thread[0], NULL, LED_Thread, &p[0]);
	pthread_create(&thread[1], NULL, LED_Thread, &p[1]);
	pthread_create(&thread[2], NULL, Ped_Thread, &p[2]);
	
	for(int i = 0; i < 3; i++)
	{
		pthread_join(thread[i]);
	}
	
	return 0;
}






