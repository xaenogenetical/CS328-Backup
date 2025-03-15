#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "dungeonutil.h"
#include "homemadeq.h"

extern vertex_t graph[21][80];
extern dungeon lev;
extern int data_idx;

queue *init_min_queue(int (*comp)(void *a, void *b))
{
    queue *pq = (queue *)malloc(sizeof(queue));
    pq->size = 0;
    pq->compare = comp;
    pq->data = NULL;
    return pq;
}

int is_empty(queue *q)
{
    return q->size == 0;
}

void swap(node_t *a, node_t *b)
{
    node_t temp = *a;
    *a = *b;
    *b = temp;
}

void quick_sort(node_t *data, int low, int high, int (*comp)(void *a, void *b))
{
    if (low < high)
    {
        int pi = partition(data, low, high, comp);
        quick_sort(data, low, pi - 1, comp);
        quick_sort(data, pi + 1, high, comp);
    }
}

int partition(node_t *data, int low, int high, int (*comp)(void *a, void *b))
{
    void *pivot = data[high].datum;
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++)
    {
        if (comp(data[j].datum, pivot) < 0)
        {
            i++;
            swap(&data[i], &data[j]);
        }
    }
    swap(&data[i + 1], &data[high]);
    return (i + 1);
}

void sort(queue *pq)
{
    if (pq->size > 1)
    {
        quick_sort(pq->data, 0, pq->size - 1, pq->compare);
    }
}

void insert(queue *pq, void *key)
{
    // node_t *datas = pq->data;
    pq->data = realloc(pq->data, sizeof(node_t) * (pq->size + 1));
    // memcpy(pq->data, datas, sizeof(node_t) * pq->size);
    // free(datas);

    node_t new_node;
    new_node.datum = key;
    pq->data[pq->size] = new_node;
    // int i = pq->size;
    pq->size++;
    sort(pq);
}

void *pop_min(queue *pq)
{

    if (pq->size <= 0)
        return NULL;
    if (pq->size == 1)
    {
        pq->size--;
        return pq->data[0].datum;
    }
    // node_t *new_data = (node_t*)malloc(sizeof(node_t) * (pq->size -1));
    // memcpy(new_data, pq->data + 1, sizeof(node_t) * (pq->size -1));
    void *root = pq->data[0].datum;
    node_t *smaller_data = malloc(sizeof(node_t) * (pq->size - 1));
    memcpy(smaller_data, pq->data + 1, sizeof(node_t) * (pq->size - 1));
    free(pq->data);
    pq->data = smaller_data;
    pq->size--;
    sort(pq);

    return root;
}

void *peek_min(queue *pq)
{
    if (pq->size <= 0)
        return NULL;
    return pq->data[0].datum;
}

void decrease(queue *q, coordinate loc, int alt)
{
    if (graph[loc.y][loc.x].distance < alt)
    {
        printf("Error: New distance is greater than current distance, this shouldn't happen if the program works properly.\n");
        return;
    }
    // printf("(in decrease) Decreasing %d, %d to %d\n", loc.y, loc.x, alt);
    // printf("Player location: %d, %d\n", lev.player.y, lev.player.x);
    graph[loc.y][loc.x].distance = alt;
    // printf("(in decrease)New value post-assignment: %d\n", graph[loc.y][loc.x].distance);
    /*if (lev.player.y == loc.y && lev.player.x == loc.x)
    {
        printf("Player Tile Decreased\n");
        printf("Player Distance: %d\n", graph[loc.y][loc.x].distance);
    }*/
}

void destroy_queue(queue *pq)
{
    free(pq->data);
    free(pq);
}
