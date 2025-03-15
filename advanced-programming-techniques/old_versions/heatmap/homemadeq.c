#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "dungeonutil.h"

extern vertex_t graph[21][80];
extern dungeon lev;

queue *init_min_queue()
{
    queue *pq = (queue *)malloc(sizeof(queue));
    pq->size = 0;
    return pq;
}
int compare_vertexes(const node_t *a, const node_t *b)
{
    return a->datum->distance - b->datum->distance;
}

void swap(node_t *a, node_t *b)
{
    node_t temp = *a;
    *a = *b;
    *b = temp;
}

/*void heapify(queue *pq, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < pq->size && compare_vertexes(&pq->data[left], &pq->data[smallest]) < 0)
    {
        smallest = left;
    }

    if (right < pq->size && compare_vertexes(&pq->data[right], &pq->data[smallest]) < 0)
    {
        smallest = right;
    }

    if (smallest != idx)
    {
        swap(&pq->data[idx], &pq->data[smallest]);
        heapify(pq, smallest);
    }
}*/

void sort(queue *pq)
{
    // printf("Merging queue with size %d\n", pq->size);
    if (pq->size <= 1)
    {
        return;
    }
    int mid = pq->size / 2;
    queue left, right;
    left.size = mid;
    right.size = pq->size - mid;

    for (int i = 0; i < mid; i++)
    {
        left.data[i] = pq->data[i];
    }
    for (int i = mid; i < pq->size; i++)
    {
        right.data[i - mid] = pq->data[i];
    }

    sort(&left);
    sort(&right);

    int i = 0, j = 0, k = 0;
    while (i < left.size && j < right.size)
    {
        if (compare_vertexes(&left.data[i], &right.data[j]) <= 0)
        {
            // printf("Placing %d from left into pq, beat %d from right.\n", left.data[i].datum->distance, right.data[j].datum->distance);
            pq->data[k++] = left.data[i++];
        }
        else
        {
            // printf("Placing %d from right into pq, beat %d from left.\n", right.data[j].datum->distance, left.data[i].datum->distance);
            pq->data[k++] = right.data[j++];
        }
    }

    while (i < left.size)
        pq->data[k++] = left.data[i++];
    while (j < right.size)
        pq->data[k++] = right.data[j++];
}

void insert(queue *pq, vertex_t *key)
{
    node_t new_node;
    new_node.datum = key;
    pq->data[pq->size] = new_node;
    // int i = pq->size;
    pq->size++;

    /*while (i != 0 && compare_vertexes(&pq->data[i - 1], &pq->data[i]) > 0)
    {
        swap(&pq->data[i], &pq->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }*/
}

vertex_t *pop_min(queue *pq)
{
    if (pq->size <= 0)
        return NULL;
    if (pq->size == 1)
    {
        pq->size--;
        return pq->data[0].datum;
    }
    vertex_t *root = pq->data[0].datum;
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    // heapify(pq, 0);
    sort(pq);

    return root;
}

vertex_t *peek_min(queue *pq)
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
    free(pq);
}
