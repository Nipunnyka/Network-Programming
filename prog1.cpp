/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
*/

/*
int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints, //points to a strcut that youve already filled out eith relevant info
                struct addrinfo **res);
*/

// this returns a pointer to a linked list to results in res

//the following snippet sets up the structure that we will need to listen to a hosts IP on port 3490
/*
int status;
struct addrinfo hints;
struct addrinfo *servinfo;  // will point to the results

memset(&hints, 0, sizeof hints); // make sure the struct is empty
hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
}

// servinfo now points to a linked list of 1 or more struct addrinfos

// ... do everything until you don't need servinfo anymore ....

freeaddrinfo(servinfo); // free the linked-list
*/

//the following progrma will print the ip of whatever host you specify on cmd line
/* struct addrinfo for ref:
 struct addrinfo {
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname
    
        struct addrinfo *ai_next;      // linked list, next node
    };
*/

/*
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char * argv[])
{
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if(argc!=2){
        fprintf(stderr, "show iphostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    if(status=getaddrinfo(argv[1], NULL, &hints, &res) !=0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n\n", argv[1]);

    for(p=res; p!= NULL; p=p->ai_next){

        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if(p->ai_family==AF_INET){
            //IPV4
            struct sockaddr_in *ipv4=(struct sockaddr_in *)p->ai_addr;
            addr=&(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else{
            struct sockaddr_in6 *ipv6=(struct sockaddr_in6 *)p->ai_addr;
            addr=&(ipv6->sin6_addr);
            ipver="IPv6";
        }
        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }
    freeaddrinfo(res);

    return 0;

}
*/
//There’s a little bit of ugliness there where we have to dig into the different types of struct sockaddrs depending on the IP version. Sorry about that! I’m not sure of a better way around it.

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//socket()—Get the File Descriptor!
/*
#include <sys/types.h>
#include <sys/socket.h>
    
int socket(int domain, int type, int protocol); 
*/

//the most correct thing to do is to use AF_INET in your struct sockaddr_in and PF_INET in your call to socket()
//socket() simply returns to you a socket descriptor that you can use in later system calls, or -1 on error

/*
int s;
struct addrinfo hints, *res;

// do the lookup
// [pretend we already filled out the "hints" struct]
getaddrinfo("www.example.com", "http", &hints, &res);

// again, you should do error-checking on getaddrinfo(), and walk
// the "res" linked list looking for valid entries instead of just
// assuming the first one is good (like many of these examples do).
// See the section on client/server for real examples.

s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
*/

//--------------------------------------------------------------------------------------------------------------------------------------------------------

//bind()-- what port am i on?
//Once you have a socket, you might have to associate that socket with a port on your local machine. (This is commonly done if you’re going to listen() for incoming connections on a specific port

/*
    #include <sys/types.h>
    #include <sys/socket.h>
    
    int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
*/

/*
struct addrinfo hints, *res;
int sockfd;

// first, load up address structs with getaddrinfo():

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

getaddrinfo(NULL, "3490", &hints, &res);

// make a socket:

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// bind it to the port we passed in to getaddrinfo():

bind(sockfd, res->ai_addr, res->ai_addrlen);
*/

//By using the AI_PASSIVE flag, I’m telling the program to bind to the IP of the host it’s running on. If you want to bind to a specific local IP address, drop the AI_PASSIVE and put an IP address in for the first argument to getaddrinfo().

/*
Sometimes, you might notice, you try to rerun a server and bind() fails, claiming “Address already in use.” What does that mean? Well, a little bit of a socket that was connected is still hanging around in the kernel, and it’s hogging the port. You can either wait for it to clear (a minute or so), or add code to your program allowing it to reuse the port, like this:

int yes=1;

// lose the pesky "Address already in use" error message
if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
} */

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//connect()--hey, you!
/*
    #include <sys/types.h>
    #include <sys/socket.h>
    
    int connect(int sockfd, struct sockaddr *serv_addr, int addrlen); 

sockfd is our friendly neighborhood socket file descriptor, as returned by the socket() call, serv_addr is a struct sockaddr containing the destination port and IP address, and addrlen is the length in bytes of the server address structure.

struct addrinfo hints, *res;
int sockfd;

// first, load up address structs with getaddrinfo():

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

getaddrinfo("www.example.com", "3490", &hints, &res);

// make a socket:

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// connect!

connect(sockfd, res->ai_addr, res->ai_addrlen);

//remeber to check if connect diesnt return an error 
*/
//-----------------------------------------------------------------------------------------------------------------------------------------
//listen()

//if you want to bind or connect to a remote host and you just want to wait for incoming msgs then you need to 1. listen() 2. aceept()
/*
int listen(int sockfd, int backlog)
sockfd: file descriptor
backlog: number of connection allowed on incoming queue, silently limited to 20
return -1 on error
need to binf before you listen 

getaddrinfo()
socket();
bind();
listen();
accept();

//accept()
Get ready—the accept() call is kinda weird! What’s going to happen is this: someone far far away will try to connect() to your machine on a port that you are listen()ing on. Their connection will be queued up waiting to be accept()ed. You call accept() and you tell it to get the pending connection. It’ll return to you a brand new socket file descriptor to use for this single connection! That’s right, suddenly you have two socket file descriptors for the price of one! The original one is still listening for more new connections, and the newly created one is finally ready to send() and recv(). We’re there!
    #include <sys/types.h>
    #include <sys/socket.h>
    
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); 

//sockfd is the listen()ing socket descriptor. Easy enough. addr will usually be a pointer to a local struct sockaddr_storage. This is where the information about the incoming connection will go (and with it you can determine which host is calling you from which port). addrlen is a local integer variable that should be set to sizeof(struct sockaddr_storage) before its address is passed to accept(). accept() will not put more than that many bytes into addr. If it puts fewer in, it’ll change the value of addrlen to reflect that.

    getaddrinfo(NULL, MYPORT, &hints, &res);

    // make a socket, bind it, and listen on it:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    // now accept an incoming connection:

    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    // ready to communicate on socket descriptor new_fd!

*/

//--------------------------------------------------------------------------------------------------------------------------------
//send(), recv()
//for communication over connected datagrm/stream sockets
/*
int send(int sockfd, const void *msg, int len, int flags);

sockfd: socket descriptor you want tosend the data throw
msg: pointer to data you want to send
len: length of data un bytes
flags: set to zero

char * msg="hey yall you rock!!\n";
int len, bytes_sent;
len=strlen(msg);
bytes_sent=send(sockfd, msg, len, 0);

send returns the number of bytes actually sent, sontines it wont sen dthe complete daya and its upto you to send it compltely

int recv(int sockfd, void * buf, int len, int flags)
sockfd: read fromhere
buf: to read info into
len: maxlen of buffer
flags: ste to zero

returns the number of bytes actually read into buffer
when returns zero remote side closed the connection on you
*/
//sendto(), recvfrom()-dgram style
/*
    int sendto(int sockfd, const void *msg, int len, unsigned int flags,
               const struct sockaddr *to, socklen_t tolen); 

 a pointer to a struct sockaddr (which will probably be another struct sockaddr_in or struct sockaddr_in6 or struct sockaddr_storage that you cast at the last minute) which contains the destination IP address and port. tolen, an int deep-down, can simply be set to sizeof *to or sizeof(struct sockaddr_storage)

    int recvfrom(int sockfd, void *buf, int len, unsigned int flags,
                 struct sockaddr *from, int *fromlen); 
So, here’s a question: why do we use struct sockaddr_storage as the socket type? Why not struct sockaddr_in? Because, you see, we want to not tie ourselves down to IPv4 or IPv6. So we use the generic struct sockaddr_storage which we know will be big enough for either.

(So… here’s another question: why isn’t struct sockaddr itself big enough for any address? We even cast the general-purpose struct sockaddr_storage to the general-purpose struct sockaddr! Seems extraneous and redundant, huh. The answer is, it just isn’t big enough, and I’d guess that changing it at this point would be Problematic. So they made a new one.)
*/
/*
gethostname()—Who am I?
#include <unistd.h>
        #include <sys/socket.h>
    
    int getpeername(int sockfd, struct sockaddr *addr, int *addrlen); 

sockfd is the descriptor of the connected stream socket, addr is a pointer to a struct sockaddr (or a struct sockaddr_in) that will hold the information about the other side of the connection, and addrlen is a pointer to an int, that should be initialized to sizeof *addr or sizeof(struct sockaddr).
int gethostname(char *hostname, size_t size); 

The arguments are simple: hostname is a pointer to an array of chars that will contain the hostname upon the function’s return, and size is the length in bytes of the hostname array.


getpeername()—Who are you?
    #include <sys/socket.h>
    
    int getpeername(int sockfd, struct sockaddr *addr, int *addrlen); 

sockfd is the descriptor of the connected stream socket, addr is a pointer to a struct sockaddr (or a struct sockaddr_in) that will hold the information about the other side of the connection, and addrlen is a pointer to an int, that should be initialized to sizeof *addr or sizeof(struct sockaddr).
*/







