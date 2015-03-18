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
    char *text = malloc(100000);
    fgets(text, 100000, fp);
    fclose(fp);
    return text;
}

//This function checks that the correct process is connecting to the correct server
//Both decrypt functions need to receive 20 and both encrypt functions need to receive 10
void verify_encrypt(int sockfd) {
    int n;
    int size = 20;
    int size_verify;
    n = write(sockfd, &size, sizeof(int));
    if (n < 0) {
        error("ERROR writing encrypt verify variagble to socket");
    }
         
    n = read(sockfd, &size_verify, sizeof(int));
    if (n < 0)
        error("ERROR reading encrypt variable from socket");

    if(size_verify != 20){
        error("Didn't receive correct socket address for decrypt module\n");
    }

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


    char *file_buffer = malloc(150000);   //massive initial buffer that will work
    strcpy(file_buffer, "");
    file_buffer = get_file(argv[1]);
    char *file_key = malloc(75000);
    strcpy(file_key, "");
    file_key = get_file(argv[2]);

    file_error_check(file_buffer, file_key);    //error check that data
    
    int buffer_initial_size = strlen(file_buffer) + strlen(file_key) + 1;     //now proper sized buffers
    char buffer[buffer_initial_size];
    strcpy(buffer, file_buffer);
    strcat(buffer, "|");    //This combines the text and key into one string
    strcat(buffer, file_key);    //I did this to simplify passing the info to the server process

    free(file_buffer);    //get rid of those massive buffers
    free(file_key);

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

    int size = strlen(buffer);
    int size_verify = 0;

    verify_encrypt(sockfd);
 
    n = write(sockfd, &size, sizeof(size));       //write out how large the buffer will be
    if (n < 0)
        error("ERROR writing sizeof to socket");

    n = read(sockfd, &size_verify, sizeof(size_verify));  //verify the server got it
    if (n < 0)
        error("ERROR reading sizeof verify from socket");

    if(size_verify != size) {
        error("size and size_verify aren't equal");
    }

    n = write(sockfd, buffer, strlen(buffer));      //write it if successful
    if (n < 0)
        error("ERROR writing to socket");

    size = 0;
     
    n = read(sockfd, &size, sizeof(int));           //wait for server to decrypt then read results
    if(n < 0)
        error("ERROR reading sizeof from socket post decrypt failed");
                 
    n = write(sockfd, &size, sizeof(int));          //we're sending size back and forth again
    if(n < 0)
        error("ERROR writing sizeof to socket post decrypt");
                 
    strcpy(buffer, "");
    memset(buffer, 0, buffer_initial_size);
    n = 0;
    int read_amount = 0;
    do {                                                          //read using do-while loop
        n = read(sockfd, buffer + read_amount, size - 1);         //gets all large and small files
        if(n < 0)
            error("ERROR reading buffer from socket post decrypt");
        read_amount += n;
                 
    }while(read_amount < size);


    printf("%s",buffer);      //print out results
    close(sockfd);
    
    return 0;
}
