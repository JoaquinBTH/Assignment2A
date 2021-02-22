#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <ctime>

// Included to get the support library
#include <calcLib.h>

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass argument during compilation '-DDEBUG'
#define DEBUG


using namespace std;

void randomCalculation(char buf[], char result[])
{
  int randomNumber = rand() % 8 + 1;
  int i1, i2, iresult;
  double f1, f2, fresult;
  i1 = rand() % 100 + 1;
  i2 = rand() % 100 + 1;

  f1 = (double)rand() / (RAND_MAX/100);
  f2 = (double)rand() / (RAND_MAX/100);
  if(randomNumber == 1)
  {
    //Add
    iresult = i1 + i2;
    sprintf(buf, "add %d %d\n", i1, i2);
    sprintf(result, "%d\n", iresult);
  }
  else if(randomNumber == 2)
  {
    //Multiplication
    iresult = i1 * i2;
    sprintf(buf, "mul %d %d\n", i1, i2);
    sprintf(result, "%d\n", iresult);
  }
  else if(randomNumber == 3)
  {
    //Division
    iresult = i1 / i2;
    sprintf(buf, "div %d %d\n", i1, i2);
    sprintf(result, "%d\n", iresult);
  }
  else if(randomNumber == 4)
  {
    //Subtraction
    iresult = abs(i1 - i2);
    sprintf(buf, "sub %d %d\n", i1, i2);
    sprintf(result, "%d\n", iresult);
  }
  else if(randomNumber == 5)
  {
    //fAdd
    fresult = f1 + f2;
    sprintf(buf, "fadd %8.8g %8.8g\n", f1, f2);
    sprintf(result, "%8.8g\n", fresult);
  }
  else if(randomNumber == 6)
  {
    //fMultiplication
    fresult = f1 * f2;
    sprintf(buf, "fmul %8.8g %8.8g\n", f1, f2);
    sprintf(result, "%8.8g\n", fresult);
  }
  else if(randomNumber == 7)
  {
    //fDivision
    fresult = f1 / f2;
    sprintf(buf, "fdiv %8.8g %8.8g\n", f1, f2);
    sprintf(result, "%8.8g\n", fresult);
  }
  else if(randomNumber == 8)
  {
    //fSubtraction
    fresult = abs(f1 - f2);
    sprintf(buf, "fsub %8.8g %8.8g\n", f1, f2);
    sprintf(result, "%8.8g\n", fresult);
  }
}



int main(int argc, char *argv[])
{  
  if (argc != 2)
  {
    printf("Usage: %s <ip>:<port> \n", argv[0]);
    exit(1);
  }
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  if (Desthost == NULL || Destport == NULL)
  {
    printf("Usage: %s <ip>:<port> \n", argv[0]);
    exit(1);
  }
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port=atoi(Destport);
#ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
#endif

  srand(unsigned(time(0)));

  int backLogSize = 5;
  int yes = 1;

  struct addrinfo hint, *servinfo, *p;
  int rv;
  int serverSock;

  memset(&hint, 0, sizeof (hint));
  hint.ai_family = AF_UNSPEC;
  hint.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(Desthost, Destport, &hint, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((serverSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      printf("Socket creation failed.\n");
      continue;
    }
    printf("Socket Created.\n");
    rv = bind(serverSock, p->ai_addr, p->ai_addrlen);
    if(rv == -1)
    {
      perror("Bind failed!\n");
      close(serverSock);
      continue;
    }
    break;
  }

  freeaddrinfo(servinfo);

  if (p == NULL)
  {
    fprintf(stderr, "Client failed to create an apporpriate socket.\n");
    exit(1);
  }

  if(setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("setsockopt failed!\n");
    exit(1);
  }

  printf("Lyssnar på %s:%d \n",Desthost, port);

  rv = listen(serverSock, backLogSize);
  if(rv == -1)
  {
    perror("Listen failed!\n");
    exit(1);
  }

  struct sockaddr_in clientAddr;
  socklen_t client_size = sizeof(clientAddr);

  int clientSock;
  char buf[256];
  char result[20];
  int recieve, response;

  int loops = 0;
  while(loops < 1)
  {
    clientSock = accept(serverSock, (sockaddr*)&clientAddr, &client_size);
    if(clientSock == -1)
    {
      perror("Accept failed!\n");
      loops++;
      continue;
    }

    struct timeval time;
    time.tv_sec = 5;
    time.tv_usec = 0;
    int socketTimeOut;
    socketTimeOut = setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof time);
    if (socketTimeOut == -1)
    {
      printf("Socket option failed! %s\n", strerror(errno));
      exit(1);
    }

    printf("Client connected!\n");
    
    //Send protocol message
    memset(&buf, 0, 256);
    strcpy(buf, "TEXT TCP 1.0\n\n");
    response = send(clientSock, &buf, sizeof (buf), 0);
    if(response == -1)
    {
      perror("Send failed!\n");
      continue;
    }

    //Recieve message
    memset(&buf, 0, 256);
    recieve = recv(clientSock, &buf, sizeof(buf), 0);
    if(recieve == -1)
    {
      printf("Client failed to reply!\n");
      sprintf(buf, "ERROR TO\n");
      send(clientSock, &buf, sizeof(buf), 0);
      continue;
    }
    
    //Check if message is "OK\n"
    if(strcmp(buf, "OK\n") == 0)
    {
      printf("OK was recieved!\n");
    }
    else
    {
      printf("OK was not recieved!\n");
      continue;
    }

    //Give buffer random calculation and calculate the result
    memset(&buf, 0, 256);
    memset(&result, 0, 20);
    randomCalculation(buf, result);

    //Send calculations to client
    response = send(clientSock, &buf, sizeof(buf), 0);
    if(response == -1)
    {
      perror("Send failed!\n");
      continue;
    }

    //Recieve answer in form of a number
    memset(&buf, 0, 256);
    recieve = recv(clientSock, &buf, sizeof(buf), 0);
    if(recieve == -1)
    {
      printf("Client failed to reply!\n");
      sprintf(buf, "ERROR TO\n");
      send(clientSock, &buf, sizeof(buf), 0);
      continue;
    }

    //Check if the answer was correct
    double D, x, y;
    x = strtod(buf, NULL);
    y = strtod(result, NULL);
    D = abs(x - y);
    if(strcmp(buf, result) == 0 || D < 0.0001)
    {
      printf("Answer is correct!\n");
      memset(&buf, 0, 256);
      strcpy(buf, "OK\n");
    }
    else
    {
      printf("Wrong answer\n");
      memset(&buf, 0, 256);
      strcpy(buf, "NOT OK\n");
    }

    //Send OK or NOT OK
    response = send(clientSock, &buf, sizeof(buf), 0);
    if(response == -1)
    {
      perror("Send failed!\n");
    }
  }

  close(clientSock);
  close(serverSock);
  return 0;
}
