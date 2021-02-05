#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// Enable if you want debugging to be printed, see examble below.
// Alternative, pass 
#define DEBUG


// Included to get the support library
#include <calcLib.h>

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

  sockaddr_in hint;
  memset(&hint, 0, sizeof hint);
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port);
  inet_pton(AF_INET, Desthost, &hint.sin_addr);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
    printf("Socket Failed!");
    return 1;
  }

  int con = connect(sock, (sockaddr*)&hint, sizeof (hint));
  if (con == -1)
  {
    printf("Connection Failed!");
    return 2;
  }

  char buf[256];

  memset(buf, 0, 256);
  int recieve = recv(sock, buf, sizeof buf, 0);
  
  

  close(sock);
}
