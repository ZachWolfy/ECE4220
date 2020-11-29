#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#define BASE_ADD	0x3F200000 //base address to gpfsel
#define RED		8 //red led pin based on wiringpi
#define YELLOW		9 //yellow led pin based on wiringpi
#define GREEN		7 //green led pin based on wiringpi

#define VALUE 		0b10000000000000000 //bcm 16 button
//main
int main(void)
{
	//open /dev/mem to access GPIO in the user space
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1) // if fail print and exit
	{
		printf("unable to open /dev/mem\n");
		exit(fd);
	}
	//get pointer to point to GPFSEL0
	unsigned long *GPFSEL0 = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BASE_ADD);
	//get pointer to point to GPEDS to check for button pressed by user
	unsigned long *GPEDS0 = GPFSEL0 + (0x40)/4;

	//print address to check and to check the button and GPEDS value
	printf("%lu\n", GPFSEL0);
	printf("%lu\n", GPFSEL0+1);
	printf("%lu\n", GPEDS0);
	printf("gpeds : %x\n", *GPEDS0);
	printf("%x\n", VALUE);

	//setup wiringpi setup and set LEDS pin mode to output
	wiringPiSetup();
	pinMode(RED,OUTPUT);
	pinMode(YELLOW,OUTPUT);
	pinMode(GREEN,OUTPUT);
	
	//infinite loop
	while(1)
	{
		//turn on red LED, delay and turn off red LED
		digitalWrite(RED, HIGH); delay(500); digitalWrite(RED, LOW);
		//turn on yellow LED, delay and turn off yellow LED
		digitalWrite(YELLOW, HIGH); delay(500); digitalWrite(YELLOW, LOW);
		//check if the BCM 16 button is pushed
		if(*GPEDS0 == 0b10000000000000000)
		{
			//turn on green LED, delay and turn off green LED
			digitalWrite(GREEN, HIGH); delay(500); digitalWrite(GREEN, LOW);
			//clear bits
			*GPEDS0 = VALUE;
		}
		
	}
	
	return 0;
}
