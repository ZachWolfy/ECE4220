EXECUTABLE := server_udp_broadcast \
		client_udp_broadcast \

CODES := server_udp_broadcast.c \
		client_udp_broadcast.c \

.PHONY : all

.PHONY : clean

all: 
	make $(EXECUTABLE)
	
clean :
	rm -f $(EXECUTABLE)
		
server_udp_broadcast: server_udp_broadcast.c
	gcc -std=c11 $< -o $@

client_udp_broadcast: client_udp_broadcast.c
	gcc -std=c11 $< -o $@

