#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dungeonutil.h"
#include "homemadeq.h"

extern dungeon lev;
extern char level[21][80];
int heatmap[21][80];

vertex_t graph[21][80];

int compare_vertexes(void *a, void *b)
{
    return ((vertex_t *)a)->distance - ((vertex_t *)b)->distance;
}

void vertex_init(vertex_t *v, coordinate loc, int dist)
{
    v->location = loc;
    v->distance = dist;
    v->prev = NULL;
    v->next = NULL;
}

int dijkstra(coordinate *cell, int enemy_tunnels, coordinate target)
{
    // Init the whole graph real quick
    for (int row = 0; row < 21; row++)
    {
        for (int col = 0; col < 80; col++)
        {
            coordinate loc = {col, row};
            vertex_init(&graph[row][col], loc, 10000); // If I used actual infinity, the int would overflow and mess up the comparison, so 10000 will do
        }
    }
    // create vertex priority queue Q
    queue *q = init_min_queue(compare_vertexes);
    graph[cell->y][cell->x].distance = 0; // Initialization of enemy cell
    insert(q, &graph[cell->y][cell->x]);  // add enemy to queue with distance 0

    for (int row = 0; row < 21; row++)
    {
        for (int col = 0; col < 80; col++)
        {
            vertex_t *v = &graph[row][col];
            if (v != &graph[cell->y][cell->x] && lev.hardness_map[v->location.y][v->location.x] != 255 && !(!enemy_tunnels && level[cell->y][cell->x] == ROCK))
            {
                // This first bit is unnecessary because the values are alread set, see previous for loop
                /*prev[v] ← NULL         // Predecessor of v
                dist[v] ← INFINITY // Unknown distance from source to v*/
                insert(q, v);
            }
        }
    }
    while (q->size > 0) // The main loop
    {
        /*printf("What the queue looks like:\n");
        for (int i = 0; i < q->size; i++)
        {
            printf("NODE %d: ", i);
            printf("    Distance: %d ", ((vertex_t *)(q->data[i].datum))->distance);
            printf("    Location: %d, %d\n", ((vertex_t *)(q->data[i].datum))->location.y, ((vertex_t *)(q->data[i].datum))->location.x);
        }*/
        vertex_t *u = (vertex_t *)pop_min(q); // Remove and return best vertex
        // printf("Popped Node w/ value: %d\n", u->distance);
        for (int neighboring_row = u->location.y - 1; neighboring_row < u->location.y + 2; neighboring_row++)
        {
            for (int neighboring_col = u->location.x - 1; neighboring_col < u->location.x + 2; neighboring_col++)
            {
                // Go through all v neighbors of u
                if (neighboring_col == u->location.x && neighboring_row == u->location.y)
                {
                    continue;
                }
                if (neighboring_col < 0 || neighboring_col > 79 || neighboring_row < 0 || neighboring_row > 20 || lev.hardness_map[neighboring_row][neighboring_col] == 255 ||
                    (!enemy_tunnels && level[neighboring_row][neighboring_col] == ROCK))
                {
                    continue;
                }
                vertex_t *v = &graph[neighboring_row][neighboring_col];
                int alt = u->distance + (lev.hardness_map[v->location.y][v->location.x] / 85) + 1;
                // printf("%d\n", alt);
                if (alt < v->distance) // A shorter path to v has been found
                {
                    // printf("Decreasing.\n");
                    v->prev = u;
                    v->distance = alt;
                }
                if (v->location.x == target.x && v->location.y == target.y)
                {
                    // printf("HIT!\n");
                    // v->prev = u;
                    // v->distance = alt;
                    goto esc;
                }
            }
        }
    }

// return dist, prev;
esc:
    vertex_t *current_v = &graph[target.y][target.x];
    while (current_v->prev != NULL)
    {
        // printf("Current: %d, %d.\n", current_v->location.x, current_v->location.y);
        // printf("Linking previous cell (%d, %d) to current.\n", current_v->prev->location.x, current_v->prev->location.y);
        current_v->prev->next = current_v;
        current_v = current_v->prev;
    }
    // printf("Ended after backtracing to %d, %d\n", current_v->location.x, current_v->location.y);
    // heatmap[cell->y][cell->x] = graph[lev.player.y][lev.player.x].distance;
    destroy_queue(q);
    return graph[target.y][target.x].distance;
}

void print_heatmap_nontunneling()
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            if (level[i][j] == ROCK)
            {
                printf(" ");
            }
            else if (i == lev.player.y && j == lev.player.x)
            {
                printf("@");
            }
            else
            {
                printf("%d", heatmap[i][j] % 10);
            }
        }
        printf("\n");
    }
}
void print_heatmap_tunneler()
{
    int i, j;
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 80; j++)
        {
            if (i == lev.player.y && j == lev.player.x)
            {
                printf("@");
            }
            else if (lev.hardness_map[i][j] == 255)
            {
                printf("X");
            }
            else
            {
                printf("%d", heatmap[i][j] % 10);
            }
        }
        printf("\n");
    }
}