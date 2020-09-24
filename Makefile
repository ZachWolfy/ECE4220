EXECUTABLE := readfile \
		element_thread \
		row_thread \
		one_thread \

CODES := readfile.c \
		element_thread.c \
		row_thread.c \
		one_thread.c \

CFLAGS := -std=c11 -O0 -Wall -ggdb -pthread

.PHONY : all

.PHONY : clean

all: 
	make $(EXECUTABLE)
	
clean :
	rm -f $(EXECUTABLE)
		
readfile: readfile.c
	$(CC) $(CFLAGS) $< -o $@
	
element_thread: element_thread.c
	$(CC) $(CFLAGS) $< -o $@
	
row_thread: row_thread.c
	$(CC) $(CFLAGS) $< -o $@
	
one_thread: one_thread.c
	$(CC) $(CFLAGS) $< -o $@


	
