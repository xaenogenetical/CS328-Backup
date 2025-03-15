#ifndef HOMEMADEQ_H
#define HOMEMADEQ_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct node;
    typedef struct node node_t;
    struct vertex;
    typedef struct vertex vertex_t;
    struct coordinate;
    typedef struct coord coordinate;

    struct node
    {
        void *datum;
    }; // User is responsible for ensuring that the type of datum is appropriate for its usage

    typedef struct
    {
        node_t *data;
        int size;
        int (*compare)(void *a, void *b);
    } queue;
    // IMPORTANT!!! User is responsible for calling destroy_queue on any queue created by init_min_queue. Failure to do so will most likely result in memory leaks!!
    queue *init_min_queue(int (*compare)(void *a, void *b));
    int is_empty(queue *q);
    void swap(node_t *a, node_t *b);
    void insert(queue *pq, void *key);
    void *pop_min(queue *pq);
    void *peek_min(queue *pq);
    void decrease(queue *pq, coordinate loc, int alt);
    void destroy_queue(queue *pq);
    void quick_sort(node_t *data, int low, int high, int (*comp)(void *a, void *b));
    int partition(node_t *data, int low, int high, int (*comp)(void *a, void *b));
    void sort(queue *pq);
#ifdef __cplusplus
}
#endif

#endif