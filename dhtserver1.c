#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 21328
#define BUFSIZE 2048

int main()
{
 	int sock,fd, clilen, n,m,i,j=0,k=0,count=1;
 	struct sockaddr_in server, client;
	struct hostent *hp; 
	struct sockaddr_in servaddr,myaddr; 
	int nbytes,end,port1,port2;
	int flag=0;
 	unsigned char buf[BUFSIZE], buffer[BUFSIZE]; 
	socklen_t addrlen = sizeof(client); 
	socklen_t myaddrlen= sizeof(myaddr); 
 	FILE *fp;
 	char data[255];
 	int MAX_SIZE=1000;
 	int counter=0;
 	char *eachLine[MAX_SIZE];
 	char *key[MAX_SIZE];
 	char *userName[MAX_SIZE];
 	char *temp ;
	int recvlen;
	char val[20]="";
	val[strlen(val)] = 0;
	char msg[20] = "";
	char post[20] = "POST ";
	post[strlen(post)] = 0;

	char get[20] = "GET ";
	get[strlen(get)] = 0;

	msg[strlen(msg)] = 0;
	char *val1;
	val1 = (char *)malloc(255);
	char *out;
	out = (char *)malloc(255);
	char ip4[INET_ADDRSTRLEN];

//Read from file
 	fp = fopen("server1.txt","r");
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
 sock = socket(AF_INET,SOCK_DGRAM,0);
 if (sock < 0)
   {
     perror("Error opening socket");
     exit(1);
   }

//UDP Server 1 details 
memset((char *)&server, 0, sizeof(server));

 server.sin_family = AF_INET;
 server.sin_port = htons(PORT);
 hp = gethostbyname("localhost");

  if (hp==0)
    {
      perror("Unknown host");
      exit(1);
    }
  memcpy((void *)&server.sin_addr, hp->h_addr_list[0], hp->h_length); 
 
//Prints IP and port number of UDP Server 1
printf("The Server 1 has UDP Port Number 21328 and IP adress ");
puts(inet_ntop(AF_INET, &(server.sin_addr), ip4, INET_ADDRSTRLEN));
printf("\n");

//Binds UDP socket -- rutgers reused code
 m = bind(sock,(struct sockaddr *)&server,sizeof(server));
 if (m < 0)
   {
     perror("Error on binding");
     exit(1);
   }
 
strcpy(msg,post);

//Infinite loop for Server 1
 while(1)
   {
flag = 0;

 	//Server 1 receives request from Client -- rutgers reused code
	recvlen = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr *)&client, &addrlen); 
	

if (recvlen > 0) 
	{ 
	buf[recvlen] = 0;
 	port1 = ntohs(client.sin_port);

	if(count % 2)
	{printf("Server 1 has received a request with key %s from Client 1 with port number %d and IP address ",buf,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));}
	else if (count % 2 == 0)
	{
	printf("Server 1 has received a request with key %s from Client 2 with port number %d and IP address ",buf,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	}
	printf("\n");
	}
//Extract key from GET key
j=0;
for(i=4;i<9; i++)
	{ 
	val[j] = buf[i];
	j++;
	} 
val[strlen(val)] = 0;

//Server 1 checks its entries for value and if found sends to client
for(i=0; i < counter; i++)
  {
 	if (strcmp(userName[i],val) == 0)
   {    
	strcat(msg,key[i]);
	n = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&client, addrlen);//send to client
	if(count % 2)
	{printf("Server 1 sends the reply %s to Client 1 with port number %d and IP address ",msg,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));}

	else if (count % 2 == 0)
	{printf("Server 1 sends the reply %s to Client 2 with port number %d and IP address ",msg,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));}
	printf("\n");

	strcpy(msg,post);
	msg[strlen(msg)] = 0;
	strcpy(val,"");
	val[strlen(val)] = 0;
        flag=1;
	
     	if (n < 0)
       	{
	 perror("sento");
	 exit(1);
       	 }
	} 
  }

//Server 1 requests Server 2 for value
	if(flag == 0)
	{
//Create TCP socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		perror("cannot create socket"); 
		return 0; 
	}

//Server 2 details
	memset((char*)&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(22328);
	hp = gethostbyname("localhost"); 

	if (!hp) 
	{ 
	fprintf(stderr, "Unknown host"); 
	return 0;
 	}  
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length); 

//Server 1 acting as TCP client details
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	memcpy((void *)&myaddr.sin_addr, hp->h_addr_list[0], hp->h_length); 

//Bind TCP socket-- rutgers reused code
	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	getsockname(fd,(struct sockaddr *)&myaddr, &myaddrlen); 
	port2 = ntohs(myaddr.sin_port);

//Connect TCP socket -- rutgers reused code
	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{		 
		perror("connect failed");
 		return 0;
	}
	
	printf("Server 1 sends the request %s to Server 2 \n",buf);
	printf("TCP port number is %d and IP address is ",port2);
	puts(inet_ntop(AF_INET, &(myaddr.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");
//Write to TCP socket
	nbytes = write(fd, buf, 15); 
	if(nbytes<0)
	{
		perror("write failed");
		exit(1);
	}

//Read from TCP socket and print value
	nbytes = read(fd, buffer, 15); 
	if(nbytes<0)
	{
		perror("read failed");
		exit(1);
	}


	printf("Server 1 received the value %s from Server 2 with port number 22328 and IP address ",buffer);
	puts(inet_ntop(AF_INET, &(servaddr.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");

//Close TCP socket
	close(fd);
	printf("Server 1 closed the TCP connection with Server 2\n");
	printf("\n");

//Send to Client the received value -- rutgers reused code
	n = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client, addrlen);

	if(count % 2)
	{	
		printf("The Server 1, sent reply %s to Client 1 with port number %d and IP address ",buffer, port1);
		puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	}

	else if (count % 2 == 0)
	{	
		printf("The Server 1, sent reply %s to Client 2 with port number %d and IP address ",buffer, port1);
		puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	}
	printf("\n");

	if (n < 0)
    {
	 perror("sento");
	 exit(1);
	} 
//Update Server 1 entries with entry received from Server 2
	j=0;
	val1 = (char *)malloc(255); 
	for(i=4;i<9; i++)
	{ 
		val1[j] = buf[i];
		j++;
	}

 
	out = (char *)malloc(255);
	k=0;
	for(j=5; j<strlen(buffer); j++)
	{
		out[k]=buffer[j];
		k++;
	}

	userName[counter]= val1;
	key[counter]= out;
	counter++;
	buffer[strlen(buffer)] = 0;   	 
	}
//Increment counter
	count++;

	}

//Close UDP Socket
	close(sock);
}
  
