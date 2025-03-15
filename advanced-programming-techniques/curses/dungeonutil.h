#ifndef DUNGEONUTIL_H__
#define DUNGEONUTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#define FLOOR '.'
#define CORRIDOR '#'
#define ROCK ' '
#define DWNSTAIR '>'
#define UPSTAIR '<'
#define DEAD -1

  struct vertex;
  typedef struct vertex vertex_t;
  struct coord;
  typedef struct coord coordinate;
  struct node;
  typedef struct node node_t;

  void lose_game();
  void win_game();

  int dijkstra(coordinate *cell, int enemy_tunnels, coordinate target);
  void print_floor();

  int compare_vertexes(void *a, void *b);

  /*#define swap(a, b) ({    \
  typeof (a) _tmp = (a); \
  (a) = (b);             \
  (b) = _tmp;            \
  })*/

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
    vertex_t *next;
    vertex_t *neighbors[8];
  };

#ifdef __cplusplus
}
#endif

#endif
