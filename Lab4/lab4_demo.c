#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

char buffer[1];

int main()
{
	char buffer2[2];
	int fd = open("/tmp/N_pipe1", O_RDONLY);
	if(fd < -1)	
	{
		printf("unable to open /tmp/N_pipe1\n");
		return(fd);
	}
	
	while(1)
	{
		int numb = read(fd, buffer2, sizeof(char));
		buffer2[1] = '\0';
		//printf("%d\n", buffer[0]);
		printf("%s\n", buffer2);
	}
	return 0;
}
