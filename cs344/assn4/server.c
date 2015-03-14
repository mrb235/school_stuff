/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}



int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     
     pid_t forked_pid = -4;
     int status = -30;

     int loop = 0;
         
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     while(loop == 0){
         listen(sockfd,5);
         clilen = sizeof(cli_addr);
         newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);
         if (newsockfd < 0) 
              error("ERROR on accept");

         forked_pid = fork();

         switch(forked_pid) {
             case -1:
                 fprintf(stderr, "something odd happened\n");
                 _exit(1);
                 break;
             case 0:    //child
                 loop = 1;
                 break;
             default:   //parent
                 break;
         }
     }

     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);

     char message[255] = "This is your message: ";
     strcat(message, buffer);


     n = write(newsockfd, message,255);
     if (n < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     _exit(0);
     return 0; 
}
