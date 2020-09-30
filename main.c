#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

struct Info{
    char *p;
};

void *Read(void *ptr)
{
    struct Info *struct_item = (struct Info*)ptr;

    FILE *read_file = fopen(struct_item->p, "r");
    
    while(read_file != EOF)
    {
       char data[300];
        wait(10);
    }
}

void *Write()
{
    for(int i = 0; i < 20; i++)
    {
        printf("%s", &data);
        wait(5);
    }
}

int main()
{
    struct Info *info = malloc(sizeof(struct Info)*2);
    info[0].p = "first.txt";
    info[1].p = "second.txt";

    pthread_t thread[3];
    pthread_create(&thread[0], NULL, Read, &info[0]);
    pthread_create(&thread[0], NULL, Read, &info[1]);
    pthread_create(&thread[0], NULL, Write, NULL);

    for(int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }
    return 0;
}
