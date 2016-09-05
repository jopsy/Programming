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

#define PORT 23328
#define BUFSIZE 2048

int main()
{
	int sock,n,m,i,j=0;
	int rqst,port1; 
	struct sockaddr_in myaddr, client;
	unsigned char buf[BUFSIZE]; 
	socklen_t alen = sizeof(client); 
	char buffer[BUFSIZE];
	int nbytes;
	struct hostent *hp; 
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
	char ip4[INET_ADDRSTRLEN];

	//Read from file
	 fp = fopen("server3.txt","r");
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

	//Server 3 details
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

	//Prints IP address and port number of Server 3
	printf("The Server 3 has TCP Port Number 23328 and IP adress ");
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
		//Accept on child socket -- rutgers reused code
	 	while ((rqst = accept(sock, (struct sockaddr *)&client, &alen)) < 0) 
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
		printf("Server 3 has received a request with key %s from Server 2 with port number %d and IP address ",buffer,port1);
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
		
		printf("Server 3 sends the reply %s to Server 2 with port number %d and IP address ",msg,port1);
		puts(inet_ntop(AF_INET, &(client.sin_addr), ip4, INET_ADDRSTRLEN));
		printf("\n");
		strcpy(val,"");
		val[strlen(val)] = 0;
		strcpy(msg,post);
		msg[strlen(msg)] = 0;
	        
		
	     	if(nbytes<0)
		{
		perror("write failed");
		exit(1);
		}
	  }
	 }

	}
	close(rqst); 
}

