/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SVRPORT "55002"
#define SVRNAME "localhost"

#define MAXDATASIZE 1000

//////////////////////////////////////
// send a string ending in \n
//////////////////////////////////////
void mealysend(int sendsock, char *b, int bsize)
{
  char sendbuf[MAXDATASIZE + 3] = "";

  strncpy(sendbuf, b, MAXDATASIZE);
  strncat(sendbuf, "\n", MAXDATASIZE+2);

  if (send(sendsock,sendbuf,strlen(sendbuf), 0) == -1){
    perror("Client: send");
    exit(1);
  }
  printf("Client sent: %s",b);
  printf("  -- and it was %d bytes.\n",bsize);
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
      perror("Client: recv failed");
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
  printf("Client received :%s",b);
  printf(": and it was %d bytes.\n", (int)strlen(b));
  return strlen(b);
}

void mealy(int msock)
{
  char buf[MAXDATASIZE] = "";
  int numbytes;

  ////// Send
 /*
  strcpy(buf,"this is a client to server test");
  mealysend(msock,buf,strlen(buf));
*/
  char *n = NULL;

  int state = 1;
  int machine = 1;
  char verify[MAXDATASIZE];
  char ring[MAXDATASIZE] = "RING";
  char hello[MAXDATASIZE] = "HELLO";
  char no[MAXDATASIZE] = "NO";
  char rawmessage[MAXDATASIZE] = "y";

  while (machine == 1)
  {  
    switch (state)
    {
      //1 = Idle
      case 1:
        printf("Would you like to begin a call?\n");
        char call[MAXDATASIZE];
        fgets(call, MAXDATASIZE, stdin);
        if ((n = strchr(call, '\n')) != NULL) //cut out the '\n' from pressing "enter"
            *n = '\0';
        printf("User Inputted: %s\n", call);

        if (call[0] == 'y' && call[1] == '\0')
        {
            printf("State Transition: Idle -> Call\n");
            state = 2;
        }
        else if (call[0] == 'q' && call[1] == '\0') //optional quit option via 'q'
        {
            machine = 0;
        }
        break; //recall switch statement via while loop

      //2 = Call
      case 2:
        

        mealysend(msock, ring, strlen(ring));
        printf("Message Sent: %s\n", ring);
        numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
        
        // <TO>/RING

        if (numbytes == -1) 
        {
            printf("Client timeout.\n");
            while (numbytes == -1) //as long as timeout occurs, continue repeating sends
            {
                printf("State Transition: Call -> Call\n"); 
                mealysend(msock, ring, strlen(ring));
                printf("Message Sent: %s\n", ring);
                numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
                if (numbytes == -1)
                {
                    printf("Client timeout.\n");
                }
                else
                {
                    printf("Message Received: %s\n", buf);
                }
            }
        }
        else if (numbytes == 0) 
        {
            printf("Client stop; server socket closed.\n");
            state = 1;
        }
        else 
        {
            printf("Message Received: %s\n", buf);
        }
        strcpy(verify, "AHOY");

        if (strcmp(buf, verify) == 0) //verify that the message received is "AHOY"
        {
            printf("State Transition: Call -> Talk\n");
            state = 3;
            mealysend(msock, hello, strlen(hello));
        	printf("Message Sent: %s\n", hello);
        }
        break; //go to switch statement via the while loop



      //3 = Talk
      case 3:
     
        while (!((rawmessage[0] == 'n' && rawmessage[1] == '\0') || (rawmessage[0] == '#' && rawmessage[1] == '\0')))
        //continue to prompt user to send data as long as 'n' or '#' have not been given
        {
            printf("What data would you like to send?\n");
            fgets(rawmessage, MAXDATASIZE, stdin);
            if ((n = strchr(rawmessage, '\n')) != NULL) //cut off '\n' resulting from pressing "enter"
                *n = '\0';

            //for printing string after "DATA"
            char message[MAXDATASIZE];
            strncpy(message, rawmessage + 5, sizeof(rawmessage) - 5); 
            int lastchar = sizeof(message);
            message[lastchar] = '\0';
        	
            if (rawmessage[0] == 'D' && rawmessage[1] == 'A' && rawmessage[2] == 'T' && rawmessage[3] == 'A' && rawmessage [4] == ' ')
            {
                printf("User Inputted: %s\n", rawmessage);
                mealysend(msock, rawmessage, strlen(rawmessage));
                printf("Message Sent: %s\n", message);
                printf("State Transition: Talk -> Talk\n");
            }
            else if ((rawmessage[0] == 'n' && rawmessage[1] == '\0') || (rawmessage[0] == '#' && rawmessage[1] == '\0'))
            //breaks while loop
            {
                printf("User Inputted: %s\n", rawmessage);
            }
            else
            //if not above 2 options, then invalid input
            {
                printf("User Inputted: %s\n", rawmessage);
                printf("USAGE: \"DATA {Message}\"\n");
            }
        }

        // CHECK
        if (rawmessage[0] == '#' && rawmessage[1] == '\0')
        {
            printf("State Transition: Talk -> Check\n");
            char get[MAXDATASIZE] = "GET";
            mealysend(msock, get, strlen(get));
            printf("Message Sent: %s\n", get);
            state = 4;
            rawmessage[0] = 'y'; 
            //need to reset rawmessage[0] otherwise on second time in Talk state,
            //an infinite loop could occur
        }
        
        //HANGUP
        if (rawmessage[0] == 'n' && rawmessage[1] == '\0')
        {  
            printf("State Transition: Talk -> Hangup\n");
            char finished[MAXDATASIZE] = "FINISHED";
            mealysend(msock, finished, strlen(finished));
            printf("Message Sent: %s\n", finished);
            state = 5;
            rawmessage[0] = 'y';
            //need to reset rawmessage[0] otherwise on second time in Talk state,
            //an infinite loop could occur            
        }

        break;

      //4 = Check & Try
      case 4:
        
        
        //strcpy(buf, no);

        printf("Please enter the 1-digit Access Code: ");
        char code[MAXDATASIZE];
        fgets(code, MAXDATASIZE, stdin);
        if ((n = strchr(code, '\n')) != NULL) //cut off '\n' resulting from pressing "enter"
            *n = '\0';

        if (code[0] == 'C' && code[1] == 'O' && code[2] == 'D' && code[3] == 'E' && code[4] == ' ')
        {
        	printf("User Inputted: %s\n", code);
        	printf("State Transition: Check -> Try\n");
        	mealysend(msock, code, strlen(code));
        	printf("Message Sent: %s\n", code);
        	numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
        	if (numbytes == -1)
        	{
           		printf("Passcode is not %s\nTry again\n", code);
            	printf("State Transition: Check -> Try\n");
        	}
        	else if (numbytes == 0)
        	{
            	printf("Client stop; server socket closed.\n"); 
        	}
        	else 
        	{
            	printf("Message Received: %s\n", buf);
        	}
        }
        else
        //If format of user's input is wrong, advise user on proper input format
        {
        	printf("User Inputted: %s\n", code);
        	printf("USAGE: \"CODE {Single-Digit #}\"\n");
        }
        
		//NO
        if (strcmp(buf, no) == 0)
        {
           	printf("State Transition: Try -> Check\n");
           	state = 4;
        }

        //MSG
        if (buf[0] == 'M' && buf[1] == 'S' && buf[2] == 'G' && buf[3] == ' ')
        {
            state = 3;            
            char message[MAXDATASIZE];
            strncpy(message, buf + 4, sizeof(rawmessage) - 4);
            int lastchar = sizeof(message);
            message[lastchar] = '\0';
            printf("Answer Machine Message: %s\n", message);
            printf("State Transition: Try -> Talk\n");
            char noted[MAXDATASIZE] = "NOTED";
            mealysend(msock, noted, strlen(noted));
            printf("Message Sent: %s\n", noted);
        }
        //FAIL
        if (buf[0] == 'F' && buf[1] == 'A' && buf[2] == 'I' && buf[3] == 'L' && buf[4] == '\0')
        {
            state = 5;
            printf("State Transition: Try -> Hangup\n");
            char quit[MAXDATASIZE] = "QUIT";
            mealysend(msock, quit, strlen(quit));
            printf("Message Sent: %s\n", quit);
        }
        break;
      //5 = Hangup
      case 5:
        
        numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
        if (numbytes == -1)
        {
            while(numbytes == -1)
            {
                numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
            }
            printf("Message Received: %s\n", buf);
        }
        else if (numbytes == 0)
        {
            printf("Client stop; server socket closed.\n"); 
        }
        else 
        {
            printf("Message Received: %s\n", buf);
        }
        strcpy(verify, "LATER");
        if (strcmp(verify, buf) == 0)
        {
            printf("State Transition: Hangup -> Idle\n");
            char bye[MAXDATASIZE] = "BYE";
            mealysend(msock, bye, strlen(bye));
            printf("Message Sent: %s\n", bye);
            state = 1;
        }
        break;
    }
  }

}

int main(int argc, char *argv[])
{
  int sockfd, sen, valopt;  
  
  struct hostent *hp;
  struct sockaddr_in server;
  socklen_t lon;
  
  extern char *optarg;
  extern int optind;
  int c;
  char *sname=SVRNAME, *pname = SVRPORT;
  static char usage[] = "usage: %s [-s sname] [- p pname]\n";
  
  long arg;
  fd_set myset;
  struct timeval tv;  // Time out
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  // parse the command line arguments
  while ((c = getopt(argc, argv, "s:p:")) != -1)
    switch (c) {
    case 'p':
      pname = optarg;
      break;
    case 's':
      sname = optarg;
      break;
    case '?':
      fprintf(stderr, usage, argv[0]);
      exit(1);
      break;
    }

  // convert the port string into a network port number
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(pname));

  // convert the hostname to a network IP address
  hp = gethostbyname(sname);
  if ((hp = gethostbyname(sname)) == NULL) {  // get the host info
    herror("Client: gethostbyname failed");
    return 2;
  }
  bcopy(hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
  bzero(&server.sin_zero, 8);

  // get a socket to play with
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
    perror("Client: could not create socket");
    exit(-1);
  }
  
  //////////////////////////////////////
  // open the connection
  
  // set the connection to non-blocking 
  arg = fcntl(sockfd, F_GETFL, NULL);
  arg |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, arg);
  //
  if ((sen = connect(sockfd,(struct sockaddr *)&server,sizeof(server))) == -1) {
    if (errno == EINPROGRESS) {
      FD_ZERO(&myset);
      FD_SET(sockfd, &myset);
      if (select(sockfd + 1, NULL, &myset, NULL, &tv) > 0){
	lon = sizeof(int);
	getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
	if (valopt) {
	  fprintf(stderr, "Client error in connection() %d - %s\n", valopt, strerror(valopt));
	  exit(-1);
	}
      }
      else {
	perror("Client: connection time out");
	exit(-1);
      }				
    }
    else {
      fprintf(stderr, "Client error connecting %d - %s\n", errno, strerror(errno));
      exit(0);
    }
  }
  //
  ///////////////////////////////
  
  // Set to blocking mode again
  arg = fcntl(sockfd, F_GETFL, NULL);
  arg &= (~O_NONBLOCK);
  fcntl(sockfd, F_SETFL, arg);

  //////////////////////////////

  mealy(sockfd);
  // and now it's done
  //////////////////////////////
  
  close(sockfd);
  return 0;
}
