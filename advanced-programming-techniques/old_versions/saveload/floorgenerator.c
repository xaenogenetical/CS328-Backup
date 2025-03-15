#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "dungeonutil.h"

#define FLOOR '.'
#define CORRIDOR '#'
#define ROCK ' '
#define DWNSTAIR '>'
#define UPSTAIR '<'

char level[21][80];

char **rooms[21 * 80]; // stores pointers to each room created by place_rooms()

dungeon lev;
coordinate centerpoints[100];

int16_t rooms_added = 0;

coordinate connected[10]; // this is made of coordinates because i can't be bothered to write a whole new struct for the same purpose
int connd = 0;

coordinate player_loc;

bool linked[100];

int up_down = 0;
int left_right = 0;
int generator_num_up;
int generator_num_dwn;

void draw_dungeon();
bool place_rooms();
void add_stairs();
bool link_rooms();
void fill_with_rock();
void print_floor();
void set_hardness();

void draw_dungeon()
{
    srand((unsigned)time(NULL));
    coordinate placeholder;
    placeholder.x = 255; // used to connect the first two rooms when connecting because connected would be empty otherwise
    connected[0] = placeholder;
    /*for (int i = 0; i < 21; i++)
    {
        level[i][0] = '|';
        level[i][79] = '|';
    }
    for (int i = 0; i < 79; i++)
    {
        level[0][i] = '-';
        level[20][i] = '-';
    }*/
    // ^ for debugging
    set_hardness();
    if (!place_rooms())
    {
        fprintf(stderr, "Program was unable to place a sufficient quantity of random rooms. Please execute again.");
    }
    fill_with_rock();
    link_rooms();
    add_stairs();
    player_loc.x = 0;
    player_loc.y = 0;
    print_floor();
    lev.player.x = player_loc.x;
    lev.player.y = player_loc.y;
    // printf("MAKE IT TO RETURN!!\n");
    return;
}

void draw_dungeon_from_file()
{
    fill_with_rock();
    // Draw rooms
    for (int r = 0; r < lev.num_rooms; r++)
    {
        int start_x = lev.rooms[r].start.x;
        int start_y = lev.rooms[r].start.y;
        int wid = lev.rooms[r].width;
        int high = lev.rooms[r].height;
        // printf("X:%d,Y:%d,W:%d,H:%d\n", start_x, start_y, wid, high);
        for (int row = 0; row < high; row++)
        {
            for (int col = 0; col < wid; col++)
            {
                level[row + start_y][col + start_x] = FLOOR;
            }
        }
    }
    // Add corridors
    for (int row = 0; row < 21; row++)
    {
        for (int col = 0; col < 80; col++)
        {
            // printf("HARDNESS: %d, EXISTING: %c\n", lev.hardness_map[row][col], level[row][col]);
            if (lev.hardness_map[row][col] == 0 && level[row][col] != FLOOR)
            {
                level[row][col] = CORRIDOR;
            }
        }
    }
    // Add stairs
    for (int u = 0; u < lev.num_up_stair; u++)
    {
        int x = lev.upstairs[u].x;
        int y = lev.upstairs[u].y;
        level[y][x] = UPSTAIR;
    }
    for (int d = 0; d < lev.num_up_stair; d++)
    {
        int x = lev.downstairs[d].x;
        int y = lev.downstairs[d].y;
        level[y][x] = DWNSTAIR;
    }
    print_floor();
    return;
}

void set_hardness()
{
    for (int row = 0; row < 21; row++)
    {
        if (row == 0 || row == 20)
        {
            for (int col = 0; col < 80; col++)
            {
                lev.hardness_map[row][col] = 255;
            }
        }
        else
        {
            for (int col = 0; col < 80; col++)
            {
                lev.hardness_map[row][col] = 10;
            }
            lev.hardness_map[row][0] = 255;
            lev.hardness_map[row][79] = 255;
        }
    }
}

bool place_rooms()
{
    int attempts = 0;
    int taken_spaces = 0;
    uint8_t rand_row;
    uint8_t rand_col;
    uint8_t width;
    uint8_t height;
    coordinate room_center;
    room *room_holder = (room *)malloc(sizeof(room) * 15);
    char **roomy = malloc(sizeof(room *) * 10); // array of pointers to the beginning of each row (a spot in level[][])
    while ((double)taken_spaces / (80 * 21) <= 0.07 || rooms_added < 6)
    {
    loop:
        attempts++;
        if (attempts > 2000)
        {
            // printf("returning false\n");
            return false;
        }
        rand_row = rand() % 21;
        rand_col = rand() % 80;
        width = (rand() & 6) + 4;
        height = (rand() % 7) + 3;

        // printf("Attempting to place a room with width %d and height %d at row %d, col %d.\n", width, height, rand_row, rand_col);
        if (rand_row + height > 20 || rand_col + width > 79 || rand_row == 0 || rand_col == 0)
        {
            // printf("was determined to be out-of-bounds\n");
            continue;
        }
        // printf("Was determined to be in-bounds\n");
        for (int i = -1; i < height + 1 && i < 21; i++)
        {
            for (int j = -1; j < width + 1 && i < 80; j++)
            {
                if (level[i + rand_row][j + rand_col] != '\0')
                {
                    // printf("area was not clear \n");
                    goto loop;
                }
            }
        }
        // printf("area clear, room should place.\n");
        for (int i = 0; i < height; i++)
        {
            roomy[i] = &level[i + rand_row][rand_col];
        }
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                roomy[i][j] = FLOOR;
                lev.hardness_map[i + rand_row][j + rand_col] = 0;
            }
        }
        taken_spaces += width * height;
        rooms[rooms_added] = roomy;
        room_center.x = rand_col + (width / 2);
        // printf("roomc x: %d\n", room_center.x);
        room_center.y = rand_row + (height / 2);
        // printf("roomc y: %d\n", room_center.y);
        centerpoints[rooms_added] = room_center;
        linked[rooms_added] = false;
        room_holder[rooms_added].start.x = rand_col;
        room_holder[rooms_added].start.y = rand_row;
        room_holder[rooms_added].width = width;
        room_holder[rooms_added].height = height;
        rooms_added++;
        // printf("Area opened: %f\n", (double)taken_spaces / (80 * 21));
    }
    lev.rooms = (room *)malloc(sizeof(room) * rooms_added);
    memcpy(lev.rooms, room_holder, sizeof(room) * rooms_added);
    lev.num_rooms = rooms_added;
    free(roomy);
    free(room_holder);
    return true;
}

void add_stairs()
{
    generator_num_up = rand() % 3;
    generator_num_dwn = rand() % 3;
    int8_t up_placed = 0;
    int8_t dwn_placed = 0;
    int rand_row;
    int rand_col;
    coordinate *generator_upstairs = (coordinate *)malloc(sizeof(coordinate) * 4);
    coordinate *generator_downstairs = (coordinate *)malloc(sizeof(coordinate) * 4);
    while (up_placed < generator_num_up + 1)
    {
        rand_row = rand() % 21;
        rand_col = rand() % 80;
        if (level[rand_row][rand_col] == CORRIDOR || level[rand_row][rand_col] == FLOOR)
        {
            level[rand_row][rand_col] = UPSTAIR;
            coordinate loc;
            loc.y = rand_row;
            loc.x = rand_col;
            generator_upstairs[up_placed] = loc;
            up_placed++;
        }
    }
    while (dwn_placed < generator_num_dwn + 1)
    {
        rand_row = rand() % 21;
        rand_col = rand() % 80;
        if (level[rand_row][rand_col] == CORRIDOR || level[rand_row][rand_col] == FLOOR)
        {
            // printf("valat2: %c\n", level[rand_row][rand_col]);
            level[rand_row][rand_col] = DWNSTAIR;
            coordinate loc;
            loc.y = rand_row;
            loc.x = rand_col;
            generator_downstairs[lev.num_dn_stair] = loc;
            dwn_placed++;
        }
    }
    lev.upstairs = (coordinate *)malloc(sizeof(coordinate) * up_placed);
    lev.downstairs = (coordinate *)malloc(sizeof(coordinate) * dwn_placed);
    memcpy(lev.upstairs, generator_upstairs, sizeof(coordinate) * up_placed);
    lev.num_up_stair = up_placed;
    memcpy(lev.downstairs, generator_downstairs, sizeof(coordinate) * dwn_placed);
    lev.num_dn_stair = dwn_placed;
    free(generator_downstairs);
    free(generator_upstairs);
    return;
}

void calc_direction(coordinate current, coordinate goal)
{
    up_down = 0;
    left_right = 0;
    if (current.x < goal.x)
    {
        left_right = 1;
    }
    else if (current.x > goal.x)
    {
        left_right = -1;
    }
    if (current.y > goal.y)
    {
        up_down = -1;
    }
    else if (current.y < goal.y)
    {
        up_down = 1;
    }
}

void link_a_to_b(int a_index, int b_index)
{
    bool escaped = false;
    // printf("a ind: %d, b ind: %d, rooms added: %d\n", a_index, b_index, rooms_added);
    if (a_index == b_index)
    {
        return;
    }
    coordinate comparer;
    comparer.x = a_index;
    comparer.y = b_index;
    for (int i = 0; i < connd; i++)
    {
        if (comparer.x == connected[i].y && comparer.y == connected[i].x)
        {
            return;
        }
    }
    coordinate a_center = centerpoints[a_index];
    // printf("A CENTER: %d, %d\n", a_center.x, a_center.y);
    coordinate b_center = centerpoints[b_index];
    // printf("B CENTER: %d, %d\n", b_center.x, b_center.y);
    coordinate cursor;
    cursor.x = a_center.x;
    cursor.y = a_center.y;
    while (cursor.x != b_center.x || cursor.y != b_center.y)
    {
        /*printf("Current cursor (%d,%d)\n", cursor.x, cursor.y);
        printf("Goal coord (%d,%d)\n", b_center.x, b_center.y);
        printf("Value at cursor is %c\n", level[cursor.y][cursor.x]);*/
        if /*(*/ (level[cursor.y][cursor.x] == CORRIDOR /*|| level[cursor.y][cursor.x] == FLOOR)*/ && escaped == true)
        {
            // printf("Ran into another open space. Terminating early\n");
            return;
        }
        if (level[cursor.y][cursor.x] == ROCK)
        {
            escaped = true;
            // printf("placing a #\n");
            level[cursor.y][cursor.x] = CORRIDOR;
            lev.hardness_map[cursor.y][cursor.x] = 0;
        }
        calc_direction(cursor, b_center);
        int coin_flip = rand() % 2;
        if ((coin_flip == 0 && up_down != 0) || left_right == 0)
        {
            cursor.y += up_down;
        }
        else if (left_right != 0)
        {
            cursor.x += left_right;
        }
    }
    linked[a_index] = true;
    linked[b_index] = true;
    connected[connd] = comparer;
    connd++;
    /*printf("Cursor: %d, %d\n", cursor.x, cursor.y);
    printf("Goal: %d, %d\n", b_center.x, b_center.y);
    printf("exiting\n");*/
}

void link_room_in(int room_index)
{
    // printf("Linking in room %c\n", 'a' + room_index);
    int i;
    double x_length;
    double y_length;
    double distance;
    int closest_room_ind = 111;
    double closest_dist = 1000.0;
    for (i = 0; i < rooms_added; i++)
    {
        if (i == room_index || linked[i] == false)
        {
            // printf("continued because room %c was not linked(or is same ind)\n", 'a' + i);
            continue;
        }
        x_length = pow(abs(centerpoints[room_index].x - centerpoints[i].x), 2);
        y_length = pow(abs(centerpoints[room_index].y - centerpoints[i].y), 2);
        distance = sqrt(x_length + y_length);
        if (distance < closest_dist)
        {
            // printf("room %c is closer than room %c(linked) at %f spaces\n", 'a' + i, 'a' + closest_room_ind, distance);
            closest_dist = distance;
            closest_room_ind = i;
        }
    }
    // printf("Closest_dist at checkpoint:%f\n", closest_dist);
    if (closest_dist == 1000.0)
    {
        for (i = 0; i < rooms_added; i++)
        {
            if (i == room_index)
            {
                continue;
            }
            x_length = pow(abs(centerpoints[room_index].x - centerpoints[i].x), 2);
            y_length = pow(abs(centerpoints[room_index].y - centerpoints[i].y), 2);
            distance = sqrt(x_length + y_length);
            if (distance < closest_dist)
            {
                // printf("room %c is closer than room %c\n", 'a' + i, 'a' + closest_room_ind);
                closest_dist = distance;
                closest_room_ind = i;
            }
        }
    }
    // printf("linking %c to %c because they are %f apart\n", 'a' + room_index, 'a' + closest_room_ind, closest_dist);
    link_a_to_b(room_index, closest_room_ind);
}
bool link_rooms()
{
    int i;
    for (i = 0; i < rooms_added; i++)
    {
        link_room_in(i);
        /*for (int j = 0; j < rooms_added; j++)
        {
            link_a_to_b(i, j);
        }*/
    }
    return false;
}

void fill_with_rock()
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 79; j++)
        {
            if (level[i][j] == '\0')
            {
                level[i][j] = ROCK;
            }
        }
    }
}

void print_floor()
{
    /*for (int i = 0; i < rooms_added; i++)
    {
        level[centerpoints[i].y][centerpoints[i].x] = 'a' + i;
    }*/
    level[lev.player.y][lev.player.x] = '@';
    // printf("player: %d, %d\n", lev.player.y, lev.player.x);
    int row, col;
    for (row = 0; row < 21; row++)
    {
        for (col = 0; col < 80; col++)
        {
            printf("%c", level[row][col]);
        }
        printf("\n");
    }
}