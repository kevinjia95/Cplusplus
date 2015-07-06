#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/uio.h>

#define SVRPORT "55002" // the server port

#define MAXDATASIZE 1000

#define MAX_CLIENTS 1000     // how many pending connections queue will hold

typedef enum {
    IDLE,
    ANSWER,
    LISTEN,
    ENDING,
    CTEST,
    PLAY,
    FEND
} statetype;

void quitthread(char *msg, int fd) {
  printf(msg);
  fflush(stdout);
  close(fd);
  pthread_exit(NULL);
}

void *server_func(void*);

int main(int argc, char **argv)
{
  int sockfd, new_fd,yes=1;  
  struct sockaddr_in server,client;
  int sockaddr_len=sizeof(struct sockaddr_in); 
  
  extern char *optarg;
  extern int optind;
  int c;
  char *pname=SVRPORT;
  static char usage[] = "usage: %s [-p port]\n";
  
  server.sin_family=AF_INET;
  
  while ((c = getopt(argc, argv, "p:")) != -1)
    switch (c) {
    case 'p':
      pname = optarg;
      break;
    case '?':
      fprintf(stderr, usage, argv[0]);
      exit(1);
      break;
    }

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(pname));
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(&server.sin_zero,8);
  
  if ((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
    perror("Server: socket");
  }
  
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) { 
    perror("Server: setsockopt");
    exit(1);
  }
  if (bind(sockfd, (struct sockaddr *)&server, sockaddr_len) == -1) {
    close(sockfd);
    perror("Server: bind");
  }
  
  if (listen(sockfd, MAX_CLIENTS) == -1) {
    perror("Server: listen");
    exit(1);
  }
  
  printf("Server at %s:%s is ready.\n", inet_ntoa((server.sin_addr)), pname);
  fflush(stdout);
  while(1) {  
    
    new_fd = accept(sockfd, (struct sockaddr *)&client,(socklen_t *) &sockaddr_len);
    if (new_fd == -1) {
      perror("Server: accept");
      continue;
      
    }
    printf("Server connected to %s.\n", inet_ntoa((client.sin_addr)));
    fflush(stdout);
    
    //multithreaded for different client connections
    pthread_t server_thread;
    int *fdarg = (int *)malloc(sizeof(int));
    *fdarg = new_fd;

    if( pthread_create(&server_thread , NULL , server_func, fdarg) < 0)
      {
	perror("Server: could not create thread");
	return 1;
      }
    
  }
  
  return 0;
}

//////////////////////////////////////
// send a string ending in \n
//////////////////////////////////////
void mealysend(int sendsock, char *b, int bsize)
{
  char sendbuf[MAXDATASIZE + 3] = "";

  strncpy(sendbuf, b, MAXDATASIZE);
  strncat(sendbuf, "\n", MAXDATASIZE+2);

  if (send(sendsock,sendbuf,strlen(sendbuf), 0) == -1){
    perror("Server: send");
    exit(1);
  }
  printf("Server sent: %s",b);
  printf(" -- and it was %d bytes.\n",bsize);
  fflush(stdout);
}

//////////////////////////////////////
// receive up to the first \n
// (but omit the \n)
//	return 0 on EOF (socket closed)
//	return -1 on timeout
//////////////////////////////////////
int mealyrecvto(int recvsock, char *b, int bsize, int to)
{
  int num;
  int selectresult;
  fd_set myset;
  struct timeval tv;  // Time out

  int count = 0;
  char c = '\127';

  memset(b,0,MAXDATASIZE);
  
  while ((count < (bsize-2)) && (c != '\n') && (c != '\0')) {
    FD_ZERO(&myset);
    FD_SET(recvsock, &myset);
    tv.tv_sec = to;
    tv.tv_usec = 0;
    if ((to > 0) &&
	((selectresult = select(recvsock+1, &myset, NULL, NULL, &tv)) <= 0)) {
      // timeout happened (drop what you were waiting for -
      // if it was delimited by \n, it didn't come!
      return -1;
    }
    // got here because select returned >0, i.e., no timeout
    if ((num = recv(recvsock, &c, 1, 0)) == -1) {
      perror("Server: recv failed");
      exit(1);
    }
    if (num == 0) {
      // nothing left to read (socket closed)
      // no need to wait for a timeout; you're already done by now
      return 0;
    }
    b[count] = c;
    count++;
  }
  // at this point, either c is \n or bsize has been reached
  // so just add a string terminator
  if (count < bsize-1) {
    b[count-1] = '\0';
  } else {
    b[bsize-1] = '\0';
  }
  printf("Server sent :%s",b);
  printf(": and it was %d bytes.\n", (int)strlen(b));
  return strlen(b);
}

void printstate(statetype thisstate) {
  char idlename[] = "IDLE";
  char answername[] = "ANSWER";
  char listenname[] = "LISTEN";
  char endingname[] = "ENDING";
  char fendname[] = "FEND";
  char playname[] = "PLAY";
  char ctestname[] = "CTEST";
  char *thisname;

  switch(thisstate) {
  case IDLE:
    thisname = idlename;
    break;
  case ANSWER:
    thisname = answername;
    break;
  case LISTEN:
    thisname = listenname;
    break;
  case ENDING:
    thisname = endingname;
    break;
  case FEND:
    thisname = fendname;
    break;
  case PLAY:
    thisname = playname;
    break;
  case CTEST:
    thisname = ctestname;
    break;
  }
  printf("Server - STATE = %s\n", thisname);
  fflush(stdout);
}

void *server_func(void* new)
{
  char buf1[MAXDATASIZE] = "\0";
  int *fd = (int *)new;
  int new_fd = *fd;
  
  free(fd);

  /* things we send */
  char ahoy[MAXDATASIZE] = "AHOY";
  char later[MAXDATASIZE] = "LATER";
  char nope[MAXDATASIZE] = "NO";
  char message[MAXDATASIZE] = "MSG Drink your ovaltine.";
  char failure[MAXDATASIZE] = "FAIL";

  /* things we receive */
  char ring[MAXDATASIZE] = "RING";
  char hello[MAXDATASIZE] = "HELLO";
  char finished[MAXDATASIZE] = "FINISHED";
  char bye[MAXDATASIZE] = "BYE";
  char datastr[MAXDATASIZE] = "DATA";
  char magic[MAXDATASIZE] = "MAGIC";
  char get[MAXDATASIZE] = "GET";
  char quit[MAXDATASIZE] = "QUIT";
  char codenum[MAXDATASIZE] = "CODE";
  char noted[MAXDATASIZE] = "NOTED";

  statetype mystate = IDLE;

  int errorcount = 0;
#define ERRORMAX 20

  int codecount = 0;
#define CODEMAX 3
#define UNLOCK 7

  int testnum;

  while (1) {
    printstate(mystate); 
    if (mealyrecvto(new_fd, buf1, MAXDATASIZE, 20) < 1) {
      quitthread("Server quit - read timeout.\n", new_fd);
    };
    switch (mystate) {
    case IDLE:
      if (strncmp(buf1, ring, strlen(ring)) == 0) {
	printf("Server got RING.\n");
	if ((rand() % 10) < 5) {
	  // lose some of the rings!
	  printf("Server lost a RING.\n");
	  fflush(stdout);
	  break;
	}
	mealysend(new_fd, ahoy, strlen(ahoy));
	mystate = ANSWER;
      } else if (strncmp(buf1, magic, strlen(magic)) == 0) {
	quitthread("Server instance closing.\n", new_fd);
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case ANSWER:
      if (strncmp(buf1, hello, strlen(hello)) == 0) {
	printf("Server got HELLO.\n");
	fflush(stdout);
	mystate = LISTEN;
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case LISTEN:
      /* just look at DATA characters */
      if (strncmp(buf1, datastr, 4) == 0) {
	printf("Server DATA heard: %s\n", buf1);
	fflush(stdout);
	mystate = LISTEN; // not needed, but helps with clarity
      } else if (strncmp(buf1, get, strlen(get)) == 0) {
	printf("Server got GET.\n");
	fflush(stdout);
	mystate = CTEST;
      } else if (strncmp(buf1, finished, strlen(finished)) == 0) {
	printf("Server got FINISHED.\n");
	fflush(stdout);
	mealysend(new_fd, later, strlen(later));
	mystate = ENDING;
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case ENDING:
      if (strncmp(buf1, bye, strlen(bye)) == 0) {
	printf("Server got BYE.\n");
	fflush(stdout);
	mystate = IDLE;
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case CTEST:
      if (strncmp(buf1, codenum, 4) == 0) {
	printf("Server got CODE.\n");
	fflush(stdout);
	testnum = atoi(buf1 + 4);
	printf("Server saw number: %d\n", testnum);
	if (testnum == UNLOCK) {
	  printf("Server: unlock succeeded.\n");
	  fflush(stdout);
	  mealysend(new_fd, message, strlen(message));
	  mystate = PLAY;
	  break;
	} else 	if (codecount++ > CODEMAX) {
	  printf("Server: code count exceeded.\n");
	  fflush(stdout);
	  mealysend(new_fd, failure, strlen(failure));
	  mystate = FEND;
	  break;
	}
	// otherwise just stay here and say no
	printf("Server: unlock failed.\n");
	fflush(stdout);
	mealysend(new_fd, nope, strlen(nope));
	mystate = CTEST; // not needed, but helps with clarity
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case PLAY:
      if (strncmp(buf1, noted, strlen(noted)) == 0) {
	printf("Server got NOTED.\n");
	fflush(stdout);
	mystate = LISTEN;
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    case FEND:
      if (strncmp(buf1, quit, strlen(quit)) == 0) {
	printf("Server got QUIT.\n");
	fflush(stdout);
	mealysend(new_fd, later, strlen(later));
	mystate = ENDING;
      } else {
	printf("Server ERROR: MESSAGE NOT EXPECTED: %s",buf1);
	fflush(stdout);
	if (errorcount++ > ERRORMAX) {
	  quitthread("Server: error count exceeded. Halting.\n", new_fd);
	}
      }
      break;
    }
  }
}
