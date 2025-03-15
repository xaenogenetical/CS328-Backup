#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "dungeonutil.h"
int saveload(char option);
dungeon draw_dungeon();
extern dungeon lev;
extern char level[21][80];
void print_heatmap_nontunneling();
void print_heatmap_tunneler();
int dijkstra(coordinate *cell);

void print_loading_bar(int current, int total)
{
    int bar_width = 79 - strlen("[] 1680/1680");
    float progress = (float)current / total;
    int pos = bar_width * progress;
    printf("[");
    for (int i = 0; i < bar_width; i++)
    {
        if (i <= pos)
        {
            printf("#");
        }
        else
        {
            printf(" ");
        }
    }
    printf("] %d/1680\r", current);
    fflush(stdout);
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Running with no arguments presumes you want to display the enemy pathfinding information.\n");
        printf("I apologize in advance, the file takes a good moment to load due to the fact that I'm pretty sure it runs in O(n^2) time.\n");
        printf("Please be patient.\n");
        goto heats;
    }
    if (strcmp(argv[1], "--load") == 0)
    {
        saveload('r');
        // printf("LOADED\n");
    }
    else if (strcmp(argv[1], "--save") == 0 || (argc > 2 && strcmp(argv[2], "--save") == 0))
    {
        saveload('w');
        // printf("SAVED\n");
    }
    else
    {
        // badarg:
        fprintf(stderr, "Usage: dungeon OR dungeon <option>\n");
        printf("Failed to start program due to improper arguments.\n");
        return 1;
    }
    free(lev.rooms);
    free(lev.upstairs);
    free(lev.downstairs);
    return 0;
heats:
    draw_dungeon();
    int z = 0;
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            print_loading_bar(z, 1680);
            z++;
            if (lev.hardness_map[i][j] == 255)
            {
                continue;
            }
            // printf("Calculating Dijkstra for %d, %d\n", j, i);
            coordinate cell = {j, i};
            /*int dist_from_player = */ dijkstra(&cell);
            // printf("Distance from player to %d, %d is %d\n", j, i, dist_from_player);
        }
    }
    printf("\r\x1b[K\n");
    fflush(stdout);
    printf("Heatmap for non-tunneling monsters:\n");
    print_heatmap_nontunneling();
    printf("Heatmap for tunneling monsters:\n");
    print_heatmap_tunneler();
    return 0;
}
