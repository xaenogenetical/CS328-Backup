#ifndef DUNGEONUTIL_H__
#define DUNGEONUTIL_H__
#include <stdint.h>
typedef struct
{
    uint8_t x;
    uint8_t y;
} coordinate;

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

#endif /*DUNGEONUTIL_H__*/