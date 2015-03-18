#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

//accepts an argument of how many random numbers to generate for a key
//the results are just output to stdout
int main(int argc, char *argv[]) {
    srand(time(NULL));          //random numbers

    if(argc == 1) {
        fprintf(stderr, "Not enough arguments\n");
        _exit(1);
    }
    else if(argc >= 3) {
        fprintf(stderr, "Too many arguments\n");
        _exit(1);
    }

    int length = atoi(argv[1]);
    char *key = malloc(length * sizeof(char));
    strcpy(key, "");
    char temp[2] = "A";
    char c = 67;
    int random;
    int i;

    for(i = 0; i < length; ++i) { //iterate over given size
        random = rand() % 27;     //make random number between 0-26
        if(random == 0) {
            c = 32;               //0 = ' '
        }
        else {
            c = random + 64;      //add 64 to each other number to get its
        }                         //ascii value
        temp[0] = c;
        strcat(key, temp);        //add that to the end of the string
    }
    printf("%s", key);

    return 0;
}
