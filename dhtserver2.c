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

#define PORT 22328
#define BUFSIZE 2048

int main()
{
	int sock,fd,n,m,i,j=0,k=0;
	int rqst,port1,port2; 
 	struct sockaddr_in myaddr, client;
 	unsigned char buf[BUFSIZE];  
	socklen_t addrlen = sizeof(client); 

	struct hostent *hp; 
	struct sockaddr_in servaddr,cliaddr;
	socklen_t cliaddrlen= sizeof(cliaddr);  
	char buffer[BUFSIZE];
	int nbytes,flag=0;
	FILE *fp;
 	char data[255];
 	int MAX_SIZE=1000;
 	int counter=0;
 	char *eachLine[MAX_SIZE];
 	char *key[MAX_SIZE];
 	char *userName[MAX_SIZE];
 	char *temp ;
	int recvlen;
	int sockoptval = 1;
	char msg[20] = "";
	char post[20] = "POST ";
	char val[20]="";
	post[strlen(post)] = 0;
	val[strlen(val)] = 0;
	msg[strlen(msg)] = 0;
	char get[20] = "GET ";
	get[strlen(get)] = 0;

	char *val1;
	val1 = (char *)malloc(255);
	char *out;
	out = (char *)malloc(255);
	char ip4[INET_ADDRSTRLEN];

//Read from file
 	fp = fopen("server2.txt","r");
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

//Create TCP socket -- rutgers reused code 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		perror("cannot create socket");
 		return 0;
 	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	//Server 2 details
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET; 
	myaddr.sin_port = htons(PORT);
	hp = gethostbyname("localhost");

	  if (hp==0)
	    {
	      perror("Unknown host");
	      exit(1);
	    }
	memcpy((void *)&myaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	memset((char *)&client, 0, sizeof(client));

	//Prints IP address and port number of Server 2
	printf("The Server 2 has TCP Port Number 22328 and IP adress ");
	puts(inet_ntop(AF_INET, &(myaddr.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n"); 
 
//Bind socket -- rutgers reused code
	if (bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
	{ 
		perror("bind failed"); 
		return 0;
	 }

	//Listen on socket
		if (listen(sock, 5) < 0) 
		{ 
			perror("listen failed"); 
			exit(1);
		 }

	while(1)
 	{
	flag=0;
//Accept on child socket -- rutgers reused code
 	while ((rqst = accept(sock, (struct sockaddr *)&client, &addrlen)) < 0) 
	{
 	 
		if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR))
	 	{ perror("accept failed"); 
		exit(1); } 
	}
	buffer[strlen(buffer)] = 0;

//Read from child socket
	nbytes = read(rqst, buffer, 15); 
	if(nbytes<0)
	{
		perror("read failed");
		exit(1);
	}

	port1 = ntohs(client.sin_port);
	printf("Server 2 has received a request with key %s from Server 1 with port number %d and IP address ",buffer,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");

//Extract key from GET key
	j=0;
	for(i=4;i<9; i++)
	{ 
		val[j] = buffer[i];
		j++;
	} 
	strcpy(msg,post);

	msg[strlen(msg)] = 0;

//Server 2 checks its entries for value
	for(i=0; i < counter; i++)
  	{
 	if (strcmp(userName[i],val) == 0)
   {    
	
	strcat(msg,key[i]);
	msg[strlen(msg)] = 0;
	nbytes = write(rqst, msg, 15); //writes to child socket
	printf("Server 2 sends the reply %s to Server 1 with port number %d and IP address ",msg,port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	
	strcpy(val,"");
	val[strlen(val)] = 0;
	strcpy(msg,post);
	msg[strlen(msg)] = 0;
    flag=1;
	
     	if(nbytes<0)
		{
			perror("write failed");
			exit(1);
		}
	}
  }



//Server 2 requests Server 3 for value
	if(flag == 0)
	{
//Creates socket -- rutgers reused code 
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		perror("cannot create socket"); 
		return 0; 
	}
//Server 3 details
	memset((char*)&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(23328);
	hp = gethostbyname("localhost"); 

	if (!hp) 
	{ 
		fprintf(stderr, "Unknown host"); 
		return 0;
 	}  
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length); 

//Server 2 acting as TCP client details
	memset((char *)&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	memcpy((void *)&cliaddr.sin_addr, hp->h_addr_list[0], hp->h_length); 

//Bind TCP socket-- rutgers reused code
	if (bind(fd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
		perror("bind failed");
		return 0;
	}


	getsockname(fd,(struct sockaddr *)&cliaddr, &cliaddrlen); 
	port2 = ntohs(cliaddr.sin_port);

//Connect to Server 3 -- rutgers reused code 
	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 		
		perror("connect failed");
 		return 0;
	}
	printf("Server 2 sends the request %s to Server 3 \n",buffer);
	printf("TCP port number is %d and IP address is ",port2);
	puts(inet_ntop(AF_INET, &(servaddr.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");

//Writes to TCP socket
	nbytes = write(fd, buffer, 15); 
	if(nbytes<0)
	{
		perror("write failed");
		exit(1);
	}

//Read from TCP socket and print value
	nbytes = read(fd, buf, 15); 
	if(nbytes<0)
	{
		perror("read failed");
		exit(1);
	}

	printf("Server 2 received the value %s from Server 3 with port number 23328 and IP address ",buf);
	puts(inet_ntop(AF_INET, &(servaddr.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");

//Close TCP socket to Server 3
	close(fd); 
	printf("Server 2 closed the TCP connection with Server 3\n");
	printf("\n");

//Write the received value to server 2 child socket
	n = write(rqst, buf, 15);

	if (n < 0)
   {
	 perror("sento");
	 exit(1);
	}

	printf("The Server 2, sent reply %s to Server 1 with port number %d and IP address ",buf, port1);
	puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
	printf("\n");

	close(rqst);//Close child socket 

//Update Server 2 entries with entry received from Server 3
	j=0;
	val1 = (char *)malloc(255); 
	for(i=4;i<9; i++)
	{ 
		val1[j] = buffer[i];
		j++;
	}
	val1[strlen(val1)] = 0;
 
	out = (char *)malloc(255);
	k=0;
	for(j=5; j<strlen(buf); j++)
	{out[k]=buf[j];
	k++;}
	val1[strlen(val1)] = 0;

	userName[counter]= val1;
	key[counter]= out;
	counter++;

	buffer[strlen(buffer)] = 0;   	 

	}

  }
 
}

