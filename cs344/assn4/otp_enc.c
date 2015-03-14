#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void valid_file_error_checking(char *arg1, char *arg2) {
    if(access(arg1, R_OK) == -1) {
        fprintf(stderr,"1st argument isn't a valid file for reading\n");
        _exit(1);
    }

    if(access(arg2, R_OK) == -1) {
        fprintf(stderr,"2nd argument isn't a valid file for reading\n");
        _exit(1);
    }
}

char* get_file(char *file) {
    FILE *fp;
    fp = fopen(file, "r");
    char *text = malloc(256);
    fgets(text, 256, fp);
    fclose(fp);
    return text;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if(argc < 4) {
        fprintf(stderr,"Incorrect number of arguments.  (HINT - USE at least 3!!)\n");
        _exit(1);
    }

    valid_file_error_checking(argv[1], argv[2]);


    char *buffer = malloc(256);
    buffer = get_file(argv[1]);
    char *key = malloc(256);
    key = get_file(argv[2]);
    


    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);

    
    printf("buffer: %s\n",buffer);
    printf("key: %s\n",key);

    return 0;
}
