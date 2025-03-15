#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dungeonutil.h"

extern dungeon lev;
extern char level[21][80];
int heatmap[21][80];

vertex_t graph[21][80];

void vertex_init(vertex_t *v, coordinate *loc, int dist, vertex_t *prev)
{
    v->location = *loc;
    v->distance = dist;
    v->prev = prev;
}

int dijkstra(coordinate *cell)
{
    // Init the whole graph real quick
    for (int row = 0; row < 21; row++)
    {
        for (int col = 0; col < 80; col++)
        {
            coordinate loc = {col, row};
            vertex_init(&graph[row][col], &loc, 10000, NULL); // If I used actual infinity, the int would overflow and mess up the comparison, so 10000 will do
        }
    }
    // create vertex priority queue Q
    queue *q = init_min_queue();
    graph[cell->y][cell->x].distance = 0; // Initialization of enemy cell
    insert(q, &graph[cell->y][cell->x]);  // add enemy to queue with distance 0

    for (int row = 0; row < 21; row++)
    {
        for (int col = 0; col < 80; col++)
        {
            vertex_t *v = &graph[row][col];
            if (v != &graph[cell->y][cell->x] && lev.hardness_map[v->location.y][v->location.x] != 255)
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
            printf("    Distance: %d ", q->data[i].datum->distance);
            printf("    Location: %d, %d\n", q->data[i].datum->location.y, q->data[i].datum->location.x);
        }*/
        vertex_t *u = pop_min(q); // Remove and return best vertex
        for (int neighboring_row = u->location.y - 1; neighboring_row < u->location.y + 2; neighboring_row++)
        {
            for (int neighboring_col = u->location.x - 1; neighboring_col < u->location.x + 2; neighboring_col++)
            {
                // Go through all v neighbors of u
                if (neighboring_col == u->location.x && neighboring_row == u->location.y)
                {
                    continue;
                }
                if (neighboring_col < 0 || neighboring_col > 79 || neighboring_row < 0 || neighboring_row > 20 || lev.hardness_map[neighboring_row][neighboring_col] == 255)
                {
                    continue;
                }
                vertex_t *v = &graph[neighboring_row][neighboring_col];
                int alt = u->distance + (lev.hardness_map[v->location.y][v->location.x] / 85) + 1;
                // printf("alt: %d\n", alt);
                // printf("u(min, %d,%d) dist: %d, hardness to pass:%d\n (plus one)\n", u->location.x, u->location.y, u->distance, lev.hardness_map[v->location.y][v->location.x] / 85);
                // printf("v(neighbor, %d,%d) dist: %d\n", v->location.x, v->location.y, v->distance);
                char *c = malloc(1);
                // scanf("%c", c);
                free(c);
                if (alt < v->distance) // A shorter path to v has been found
                {
                    // printf("Decreasing.\n");
                    v->prev = u;
                    v->distance = alt;
                    // coordinate v_loc = {v->location.x, v->location.y};
                    //  printf("Decreasing %d, %d to %d\n", v_loc.y, v_loc.x, alt);
                    /*printf("FUNNy\n");
                    printf("%d,%d\n", v->location.x, v->location.y);
                    printf("%d,%d\n", lev.player.x, lev.player.y);
                    printf("%d, %d\n", v_loc.x, v_loc.y);
                    printf("alt: %d\n", alt);*/
                    /*decrease(q, v_loc, alt);
                    printf("%d\n", v->distance);
                    printf("%d\n", graph[lev.player.y][lev.player.x].distance);*/
                }
                if (v == &graph[lev.player.y][lev.player.x])
                {
                    // printf("HIT!\n");
                    goto esc;
                }
            }
        }
    }

// return dist, prev;
esc:
    // printf("Player found at %d, %d\n", lev.player.y, lev.player.x);
    //  printf("Setting heatmap[%d][%d] to %d\n", cell->y, cell->x, graph[lev.player.y][lev.player.x].distance);
    heatmap[cell->y][cell->x] = graph[lev.player.y][lev.player.x].distance;
    destroy_queue(q);
    // printf("Dijkstra complete, player has been determined to be %d away\n", graph[lev.player.y][lev.player.x].distance);
    return graph[lev.player.y][lev.player.x].distance;
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