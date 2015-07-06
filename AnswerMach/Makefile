all: ansmach-server ansmach-client

ansmach-client: ansmach-client.o
	${CC} -o ansmach-client -g ansmach-client.o 

ansmach-client.o: ansmach-client.c
	${CC} -g -c -Wall ansmach-client.c

ansmach-server: ansmach-server.o
	${CC} -o ansmach-server -g ansmach-server.o -lpthread

ansmach-server.o: ansmach-server.c
	${CC} -g -c -Wall ansmach-server.c -lpthread

clean:
	rm -f *.o *~ ansmach-client ansmach-server
