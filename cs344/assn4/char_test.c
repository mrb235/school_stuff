#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {

    char c = 'A';
    int array[128];
    int i;

    for(i = 0; i < 128; ++i) {
        array[i] = i;
    }

    printf("char[%c]: %d\n", c, array[c]);

    return 0;
}
