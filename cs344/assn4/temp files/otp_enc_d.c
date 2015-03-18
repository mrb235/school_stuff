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
int data_error_check(char *text, char *key) {
    int n = 0;
    if(strlen(text) > strlen(key)){
        fprintf(stderr,"Key ( %d ) is shorter than text ( %d ).\n", strlen(key), strlen(text));
        return 1;
    }
    bad_chars_check(text);
    if(n == 1)
        return n;
    bad_chars_check(key);
    return n;
}

//change ascii value to numbers between 0 and 26
int char_num(char c){
    if(c == ' ')
        return 0;
    else{
        return c - 64;
    }
}

//This function takes two characters as arguments, one from the text string and
//one from the key string.  It returns the encrypted result
char get_encrypted_char(char t, char k) {
    int t_int = char_num(t);  //This converts a character to a number between 0-26
    int k_int = char_num(k);
    int result = (t_int + k_int) % 27;     //get modulus 27 of addition
    if(result == 0) {                      //0 = ' '
        return ' ';
    }
    if(t_int < 0 || k_int < 0)
        return '\0';
    else
        return '\0' + result + 64;         //This math results in the character we want
}                                          //1 = 'A' 2 = 'B' etc.

//This function accepts the text and key strings and returns an encrypted version
//of the text string using the key to do so.
char* encrypt(char *text, char *key) {
    char temp[2] = " ";
    char *encrypted_text = malloc(75000);
    int i = 0;
    strcpy(encrypted_text, "");
    for (i = 0; i < strlen(text); ++i) {               //Here we iterate over every
        temp[0] = get_encrypted_char(text[i], key[i]); //character in the text string
        strcat(encrypted_text, temp);                  //we get the encrypted char and
    }                                                  //add that onto the end of
    return encrypted_text;                             //the encrypted string
}

//This function checks that the correct process is connecting to the correct server
//Both decrypt functions need to receive 20 and both encrypt functions need to receive 10
void verify_encrypt(int newsockfd) {
    int n;
    int size;
    n = read(newsockfd, &size, sizeof(int));
    if (n < 0) {
        fprintf(stderr, "newsockfd: %d n: %d\n", newsockfd, n);
        error("ERROR reading encrypt verify variagble from socket");
    }

    
    int verify_size = 10;

    n = write(newsockfd, &verify_size, sizeof(int));
    if (n < 0)
        error("ERROR writing encrypt variable to socket");
    
    if(size != verify_size) {
        _exit(1);               //exit is safe, since we've forked by now
    }

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
        fprintf(stderr,"ERROR, no port provided\n");  //make sure we have a port argument
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
             
    while(loop == 0){                  //This part of the network setup goes in a while loop
        listen(sockfd,5);              //it will wait on listen for new connections
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                    (struct sockaddr *) &cli_addr,
                    &clilen);
        if (newsockfd < 0)
             error("ERROR on accept");

        forked_pid = fork();           //We fork off the process when a connection is accepted
        
        switch(forked_pid) {
            case -1:
                fprintf(stderr, "something odd happened\n");
                _exit(1);
                break;
            case 0:    //child
                loop = 1;       //Keep doing work below
                break;
            default:   //parent
                break;          //Keep looping and go back to listening
        }
    }

    int size = 0;
    int read_amount = 0;

    verify_encrypt(newsockfd);      //make sure the correct process is connected to the server

    n = read(newsockfd, &size, sizeof(int));        //The process will tell us how big the incoming file is
    if (n < 0) {
        fprintf(stderr, "newsockfd: %d n: %d\n", newsockfd, n);
        error("ERROR reading sizeof from socket");
    }

    n = write(newsockfd, &size, sizeof(int));     //write back the same information
    if (n < 0)                                    //if they're equal the process will send the info
        error("ERROR writing sizeof to socket");
        
    char buffer[size + 2];          //setup buffer string with proper sizing
    strcpy(buffer, "");
    memset(buffer, 0, size + 2);
    n = 0;
    do {                                                      //read in a do-while loop
        n = read(newsockfd, buffer + read_amount, size - 1);  //this allows us to get info
        if (n < 0)                                            //if the packets are too big
            error("ERROR reading buffer from socket");        //and it can't be sent in one group
        read_amount += n;

    } while(read_amount < size);


    char *text = malloc((size / 2) + 1);      //make strings which will hold the individual parts
    char *key = malloc((size / 2) + 1);
    char *temp = malloc((size / 2) + 1);
    strcpy(text, "");
    strcpy(key, "");
    strcpy(temp, "");
    char *delim = "|";
    text = strtok(buffer, delim);             //grab tokens and place into those strings
    temp = strtok(NULL, delim);               //key goes in temp string
    strncpy(key, temp, strlen(text));         //temp is trimmed to proper size and put into text string
    char *encrypted_text = malloc((size / 2) + 1);
    strcpy(encrypted_text, "");


    int error_check = data_error_check(text, key);  //decrypt those strings
    if (error_check == 0) {
        encrypted_text = encrypt(text, key);
    }

    int size_verify = 0;
    size = strlen(encrypted_text);

    n = write(newsockfd, &size, sizeof(size));                      //same write process in reverse
    if(n < 0)                                                       //to send the info back to the process
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
