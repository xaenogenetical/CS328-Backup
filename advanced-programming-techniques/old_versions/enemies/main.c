#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <curses.h>
#include "dungeonutil.h"
#include "enemies.h"
#include "homemadeq.h"
extern dungeon lev;
extern char level[21][80];
void print_heatmap_nontunneling();
void print_heatmap_tunneler();
extern character_t *enemies;

int contains(const char *str[], char *target)
{
    for (int i = 0; str[i]; i++)
    {
        if (strcmp(str[i], target) == 0)
        {
            return i;
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    // initscr();
    // raw();
    // noecho();
    // keypad(stdscr, true);
    if (contains(argv, "--load"))
    {
        saveload('r');
        // printf("LOADED\n");
    }
    if (contains(argv, "--save"))
    {
        saveload('w');
        // printf("SAVED\n");
    }
    else
    {
        draw_dungeon();
    }
    // printf("saveload passed\n");
    int idx = contains(argv, "--nummon");
    if (idx)
    {
        init_enemies(atoi(argv[idx + 1]));
    }
    else
    {
        init_enemies(-1);
    }
    // printf("enemies initialized\n");
    int result = run();
    if (result == 0)
    {
        printf("congrats on standing still\n");
    }
    else
    {
        printf("you died :(\n");
    }
    free(lev.rooms);
    free(lev.upstairs);
    free(lev.downstairs);
    // endwin();
    return 0;
}
