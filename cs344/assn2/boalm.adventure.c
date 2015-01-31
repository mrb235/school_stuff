#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct room
{
    char *name;
    char *type;
    int doors[7];
    int num_doors;
};

const struct room room_default = {"", "", {20, 20, 20, 20, 20, 20, 20}, 0};
//This creates default values for the room struct.
//I'm using 20 for the doors since they're out of range and are easy to see

void intro() {
    printf("Welcome to the room game!\n");
    printf("My processID: %d\n", getpid());
    printf("More to come :)\n");
}

int num_of_doors() {
    int temp = rand() % 4;
    return temp + 3;
}

//This generates a random number between 0 and 6 inclusive that isn't already used
//An integer is passed, which is invalid
//returns a valid int
int rand_seven(int n) {
    int invalid_num = 0;
    int temp;
    while (invalid_num == 0) {
        invalid_num = 1;
        temp = rand() % 7;
        if (temp == n) {
            invalid_num = 0;
        }
    }
    return temp;
}

int unused_rand(int nums[7]) {
    int invalid_num = 0;
    int temp_num;
    int i;

    while(invalid_num == 0) {
        invalid_num = 1;
        temp_num = rand() % 10;
        for (i = 0; i < 7; ++i) {
            if (nums[i] == temp_num) {
                invalid_num = 0;
                break;
            }
        }
    }

    return temp_num;
}

void make_dir() {
    int pid = getpid();
    char pidstr[10];
    sprintf(pidstr, "%d", pid);
    
    char file[80];

    strcpy(file, "boalm.adventure.");
    strcat(file, pidstr);
     
    mkdir(file, S_IRUSR | S_IWUSR | S_IXUSR);
}

void make_connection (struct room *room, int num) {
    int the_door;
    int i;
    for (i = 0; i < 7; ++i) {
        if(room->doors[i] > 10) {
            the_door = i;
            room->doors[i] = num;
            if (room->num_doors <= i) {
                room->num_doors = i + 1;
            }
            break;
        }
    }
} 

void write_files(struct room rooms[7]) {
    make_dir();


    int f;
    for (f = 0; f < 7; f++) {
        char r[6] = "/room";
        char r_num[2];
        sprintf(r_num, "%d", f); 
        strcat(r, r_num);

        char file_name[40] = "boalm.adventure.";
        char pid_str[10];
        sprintf(pid_str, "%d", getpid());
        strcat(file_name, pid_str);
        strcat(file_name, r);

        FILE *fp;
        fp = fopen(file_name, "w+");
        char room_name[30] = "ROOM NAME: ";
        strcat(room_name, rooms[f].name);
        strcat(room_name, "\n");
        fputs(room_name, fp);
        int i;
        for (i = 0; i < rooms[f].num_doors; ++i) {
            char room_connect[30] = "CONNECTION ";
            char room_num[2];
            sprintf(room_num, "%d", i + 1);
            strcat(room_connect, room_num);
            strcat(room_connect, ": ");
            strcat(room_connect, rooms[rooms[f].doors[i]].name);
            strcat(room_connect, "\n");
            fputs(room_connect, fp);
        }
        char room_type[30] = "ROOM TYPE: ";
        strcat(room_type, rooms[f].type);
        fputs(room_type, fp);
    }
}
void testing() {
   /* 
    strcat(file, "/room1");

    printf("%s\n", folder);

    FILE *fp;

    fp = fopen(file, "w+");
    fputs("testing\n", fp);
    fputs("This should be on the second line\n", fp);
    fclose(fp);
    */
}

void write_rooms(int rand_nums[7], char *room_names[10]) {
/*    struct room room1 = room_default;
    printf("%d %s\n", room1.name, room_names[rand_nums[3]]);

    room1.name = room_names[rand_nums[1]];
    printf("%s\n", room1.name);
*/
    struct room rooms[7];

    int i;
    int j;
    int k;
    for (i = 0; i < 7; ++i) {
        rooms[i] = room_default;
        rooms[i].name = room_names[rand_nums[i]];
        rooms[i].num_doors = num_of_doors();
        printf("%s%d\n",rooms[i].name, rooms[i].num_doors);
    }

    for (i = 0; i < 7; ++i) {
        int first_unused_door;
        int n;
        for (n = 0; n < 7; ++n) {
            if (rooms[i].doors[n] > 10) {
                first_unused_door = n;
                break;
            }
        }
        for (j = first_unused_door; j < rooms[i].num_doors; ++j){
            int invalid_num = 0;
            int temp;
            while (invalid_num == 0) {
                invalid_num = 1;
                temp = rand_seven(i);
                for (k = 0; k < rooms[i].num_doors; ++k) {
                    if (temp == rooms[i].doors[k]) {
                        invalid_num = 0;
                        break;
                    }
                }
            }
            rooms[i].doors[j] = temp;
            make_connection(&rooms[temp], i);
/*            int r;
            int s;
            for (r = 0; r < 7; ++r) {
                for (s = 0; s < 7; ++s) {
                    printf("%d ", rooms[r].doors[s]);
                }

                printf("%d\n", rooms[r].num_doors);
            }
            printf("\n");*/
        }
    }
    int m;
    int p;
    for(m = 0; m < 7; m++) {
        for (p = 0; p < rooms[m].num_doors; p++){
            printf("%d ", rooms[m].doors[p]);
        }
        printf("num_doors: %d\n", rooms[m].num_doors);
    }

    rooms[0].type = "START_ROOM";
    rooms[1].type = "END_ROOM";
    int t;
    for (t = 2; t < 7; ++t) {
        rooms[t].type = "MID_ROOM";
    }

    write_files(rooms);

}


void make_rooms() {

    char *room_names[10];
    int i;
    int rand_nums[7];
    for (i = 0; i < 7; ++i) {
        rand_nums[i] = 20;
    }
    for (i = 0; i < 7; ++i) {
        rand_nums[i] = unused_rand(rand_nums);
    }
 
    room_names[0] = "Sunny";
    room_names[1] = "Gloomy";
    room_names[2] = "Cloudy";
    room_names[3] = "Snowy";
    room_names[4] = "Rainy";
    room_names[5] = "Foggy";
    room_names[6] = "Misty";
    room_names[7] = "Hazy";
    room_names[8] = "Icy";
    room_names[9] = "Ominous";

    write_rooms(rand_nums, room_names);

/*    printf("%s\n",room_names[rand_nums[0]]);
    for (i = 0; i < 7; ++i) {
        printf("%d", rand_nums[i]);
    }
    printf("\n");
   */ 
}

int main() {
    
    srand(time(NULL));
//    testing();
    make_rooms();
    intro();

    return 0;
}
