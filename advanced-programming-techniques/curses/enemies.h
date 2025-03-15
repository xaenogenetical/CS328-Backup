#ifndef ENEMIES_H
#define ENEMIES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

    struct character;
    typedef struct character character_t;

    character_t *generateEnemy(int speed, char abilities);
    char get_enemy_display_char(character_t *enemy);
    int get_enemy_speed(character_t *enemy);
    int isOccupied(char tile);
    int enemy_move(character_t *enemy);
    void init_enemies(int nummon);
    int run();
    int player_move();

#ifdef __cplusplus
}
#endif

#endif