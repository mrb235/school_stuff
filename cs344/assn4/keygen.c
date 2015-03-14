#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

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

    for(i = 0; i < length; ++i) {
        random = rand() % 27;
        if(random == 0) {
            c = 32;
        }
        else {
            c = random + 64;
        }
        temp[0] = c;
        strcat(key, temp);
    }
    printf("%s", key);

    return 0;
}
