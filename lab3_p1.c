#include <stdio.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#define BASE_ADD	0x3F200000
#define RED		8
#define YELLOW		9
#define GREEN		7

#define VALUE 0b00000000000000001000000000000000
#define Value 1<<16
int main(void)
{
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	if(fd == -1)
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

	wiringPiSetup();
	pinMode(RED,OUTPUT);
	pinMode(YELLOW,OUTPUT);
	pinMode(GREEN,OUTPUT);
	
	while(1)
	{
		digitalWrite(RED, HIGH); delay(500); digitalWrite(RED, LOW);
		digitalWrite(YELLOW, HIGH); delay(500); digitalWrite(YELLOW, LOW);
		if(*GPEDS0 == VALUE)
		{
			printf("%x\n", *GPEDS0);
			digitalWrite(GREEN, HIGH); delay(500); digitalWrite(GREEN, LOW);
			//clear bits
			*GPEDS0 = VALUE;
		}
		
	}
	
	return 0;
}
