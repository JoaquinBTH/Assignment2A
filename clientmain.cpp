#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <math.h>
#include <iostream>
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

  //Create buffer
  char buf[256];
  memset(buf, 0, 256);

  //Recieve a message from the server
  int recieve = recv(sock, buf, sizeof buf, 0);
  if(recieve == -1)
  {
    printf("Recieve Failed!");
    return 3;
  }
  else if(strcmp(buf, "TEXT TCP 1.0\n") == 0)
  {
    printf("Wrong Client Application!");
    return 4;
  }

  //Send "OK" as a response to the server
  std::string ok = "OK\n";
  int response = send(sock, ok.c_str(), ok.length(), 0);
  if(response == -1)
  {
    printf("Response Failed!");
    return 5;
  }

  //Recieve the operation and numbers
  memset(buf, 0, 256);
  recieve = recv(sock, buf, sizeof buf, 0);
  if(recieve == -1)
  {
    printf("Recieve Failed!");
    return 6;
  }
  printf("%s",buf);
  
  //Start calculating
  double f1,f2,fresult;
  int i1,i2,iresult;
  int size = 0;
  bool floatValue;
  if(buf[0] == 'f')
  {
    floatValue = true;
    char *ptr;
    f1 = strtod(&buf[5], &ptr);
    for(int i = 5; i < 15; i++)
    {
      if (buf[i] == ' ')
      {
        break;
      }
      else
      {
        size++;
      }
    }
    f2 = strtod(&buf[5 + size], NULL);

    //Float operations
    if(buf[1] == 'a')
    {
      fresult = f1 + f2;
    }
    else if(buf[1] == 'd')
    {
      fresult = f1 / f2;
    }
    else if(buf[1] == 'm')
    {
      fresult = f1 * f2;
    }
    else if(buf[1] == 's')
    {
      fresult = abs(f1-f2);
    }
  }
  else
  {
    floatValue = false;
    i1 = atoi(&buf[4]);
    size = log10(i1) + 1;
    i2 = atoi(&buf[4 + size]);

    //Int operations
    if(buf[0] == 'a')
    {
      iresult = i1 + i2;
    }
    else if(buf[0] == 'd')
    {
      iresult = i1 / i2;
    }
    else if(buf[0] == 'm')
    {
      iresult = i1 * i2;
    }
    else if(buf[0] == 's')
    {
      iresult = abs(i1-i2);
    }
  }
  
  std::string result;
  if(floatValue == true)
  {
    result = std::to_string(fresult) + "\n";
  }
  else
  {
    result = std::to_string(iresult) + "\n";
  }

  response = send(sock, result.c_str(), result.length(), 0);
  if(response == -1)
  {
    printf("Response Failed!");
    return 7;
  }

  //Confirmation if correct or error
  memset(buf, 0, 256);
  recieve = recv(sock, buf, sizeof buf, 0);
  if(recieve == -1)
  {
    printf("Recieve Failed!");
    return 8;
  }
  printf("%s",buf);

  //Close socket
  close(sock);
  return 0;
}
