#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "dungeonutil.h"
#include "enemies.h"
#include "homemadeq.h"

#define NPC_INTELLIGENT 0b00010000
#define NPC_TELEPATH 0b00100000
#define NPC_TUNNELER 0b01000000
#define NPC_ERRATIC 0b10000000

#define PLAYER_SPEED 10

extern vertex_t graph[21][80];
extern char level[21][80];
extern dungeon lev;
character_t **enemies;
int enemies_count;
int e_added = 0;

struct character
{
    unsigned char stats;
    coordinate player_spotted_at;
    coordinate locale;
    char replacing;
    int isplayer;
    int id;
};

int compare_speeds(void *a, void *b)
{
    return get_enemy_speed(b) - get_enemy_speed(a); // Its set up this way to convert the queue to a max-priority queue instead of mine withoutt me having to redo the whole structure
}

character_t *generateEnemy(int speed, char abilities)
{ // Even though abilities is a char, it should be passed as a 4-bit number
    character_t *enemy = malloc(sizeof(character_t));
    char details = 0; // chars get 8 bits (0000 0000)
    // Puts the speed into the empy char
    details &= speed;                      // Ex. if speed is 1010, details should look like 0000 1010;
    char abilities_shift = abilities << 4; // Shifts the abilities char to put it in the top nibble (f turns from 0000 1111 to 1111 0000)
    details |= abilities_shift;            // details should now be 1111 1010 as 1111 | 0000 is 1111 and 0000 | 1010 is 1010
    enemy->stats = details;
    int choose_x = rand() % 80;
    int choose_y = rand() % 21;
    while (level[choose_y][choose_x] != FLOOR && level[choose_y][choose_x] != CORRIDOR)
    {
        choose_x = rand() % 80;
        choose_y = rand() % 21;
    }
    enemy->locale.x = choose_x;
    enemy->locale.y = choose_y;
    if (enemy->stats & NPC_TELEPATH)
    {
        enemy->player_spotted_at.x = lev.player.x;
        enemy->player_spotted_at.y = lev.player.y;
    }
    else
    {
        enemy->player_spotted_at.x = enemy->locale.x;
        enemy->player_spotted_at.y = enemy->locale.y;
    }
    enemy->isplayer = false;
    enemy->id = e_added++;
    return enemy;
}
char get_enemy_display_char(character_t *enemy)
{
    if (enemy->isplayer)
    {
        return '@';
    }
    unsigned int code = enemy->stats >> 4; // Pushes speed out of the byte to the right and leaves the char offset (from a) in the bottom nibble
    // printf("%c\n", code);
    if (code < 10)
    {
        return '0' + code; // 0-9
    }
    return 'a' + (code - 10); // a-f
}

int get_enemy_speed(character_t *enemy)
{
    return (enemy->stats & 0x0F) + 5;
    // If stats is 1111 1010, 1111 & 0000 is 0000 and 1010 & 1111 is 1010 so it returns 0000 1010
}

int isOccupied(char tile)
{
    switch (tile)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'f':
        return 1;
    default:
        return 0;
    }
}

char kill(int y, int x)
{
    for (int i = 0; i < enemies_count; i++)
    {
        if (enemies[i]->locale.x == x && enemies[i]->locale.y == y)
        {
            enemies[i]->id = DEAD;
            return enemies[i]->replacing;
        }
    }
    return 'G'; // G will only ever appear on the map if there is an error
}

void move_entity(character_t *mover, coordinate target)
{
    level[mover->locale.y][mover->locale.x] = mover->replacing;
    mover->replacing = level[target.y][target.x];
    level[target.y][target.x] = get_enemy_display_char(mover);
}

int enemy_move(character_t *enemy)
{
    if (enemy->locale.x == enemy->player_spotted_at.x && enemy->locale.y == enemy->player_spotted_at.y)
    {
        return 0;
    }
    int is_erratic = enemy->stats & NPC_ERRATIC;
    int lr = 0;
    int ud = 0;
    if (is_erratic)
    {
        if (rand() % 2 == 0)
        {
            int tunneler = enemy->stats & NPC_TUNNELER;
        randomize:
            lr = rand() % 3;
            lr -= 1;
            ud = rand() % 3;
            ud -= 1;
            if (level[enemy->locale.y + ud][enemy->locale.x + lr] == '@')
            {
                return 1;
            }
            if (lr == 0 && ud == 0)
            {
                goto randomize;
            }
            int occ = isOccupied(level[enemy->locale.y + ud][enemy->locale.x + lr]);
            if (occ)
            {
                char rep = kill(enemy->locale.y + ud, enemy->locale.x + lr);
                level[enemy->locale.y][enemy->locale.x] = enemy->replacing;
                enemy->replacing = rep;
                level[enemy->locale.y + ud][enemy->locale.x + lr] = get_enemy_display_char(enemy);
                enemy->locale.x += lr;
                enemy->locale.y += ud;
                return 0;
            }
            else
            {
                if (level[enemy->locale.y + ud][enemy->locale.x + lr] == ROCK)
                {
                    if (!tunneler)
                    {
                        goto randomize;
                    }
                    lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] -= 85;
                    if (lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] <= 0)
                    {
                        lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] = 0;
                        level[enemy->locale.y + ud][enemy->locale.x + lr] = CORRIDOR;
                        coordinate dest = {enemy->locale.x, enemy->locale.y};
                        move_entity(enemy, dest);
                    }
                    return 0;
                }
                else
                {
                    coordinate targ = {enemy->locale.y + ud, enemy->locale.x + lr};
                    move_entity(enemy, targ);
                    return 0;
                }
            }
        }
    }
    int is_smart = enemy->stats & NPC_INTELLIGENT;
    if (is_smart)
    {
        dijkstra(&enemy->locale, enemy->stats & NPC_TUNNELER, enemy->player_spotted_at);
        vertex_t *moveto = graph[enemy->locale.y][enemy->locale.x].next;
        if (level[moveto->location.y][moveto->location.x] == '@')
        {
            return 1;
        }
        int occ = isOccupied(level[moveto->location.y][moveto->location.x]);
        if (occ)
        {
            char rep = kill(moveto->location.y, moveto->location.x);
            level[moveto->location.y][moveto->location.x] = enemy->replacing;
            enemy->replacing = rep;
            level[moveto->location.y][moveto->location.x] = get_enemy_display_char(enemy);
            enemy->locale.x = moveto->location.x;
            enemy->locale.y = moveto->location.y;
            return 0;
        }
        if (level[moveto->location.y][moveto->location.x] == ROCK)
        {
            lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] -= 85;
            if (lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] <= 0)
            {
                lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] = 0;
                level[enemy->locale.y + ud][enemy->locale.x + lr] = CORRIDOR;
                coordinate dest = {enemy->locale.x, enemy->locale.y};
                move_entity(enemy, dest);
            }
            return 0;
        }
        coordinate dest = {moveto->location.x, moveto->location.y};
        move_entity(enemy, dest);
        return 0;
    }
    int targetx = enemy->player_spotted_at.x;
    int targety = enemy->player_spotted_at.y;
    int currentx = enemy->locale.x;
    int currenty = enemy->locale.y;
    lr = targetx - currentx;
    ud = targety - currenty;
    lr = lr < 0 ? -1 : 1;
    ud = ud < 0 ? -1 : 1;
    if (targetx == currentx)
    {
        lr = 0;
    }
    if (targety == currenty)
    {
        ud = 0;
    }
    if (level[enemy->locale.y + ud][enemy->locale.x + lr] == '@')
    {
        return 1;
    }
    int occ = isOccupied(level[enemy->locale.y + ud][enemy->locale.x + lr]);
    if (occ)
    {
        char rep = kill(enemy->locale.y + ud, enemy->locale.x + lr);
        level[enemy->locale.y][enemy->locale.x] = enemy->replacing;
        enemy->replacing = rep;
        level[enemy->locale.y + ud][enemy->locale.x + lr] = get_enemy_display_char(enemy);
        enemy->locale.x += lr;
        enemy->locale.y += ud;
        return 0;
    }
    if (level[enemy->locale.y + ud][enemy->locale.x + lr] == ROCK)
    {
        if (!(enemy->stats & NPC_TUNNELER))
        {
            return 0;
        }
        lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] -= 85;
        if (lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] <= 0)
        {
            lev.hardness_map[enemy->locale.y + ud][enemy->locale.x + lr] = 0;
            level[enemy->locale.y + ud][enemy->locale.x + lr] = CORRIDOR;
            coordinate dest = {enemy->locale.x, enemy->locale.y};
            move_entity(enemy, dest);
        }
        return 0;
    }
    else
    {
        coordinate targ = {enemy->locale.y + ud, enemy->locale.x + lr};
        move_entity(enemy, targ);
        return 0;
    }
    return 0;
    /*

    Is enemy erratic?
    If so.. 50% chance to move randomly
    Is enemy smart
    yes - dijkstra to player
    no - travel straight toward player

    */
}

void init_enemies(int nummon)
{
    if (nummon == -1)
    {
        enemies_count = 10 + ((rand() % 3) - 1); // Should be 10 +/- 1
    }
    else
    {
        enemies_count = nummon;
    }
    enemies = malloc(sizeof(character_t) * enemies_count);
    for (int i = 0; i < enemies_count; i++)
    {
        enemies[i] = generateEnemy(rand() % 16, rand() % 16);
        enemies[i]->replacing = level[enemies[i]->locale.y][enemies[i]->locale.x];
        level[enemies[i]->locale.y][enemies[i]->locale.x] = get_enemy_display_char(enemies[i]);
    }
}

int do_turn(queue *q, character_t **enemies, character_t *player, int turn_count)
{
    for (int i = 0; i < enemies_count; i++)
    {
        if (enemies[i]->id == DEAD)
        {
            continue;
        }
        if (turn_count == 0 || (int)(1000 / get_enemy_speed(enemies[i])) % turn_count == 0)
        {
            insert(q, enemies[i]);
        }
    }
    if (turn_count % 10 == 0)
    {
        insert(q, player);
    }
    while (!is_empty(q))
    {
        character_t *enemy = pop_min(q);
        while (enemy->id == DEAD)
        {
            enemy = pop_min(q);
            if (is_empty(q))
            {
                return 2;
            }
        }
        int outcome = enemy->isplayer ? player_move() : enemy_move(enemy);
        if (outcome == 1)
        {
            destroy_queue(q);
            return 1;
        }
    }
    return 0;
}

int run()
{
    int turn_count = 0;
    /*int tunnelers = 0;
    for (int i = 0; i < enemies_count; i++)
    {
        tunnelers += (enemies[i]->stats & NPC_TUNNELER) != 0;
    } //DEBUG */
    // printf("Out of %d enemies, %d should be tunneling\n", enemies_count, tunnelers);
    character_t player;
    player.isplayer = true;
    player.locale.x = lev.player.x;
    player.locale.y = lev.player.y;
    player.stats = (char)PLAYER_SPEED; // should read as 0000 1010 because the player has a set display char which fits outside the normal set
    queue *q = init_min_queue(compare_speeds);
    print_floor();
    while (1)
    {
        int res = do_turn(q, enemies, &player, turn_count);
        if (res == 1)
        {
            for (int i = 0; i < enemies_count; i++)
            {
                free(enemies[i]);
            }
            free(enemies);
            return 1;
        }
        if (res == 2)
        {
            return 0;
        }
        print_floor();
        usleep(250000);
        // turn_count++;
    }
    // destroy_queue(q);
    for (int i = 0; i < enemies_count; i++)
    {
        free(enemies[i]);
    }
    free(enemies);
    return 0;
}

int has_LOS(coordinate looker, coordinate target)
{
    // https://steemit.com/programming/@woz.software/roguelike-line-of-sight-calculation was used as reference for this function
    int x_diff = target.x - looker.x;
    int y_diff = target.y - looker.y;
    int x_dir = x_diff < 0 ? -1 : 1;
    int y_dir = y_diff < 0 ? -1 : 1;
    if (x_diff == 0)
    {
        x_dir = 0;
    }
    if (y_diff == 0)
    {
        y_dir = 0;
    }
    int error = x_diff - y_diff;
    int x_correct = 2 * x_diff;
    int y_correct = 2 * y_diff;
    coordinate cur = looker;
    while (1)
    {
        if (cur.x == target.x && cur.y == target.y)
        {
            // printf("FOUND LOS\n");
            return 1;
        }
        if (level[cur.y][cur.x] == ROCK)
        {
            return 0;
        }
        if (error > 0)
        {
            cur.x += x_dir;
            error -= y_correct;
        }
        else if (error < 0)
        {
            cur.y += y_dir;
            error += x_correct;
        }
        else
        {
            cur.x += x_dir;
            cur.y += y_dir;
        }
    }
    // printf("NO LOS\n");
    return 0;
}

void take_player_input()
{
    // printf("Please enter an input to simulate a character action.");
    // char *a = malloc(2);
    // scanf("%s", a);
    // free(a);
}

int player_move()
{
    take_player_input();
    // move_player(); //If player moves into enemy, return 1   PLAYER DOES NOT MOVE AT THIS POINT IN TIME
    for (int i = 0; i < enemies_count; i++)
    { // Set where the enemy should pathfind to
        character_t *current_enemy = enemies[i];
        if (current_enemy->stats & NPC_TELEPATH)
        {
            current_enemy->player_spotted_at = lev.player;
        }
        else if (has_LOS(current_enemy->locale, lev.player))
        {
            current_enemy->player_spotted_at = lev.player;
        }
    }
    return 0;
}