#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "dungeonutil.h"
int saveload(char option);
dungeon draw_dungeon();
extern dungeon lev;

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        draw_dungeon();
    }
    else if (strcmp(argv[1], "--load") == 0)
    {
        saveload('r');
        // printf("LOADED\n");
    }
    else if (strcmp(argv[1], "--save") == 0)
    {
        saveload('w');
        // printf("SAVED\n");
    }
    else
    {
        fprintf(stderr, "Usage: dungeon OR dungeon <option>\n");
        printf("Failed to start program due to improper arguments.\n");
        return 1;
    }
    free(lev.rooms);
    free(lev.upstairs);
    free(lev.downstairs);
    return 0;
}
