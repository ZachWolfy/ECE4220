EXECUTABLE := main \

CODES := main.c \

CFLAGS := -std=c11 -O0 -Wall -ggdb -pthread

.PHONY : all

.PHONY : clean

all: 
	make $(EXECUTABLE)
	
clean :
	rm -f $(EXECUTABLE)
		
main: main.c
	$(CC) $(CFLAGS) $< -o $@


	
