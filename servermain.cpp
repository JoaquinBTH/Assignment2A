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

// Included to get the support library
#include <calcLib.h>

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass argument during compilation '-DDEBUG'
#define DEBUG


using namespace std;


int main(int argc, char *argv[]){
  
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port=atoi(Destport);
#ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
#endif

  int backLogSize = 5;
  int yes = 1;

  int serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port=htons(port);
  inet_aton(Desthost, (in_addr*)&serverAddr.sin_addr.s_addr);

  if(setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("setsockopt failed!\n");
    exit(1);
  }

  int rv = bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
  if(rv == -1)
  {
    perror("Bind failed!\n");
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
  int recieve, response;

  while(1)
  {
    clientSock = accept(serverSock, (sockaddr*)&clientAddr, &client_size);
    if(clientSock == -1)
    {
      perror("Accept failed!\n");
      continue;
    }
    printf("Client connected!\n");
    
    memset(&buf, 0, 256);
    strcpy(buf, "TEXT TCP 1.0\n\n");
    response = send(clientSock, &buf, sizeof (buf), 0);
  }

  return 0;
}
