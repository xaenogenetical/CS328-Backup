#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "dungeonutil.h"

#define FLOOR '.'
#define CORRIDOR '#'
#define ROCK ' '
#define DWNSTAIR '>'
#define UPSTAIR '<'

char level[21][80];
int hardness[21][80]; // imlementing in this way because it would be too much work to overhaul the whole system to turn level into a struct array of chars AND ints

char **rooms[21 * 80]; // stores pointers to each room created by place_rooms()

typedef struct
{
    int x;
    int y;
} coordinate;

coordinate centerpoints[100];

int rooms_added = 0;

int taken_spaces = 0;

coordinate connected[10]; // this is made of coordinates because i can't be bothered to write a whole new struct for the same purpose
int connd = 0;

bool linked[100];

int up_down = 0;
int left_right = 0;

bool place_rooms();
void add_to_rooms(char *room[]);
void add_stairs();
bool link_rooms();
void fill_with_rock();
void print_floor();

int main(int argc, char *argv[])
{
    srand(time(NULL));
    coordinate placeholder;
    placeholder.x = -1; // used to connect the first two rooms when connecting because connected would be empty otherwise
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
    for (int i = 0; i < 21; i++)
    {
        if (i == 0 || i == 20)
        {
            for (int j = 0; j < 80; j++)
            {
                hardness[i][j] = 255;
            }
        }
        hardness[i][0] = 255;
        hardness[i][79] = 255;
    }
    place_rooms();
    fill_with_rock();
    link_rooms();
    add_stairs();
    print_floor();
    return 0;
}

bool place_rooms()
{
    int attempts = 0;
    int rand_row;
    int rand_col;
    int width;
    int height;
    coordinate room_center;
    char *room[height]; // array of pointers to the beginning of each row (a spot in level[][])
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
            room[i] = &level[i + rand_row][rand_col];
        }
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                room[i][j] = FLOOR;
                hardness[i][j] = 0;
            }
        }
        taken_spaces += width * height;
        rooms[rooms_added] = room;
        room_center.x = rand_col + (width / 2);
        // printf("roomc x: %d\n", room_center.x);
        room_center.y = rand_row + (height / 2);
        // printf("roomc y: %d\n", room_center.y);
        centerpoints[rooms_added] = room_center;
        linked[rooms_added] = false;
        rooms_added++;
        // printf("Area opened: %f\n", (double)taken_spaces / (80 * 21));
    }
    return true;
}

void add_stairs()
{
    int num_up_stair = rand() % 3;
    int num_dwn_stair = rand() % 3;
    int up_placed = 0;
    int dwn_placed = 0;
    int rand_row;
    int rand_col;
    while (up_placed < num_up_stair + 1)
    {
        rand_row = rand() % 21;
        rand_col = rand() % 80;

        if (level[rand_row][rand_col] == CORRIDOR || level[rand_row][rand_col] == FLOOR)
        {
            // printf("valat1: %c\n", level[rand_row][rand_col]);
            level[rand_row][rand_col] = UPSTAIR;
            up_placed++;
        }
    }
    while (dwn_placed < num_dwn_stair + 1)
    {
        rand_row = rand() % 21;
        rand_col = rand() % 80;
        if (level[rand_row][rand_col] == CORRIDOR || level[rand_row][rand_col] == FLOOR)
        {
            // printf("valat2: %c\n", level[rand_row][rand_col]);
            level[rand_row][rand_col] = DWNSTAIR;
            dwn_placed++;
        }
    }
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
    coordinate current = centerpoints[room_index];
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