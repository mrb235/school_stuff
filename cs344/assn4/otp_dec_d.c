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

int bad_chars_check(char *str) {
    int i;
    char c;
    for (i = 0; i < strlen(str); ++i){
        c = str[i];
        if(c > 90 || c < 65){
            if(c != 32 && c != 10 && c != 0) {
                fprintf(stderr, "Invalid character ( '%c' num: %d ) detected at location ( %d )", c, c, i);
                return 1;
            }
        }
    }
    return 0;
}

int data_error_check(char *text, char *key) {
    int n = 0;
    if(strlen(text) > strlen(key)){
        fprintf(stderr,"Key is shorter than text.\n");
        return 1;
    }
    n = bad_chars_check(text);
    if(n == 1)
        return n;
    n = bad_chars_check(key);
    return n;
}

int char_num(char c){
    if(c == ' ')
        return 0;
    else{
        return c - 64;
    }
}

char get_encrypted_char(char t, char k) {
    int t_int = char_num(t);
    int k_int = char_num(k);
    int result = (t_int - k_int) % 27;
    if(result < 0) 
        result += 27;

    if(result == 0) {
        return ' ';
    }
    if(t_int < 0 || k_int < 0)
        return '\0';
    else
        return '\0' + result + 64;
}

char* encrypt(char *text, char *key) {
    char temp[2] = " ";
    char *encrypted_text = malloc(100000);
    int i = 0;
    strcpy(encrypted_text, "");
    for (i = 0; i < strlen(text); ++i) {
        temp[0] = get_encrypted_char(text[i], key[i]);
        strcat(encrypted_text, temp);
    }
    return encrypted_text;
}

void verify_encrypt(int newsockfd) {

    int n;
    int size;
    n = read(newsockfd, &size, sizeof(int));
    if (n < 0) {
        fprintf(stderr, "newsockfd: %d n: %d\n", newsockfd, n);
        error("ERROR reading encrypt verify variagble from socket");
    }

    int verify_size = 20;
         
    n = write(newsockfd, &verify_size, sizeof(int));
    if (n < 0)
        error("ERROR writing encrypt variable to socket");

    if(verify_size != size)
        _exit(1);
         
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
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

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
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

//    fprintf(stderr, "newsockfd: %d", newsockfd);
//    read_stuff(newsockfd, buffer);
//   int n;
    int size = 0;
    int read_amount = 0;

    verify_encrypt(newsockfd);

    n = read(newsockfd, &size, sizeof(int));
    if (n < 0) {
        fprintf(stderr, "newsockfd: %d n: %d\n", newsockfd, n);
        error("ERROR reading sizeof from socket");
    }
    
    n = write(newsockfd, &size, sizeof(int));
    if (n < 0)
        error("ERROR writing sizeof to socket");

    char buffer[size + 2];
    strcpy(buffer, "");
    memset(buffer, 0, size + 2);
    n = 0;
    do {
        n = read(newsockfd, buffer + read_amount, size - 1);
        if (n < 0)
            error("ERROR reading buffer from socket");
            read_amount += n;
    
    } while(read_amount < size);


    char *text = malloc((size / 2) + 1);
    char *key = malloc((size / 2) + 1);
    char *temp = malloc((size / 2) + 1);
    strcpy(text, "");
    strcpy(key, "");
    strcpy(temp, "");
    char *delim = "|";
    text = strtok(buffer, delim);
    temp = strtok(NULL, delim);
    strncpy(key, temp, strlen(text));
    char *encrypted_text = malloc((size / 2) + 1);
    strcpy(encrypted_text, "");

    int error_check = data_error_check(text, key);
    if (error_check == 0) {
        encrypted_text = encrypt(text, key);
    }
        //do more stuff
//ADD THIS STUFF NEXT
//IT SHOULD CHECK CHARACTERS AND DO THE SAME THING AS CLIENT
/*
   */ 

    //ONLY WRITE IF DATA IS GOOD ALWAYS CLOSE THE SOCKET

    int size_verify = 0;
    size = strlen(encrypted_text);
     
    n = write(newsockfd, &size, sizeof(size));
    if(n < 0)
        error("ERROR writing sizeof for decrpyted to socket");
    
    n = read(newsockfd, &size_verify, sizeof(size_verify));
    if(n < 0)
        error("Error reading sizeof verify from socket post encrypt");
    
    if(size != size_verify)
        error("size and size_verify post encrypte aren't equal");

    n = write(newsockfd, encrypted_text, size);
    if (n < 0) 
        fprintf(stderr, "ERROR writing to socket");

    close(newsockfd);
    close(sockfd);
    return 0; 
    
}
