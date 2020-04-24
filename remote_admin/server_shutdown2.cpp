// To shutdown give CTRL+C command
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<iostream>
#include<bits/stdc++.h>
#include <pthread.h> 

using namespace std;
#define PORT "3000"
#define MAX_LISTEN 10

vector<pair<int,char*> > req;

void func(int new_s,char* peer_ip4);

/* Signal Handler for SIGINT */

void closing_print(){
	cout<<"*******************************************************************************\n";
}

void getUserName(string str){
	istringstream iss(str);
	int i = 1;
	do
    {
        string subs;
        iss >> subs;
        if(i>9)
        cout << subs << " ";
       	i++;
       	
    } while (iss);	
    cout << endl;		
}

void* printList(void* args){
	while(true){
		cout<<"Currently " << req.size() <<" users are connected\n";
		cout<<"To list all the connected devices press 1:\nTo get user info press 2:\nTo shutdown all press 3:\nTo get user list press 4:\n";
		int x; cin>>x;
		
		if(x == 1){
			for(auto x:req){
				cout<<x.second<<endl;
			}
		}
		else if(x == 2){
			for(auto x:req){
				int new_s = x.first; char* peer_ip4 = x.second;
				char* message2 = "finger";
			   	int bytes_sent = send(new_s ,(void *)message2, 1024,0);
			   	
			   	char message_rec[10240];
			   	bzero(message_rec, 10240);
			   	read(new_s, message_rec, sizeof(message_rec));
			   	string str(message_rec);
			   	getUserName(str);
			   	// cout << peer_ip4 << " -- Current User Name -- " <<  str << endl;
				closing_print();
			}
		}
		else if(x == 3){
			for(auto x:req){
				int new_s = x.first; char* peer_ip4 = x.second;
				char* message2 = "shutdown -h 1";
				cout<<"Shutting down "<<peer_ip4<<endl;
			   	int bytes_sent = send(new_s ,(void *)message2, 1024,0);
			   	closing_print();	
			}
		}
		else if(x==4){
			for(auto x:req){
				int new_s = x.first; char* peer_ip4 = x.second;
				char* message2 = "whoami";
			   	int bytes_sent = send(new_s ,(void *)message2, 1024,0);

			   	char message_rec[10240];
			   	bzero(message_rec, 10240);
			   	read(new_s, message_rec, sizeof(message_rec));
			   	cout << peer_ip4 << " -- Current User Name -- " <<  message_rec << endl;
				
			}
			closing_print();
		}	
	}	
}


void sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

int main(){
	//Handle signals
	
	struct addrinfo hints;
	struct addrinfo* servinfo,*p;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; 	
	int sockfd;
	int yes=1;
	struct sigaction sa;
	socklen_t sin_size;

	
	int status = getaddrinfo(NULL,PORT, &hints, &servinfo);
	if(status != 0){
		std::cout<<"Couldn't load the server\n";
		exit(1);
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	
	int l_listen = listen(sockfd,MAX_LISTEN);
	if(l_listen == -1){
		cout<<"Listen failed\n";
		exit(1);
	}
	/* Killing any unused ports*/

	pthread_t thread_id1; 
	pthread_create(&thread_id1, NULL,printList, NULL);
	
	
	bool first_time = true;
	while(true){
		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}
			
		struct sockaddr_storage client;
		socklen_t client_size = sizeof (client);

			// cout<<"Waiting for a new connection...\n";
			int new_s = accept(sockfd, (struct sockaddr *)&client, &client_size);
			if(new_s < 0){
				cout<<"Failed to accept at server";
				exit(1);
			}

			struct sockaddr_in peer;
			socklen_t peer_size = sizeof(peer);

			int peer_status =  getpeername(new_s, (struct sockaddr*)&peer, &peer_size);
			if(peer_status == -1){
				// Report Error 
			}

			char peer_ip4[INET_ADDRSTRLEN]; 
			inet_ntop(AF_INET, &(peer.sin_addr), peer_ip4, INET_ADDRSTRLEN);
			
			req.push_back({new_s,peer_ip4});
		
	}

	for(auto x:req){
		func(x.first,x.second);
		close(x.first);
	}
}
