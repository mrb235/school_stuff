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

struct game_room
{
    char name[10];
    char type[15];
    char doors[7][10];
    int num_doors;
};

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
        strcat(room_type, "\n");
        fputs(room_type, fp);
        fclose(fp);
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

void game_room_name(struct game_room *room, char line[40]) {
/*    int begin = 11;
    int end = strlen(line) - 1;
    char temp[10];

    strncpy(temp, line + begin, end - begin);

    printf("%s ", temp);
    room->name = temp;*/
    char temp[10];
}

void get_line_data(struct game_room *room, char line[40]) {
/*    printf("%s ", room->name);
    char temp[10];
    
    if(line[5] == 'N') {
    }*/
    char temp[10];
    
    if (line[5] == 'N') {
        strncpy(temp, line + 11, strlen(line) - 12);
        strcpy(room->name, temp);
    }
    else if (line[5] == 'C') {
        strncpy(temp, line + 14, strlen(line) - 15);
        strcpy(room->doors[room->num_doors], temp);
        room->num_doors++;
    }
    else if (line[5] == 'T') {
        strncpy(temp, line + 11, strlen(line) - 17);
        strcpy(room->type, temp);
        strcat(room->type, "_ROOM");
    }
    
}

void game_room_initialize(struct game_room game_rooms[7]) {
    int i;
    
    char file_name[40] = "boalm.adventure.";
    char temp_file[40];
    char pid_str[10];
    sprintf(pid_str, "%d", getpid());
    strcat(file_name, pid_str);
    strcat(file_name, "/room");

    for (i = 0; i < 7; ++i) {
        game_rooms[i].num_doors = 0;
        FILE *fp;
        strcpy(temp_file, file_name);
        char num[2];
        sprintf(num, "%d", i);
        strcat(temp_file, num);

        fp = fopen(temp_file, "r");
        char line[40];
        while(fgets(line, 40, fp)) {
            get_line_data(&game_rooms[i], line);
        }

        fclose(fp);
    }
}

int valid_room(char usr_input[15], struct game_room game_rooms[7], int curr_room) {
    int i;
    char temp[15];
    for (i = 0; i < game_rooms[curr_room].num_doors; i++) {
        strcpy(temp, game_rooms[curr_room].doors[i]);
        strcat(temp, "\n");
        if(strcmp(temp, usr_input) == 0){
            int j;
            for (j = 0; j < 7; j++) {
                char room_name[15];
                strcpy(room_name, game_rooms[j].name);
                strcat(room_name, "\n");
                if (strcmp(usr_input, room_name) == 0) {
                    return j;
                }
            }
        }
    } 
    return -1;
}


void the_game() {
    struct game_room game_rooms[7];
    game_room_initialize(game_rooms);

    char curr_loc[30] = "CURRENT LOCATION: ";
    char poss_conn[80] = "POSSIBLE CONNECTIONS: ";
    char where_to[30] = "WHERE TO? >";
    int curr_room = 0;

    while(game_rooms[curr_room].type != "END_ROOM") {
        printf("%s",curr_loc);
        printf("%s\n", game_rooms[curr_room].name);
        printf("%s", poss_conn);
        int i;
        for (i = 0; i < game_rooms[curr_room].num_doors - 1; ++i) {
            printf("%s, ", game_rooms[curr_room].doors[i]);
        }
        printf("%s.\n", game_rooms[curr_room].doors[i]);
        printf("%s", where_to);
        
        char usr_input[15];
        fgets(usr_input, 15, stdin);

        int maybe_success = valid_room(usr_input, game_rooms, curr_room);
        
        if(maybe_success == -1) {
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
            continue;
        }
        else{
            printf("success\n");
            curr_room = maybe_success;
        }
        if(maybe_success == 1) {
            break;
        }
    }

//    printf("%s",game_rooms[7].name);
/*    printf("%s\n", game_rooms[0].name);
    int i;
    for ( i = 0; i < 7; ++i) {
        printf("%s ", game_rooms[0].doors[i]);
    }

    char line[80];
    FILE *fp;
//    char file_name[10] = "room1";
    char file_name[40] = "boalm.adventure.";
    char pid_str[10];
    sprintf(pid_str, "%d", getpid());
    strcat(file_name, pid_str);
    strcat(file_name, "/room1");

    fp = fopen(file_name, "r");
    while(fgets(line, 80, fp)) {
        printf("%s %s", file_name, line);
    }*/
}

int main() {
    
    srand(time(NULL));
    make_rooms();
    intro();
    printf("got here");
    the_game();
    
    printf("\n");

    return 0;
}
