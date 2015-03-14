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
    exit(2);
}

//
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

//accepts a string as input
//checks each character in the string to see if it's a capital letter or space
//Any non-valid character will result in error output and exit with value 1
void bad_chars_check(char *str){
    int i;
    char c;
    for(i = 0; i < strlen(str); ++i){
        c = str[i];
        if(c > 90 || c < 65){
           if(c != 32 && c != 10 && c != 0) {   //I include 10(line feed) and 0(NULL) because the test
                                                //files contain 10.

               fprintf(stderr,"Invalid character ( '%c' num: %d ) detected at location ( %d )\n", c, c, i);
               _exit(1);
           }
        }
    }
}

//Accepts two strings as input
//checks to make sure the key is longer or equal to buffer
//then runs them individually through bad_chars_check()
void file_error_check(char *buffer, char *key) {
    if(strlen(buffer) > strlen(key)){
        fprintf(stderr,"Key is shorter than text.\n");
        _exit(1);
    }
    bad_chars_check(buffer);
    bad_chars_check(key);
}

//This grabs the first line of a file and returns it as a character string pointer
//accepts a filename as a parameter and returns the first line as a string.
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


    char *buffer;
    buffer = get_file(argv[1]);
    char *key;
    key = get_file(argv[2]);

    file_error_check(buffer, key);

    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket\n");
        _exit(1);
    }
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
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
    memset(buffer, 0, 256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);

    
    printf("buffer: %s\n",buffer);
    printf("key: %s\n",key);

    return 0;
}
