#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>

#define BASE_ADD	0x3F200000 		//base address to gpfsel
#define RED		8 			//red led pin based on wiringpi
#define YELLOW		9 			//yellow led pin based on wiringpi
#define GREEN		7 			//green led pin based on wiringpi
#define VALUE 		0b10000000000000000 	//bcm 16 button

//set semaphore as global variable
sem_t mySem;

//data argument passing of LED pin and priority of the task
struct data{
	int led;
	struct sched_param param;
};

//led thread for red and yellow LED
void *LED_Thread(void *ptr)
{
	//convert void ptr to struct daTA
	struct data *led = (struct data*)ptr;
	//set schedular with assigned priority and FIFO or RR scheduling
	sched_setscheduler(0, SCHED_FIFO, &led->param);
	//infinite loop
	while(1)
	{
		//lock thread
		sem_wait(&mySem);
		//critical section: turn on red LED, delay and turn off red LED
		digitalWrite(led->led, HIGH); delay(500); digitalWrite(led->led, LOW);
		//unlock thread
		sem_post(&mySem);
		//sleep
		usleep(10);
	}
}

void *Ped_Thread(void *ptr)
{
	//convert void ptr to struct daTA
	struct data *led = (struct data*)ptr;
	//open /dev/mem to access GPIO in the user space
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)// if fail print and exit
	{
		printf("unable to open /dev/mem\n");
		exit(fd);
	}
	unsigned long *GPFSEL0 = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BASE_ADD);
	unsigned long *GPEDS0 = GPFSEL0 + 0x10;

	printf("%lu\n", GPFSEL0);
	printf("%lu\n", GPFSEL0+1);
	printf("%lu\n", GPEDS0);
	printf("gpeds : %x\n", *GPEDS0);
	printf("%x\n", VALUE);
	
	sched_setscheduler(0, SCHED_FIFO, &led->param);
	
	while(1)
	{
		if(*GPEDS0 == VALUE)
		{
			sem_wait(&mySem);
			digitalWrite(led->led, HIGH); delay(500); digitalWrite(led->led, LOW);
			*GPEDS0 = VALUE;
			sem_post(&mySem);
			usleep(10);
		}
	}
}

int main(void)
{
	//set sechedular
	struct sched_param param =  { .sched_priority = 50 };
	
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
	{
		printf("Error setting up sched_setschedular\n");
	}
	//initialize semaphores
	sem_init(&mySem,0,1);

	//wiringpi setup and LED  output
	wiringPiSetup();
	pinMode(RED,OUTPUT);
	pinMode(YELLOW,OUTPUT);
	pinMode(GREEN,OUTPUT);

	//set data led and priority value
	struct data *p = malloc(sizeof(struct data)*3);
	struct sched_param param_th1 = { .sched_priority = 50};
	struct sched_param param_th2 = { .sched_priority = 50};
	struct sched_param param_pedth = { .sched_priority = 51};
	p[0].led = RED;
	p[0].param = param_th1;
	p[1].led = YELLOW;
	p[1].param = param_th2;
	p[2].led = GREEN;
	p[2].param = param_pedth;
	
	pthread_t thread[3];
	//create thread
	pthread_create(&thread[0], NULL, LED_Thread, &p[0]);
	pthread_create(&thread[1], NULL, LED_Thread, &p[1]);
	pthread_create(&thread[2], NULL, Ped_Thread, &p[2]);
	
	for(int i = 0; i < 3; i++)
	{
		pthread_join(thread[i], NULL);
	}
	
	return 0;
}


