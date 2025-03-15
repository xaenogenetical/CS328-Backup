#ifndef DUNGEONUTIL_H__
#define DUNGEONUTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
  // Partially obtained from Jeremy Sheaffer (Iowa State University)

#define FLOOR '.'
#define CORRIDOR '#'
#define ROCK ' '
#define DWNSTAIR '>'
#define UPSTAIR '<'

  struct vertex;
  typedef struct vertex vertex_t;
  struct node;
  typedef struct node node_t;
  struct coord;
  typedef struct coord coordinate;

  struct node
  {
    vertex_t *datum;
  };

  typedef struct
  {
    node_t data[1680];
    int size;
  } queue;

  queue *init_min_queue();
  int compare_vertexes(const node_t *a, const node_t *b);
  void swap(node_t *a, node_t *b);
  void heapify(queue *pq, int idx);
  void insert(queue *pq, vertex_t *key);
  vertex_t *pop_min(queue *pq);
  vertex_t *peek_min(queue *pq);
  void decrease(queue *pq, coordinate loc, int alt);
  void destroy_queue(queue *pq);

  struct coord
  {
    uint8_t x;
    uint8_t y;
  };
  typedef struct
  {
    coordinate start;
    uint8_t width;
    uint8_t height;
  } room;

  typedef struct
  {
    coordinate player;
    uint8_t hardness_map[21][80];
    int16_t num_rooms;
    room *rooms;
    int16_t num_up_stair;
    coordinate *upstairs;
    int16_t num_dn_stair;
    coordinate *downstairs;
  } dungeon;

  struct vertex
  {
    coordinate location;
    int distance;
    vertex_t *prev;
    vertex_t *neighbors[8];
  };

#ifdef __cplusplus
}
#endif

#endif
