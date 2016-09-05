#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define BUFSIZE 2048

int main()
{
  int sock, length, n,i,j,k=0,m,port1;
  struct sockaddr_in servaddr, client;
  struct hostent *hp;
  char buff[25];
  char buff1[25];
  FILE *fp;
  char data[255];
  int MAX_SIZE=1000;
  int counter=0;
  char *eachLine[MAX_SIZE];
  char *key[MAX_SIZE];
  char *userName[MAX_SIZE];
  char *temp ;
  unsigned char buf[BUFSIZE];
  int recvlen;
  socklen_t addrlen = sizeof(servaddr);  
  socklen_t cliaddrlen = sizeof(client);
  char msg[20] = "";
  char get[20] = "GET ";
  msg[strlen(msg)] = 0;
  get[strlen(get)] = 0;
  char ip4[INET_ADDRSTRLEN];

  //Reading from file
  fp = fopen("client2.txt","r");
  if (fp==NULL)
    {
      perror("Error opening file");
      return(-1);
    }
  while(fgets(data,255,fp)!=NULL)
    {
      eachLine[counter]=data;
      
      char *ptr = strdup(eachLine[counter]); 
      i=0;
      for (temp= strtok(ptr," "); temp != NULL; temp = strtok(NULL, " "))
   { 
    if(i==0 && temp!=NULL) {
      userName[counter] = temp;
    } else {
      if(temp!=NULL)
        key[counter] =temp;
    }

    i++;  
        }
      counter++;
    }
  fclose(fp);

//Create UDP socket -- rutgers reused code
  sock = socket(AF_INET, SOCK_DGRAM,0);
  if (sock < 0)
    {
      perror("socket");
      exit(1);
    }

//UDP server details 
memset((char*)&servaddr, 0, sizeof(servaddr));
servaddr.sin_family = AF_INET; 
servaddr.sin_port = htons(21328); 

  hp = gethostbyname("localhost");

  if (hp==0)
    {
      perror("Unknown host");
      exit(1);
    }
  memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length); 

//UDP Client details 
memset((char*)&client, 0, sizeof(client));
client.sin_family = AF_INET; 
memcpy((void *)&client.sin_addr, hp->h_addr_list[0], hp->h_length); 

m = bind(sock,(struct sockaddr *)&client, sizeof(client));
 if (m < 0)
   {
     perror("Error on binding");
     exit(1);
   }

getsockname(sock,(struct sockaddr *)&client, &cliaddrlen);
 
//Getting user input
  printf("Please enter your search (USC, UCLA etc): \n");
  gets(buff1);

strcpy(msg,get);
char out[20]="";

//If client finds a match for key, it requests Server 1 for value   
  for(i=0;i<counter;i++) {
     
      if (strcmp(userName[i],buff1) == 0)
  {
  printf("Client 2 has received a request with search key word %s which maps to key %s", buff1, key[i]);
  printf("\n");
  
  strcat(msg,key[i]); 
  msg[strlen(msg)] = 0;
//Sends request to server 1 -- rutgers reused code
  if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
  {
  perror("sendto failed");
  return 0;
  } 
    printf("Client 2 sends the request %s to Server 1 with Port number 21328 and IP address ",msg);
  puts(inet_ntop(AF_INET, &(servaddr.sin_addr), ip4, INET_ADDRSTRLEN));
  printf("\n");

     }
  }    

  port1 = ntohs(client.sin_port);
  printf("Client 2's Port number is %d and the IP address is ",port1);
  puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
  printf("\n");

//Receives response from server 1 -- rutgers reused code
  recvlen = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr *)&servaddr, &addrlen);


//Extracts value from received message and prints value
  if (recvlen > 0) 
  {
    buf[recvlen] = 0;
    k=0;
    for(j=5; j<recvlen; j++)
    {
      out[k]=buf[j];
      k++;
    }

  printf("Client 2 received the value %s from Server 1 with Port number 21328 and IP address ", out);
  puts(inet_ntop(AF_INET, &(servaddr.sin_addr), ip4, INET_ADDRSTRLEN));
  printf("\n");
  printf("Client 2's Port number is %d and the IP address is ",port1);
  puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
   
  }
  else
  {
  perror("Receive failed");
  exit(1);
  }
  //Close socket
  close(sock);
}
