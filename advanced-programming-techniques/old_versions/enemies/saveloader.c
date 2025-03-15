#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "endian.h"
#include <errno.h>
#include <stdint.h>
#include "dungeonutil.h"

bool write_file(FILE *f);
bool read_file(FILE *f);
void draw_dungeon();
void draw_dungeon_from_file();

coordinate pc;
int level_hardness[21][80];
int num_rooms;
room *room_list;
int loader_num_up;
coordinate *loader_upstairs;
int loader_num_dwn;
coordinate *loader_downstairs;
extern dungeon lev;

int saveload(char option)
{
    // mkdir(folder_path, 0777);
    FILE *f;
    char *homedir = getenv("HOME");
    char path[] = "/.rlg327/dungeon";
    char folder[] = "/.rlg327";
    // printf("HOME is: %s\n", homedir);
    char *filepath = (char *)malloc(sizeof(char) * strlen(homedir) + strlen(path) + 1);
    strcpy(filepath, homedir);
    char *folder_path = (char *)malloc(sizeof(char) * strlen(homedir) + strlen(folder) + 1);
    strcpy(folder_path, homedir);
    strcat(filepath, path);
    strcat(folder_path, folder);
    mkdir(folder_path, 0777);
    // printf("Path is: %s\n", filepath);
    if (option == 'r')
    {
        f = fopen(filepath, "r");
        if (f == NULL)
        {
            goto badfile;
        }
        // printf("seY??\n");
        read_file(f);
    }
    else if (option == 'w')
    {
        f = fopen(filepath, "w");
        if (f == NULL)
        {
            goto badfile;
        }
        // printf("Yes??\n");
        write_file(f);
    }
    else
    {
        return 2;
    }
    fclose(f);
    free(folder_path);
    free(filepath);
    return 0;
badfile:
    perror("Failed to open file");
    printf("Error opening dungeon file %s\n", filepath);
    printf("Error message: %s\n", strerror(errno));
    return 1;
}

bool read_file(FILE *f)
{

    // int32_t version;
    // int file_size;
    int8_t player_x;
    int8_t player_y;
    uint8_t hards[21][80];
    int16_t room_c;
    room *room_data;
    int16_t up_c;
    coordinate *ups;
    int16_t down_c;
    coordinate *downs;
    // printf("TEST");
    fseek(f, 12, SEEK_SET);
    // int version_read;
    // fread(&version_read, sizeof(int), 1, f);
    fseek(f, sizeof(int), SEEK_CUR);
    // version = be32toh(version_read);
    // int size_read;
    // fread(&size_read, sizeof(int), 1, f);
    fseek(f, sizeof(int), SEEK_CUR);
    // file_size = be32toh(size_read);
    fread(&player_x, 1, 1, f);
    fread(&player_y, 1, 1, f);
    lev.player.x = player_x;
    lev.player.y = player_y;
    fread(&hards, 1, 1680, f);
    memcpy(lev.hardness_map, hards, 1680);
    int16_t room_c_read;
    fread(&room_c_read, 2, 1, f);
    room_c = be16toh(room_c_read);
    room_data = (room *)malloc(sizeof(room) * room_c);
    for (int i = 0; i < room_c; i++)
    {
        fread(&room_data[i].start.x, 1, 1, f);
        fread(&room_data[i].start.y, 1, 1, f);
        fread(&room_data[i].width, 1, 1, f);
        fread(&room_data[i].height, 1, 1, f);
    }
    lev.num_rooms = room_c;
    lev.rooms = (room *)malloc(sizeof(room) * room_c);
    memcpy(lev.rooms, room_data, sizeof(room) * room_c);
    int16_t up_c_read;
    fread(&up_c_read, 2, 1, f);
    up_c = be16toh(up_c_read);
    ups = (coordinate *)malloc(sizeof(coordinate) * up_c);
    for (int i = 0; i < up_c; i++)
    {
        fread(&ups[i].x, 1, 1, f);
        fread(&ups[i].y, 1, 1, f);
    }
    lev.num_up_stair = up_c;
    lev.upstairs = (coordinate *)malloc(sizeof(coordinate) * up_c);
    memcpy(lev.upstairs, ups, sizeof(coordinate) * up_c);
    int16_t down_c_read;
    fread(&down_c_read, 2, 1, f);
    down_c = be16toh(down_c_read);
    downs = (coordinate *)malloc(sizeof(coordinate) * down_c);
    for (int i = 0; i < down_c; i++)
    {
        fread(&downs[i].x, 1, 1, f);
        fread(&downs[i].y, 1, 1, f);
    }
    lev.num_dn_stair = down_c;
    lev.downstairs = (coordinate *)malloc(sizeof(coordinate) * down_c);
    memcpy(lev.downstairs, downs, sizeof(coordinate) * down_c);
    draw_dungeon_from_file();
    free(downs);
    free(ups);
    free(room_data);
    // free(semantic);
    return false;
}

bool write_file(FILE *f)
{
    draw_dungeon();
    char semantic[] = "RLG327-S2025";
    int32_t version = 0;
    int filesize = 1708 + (4 * lev.num_rooms) + (lev.num_up_stair * 2) + (lev.num_dn_stair * 2);
    int filesize_be = htobe32(filesize);
    fwrite(semantic, sizeof(char), 12, f);
    fwrite(&version, sizeof(int), 1, f);
    fwrite(&filesize_be, sizeof(int), 1, f);
    int8_t play_x = lev.player.x;
    fwrite(&play_x, 1, 1, f);
    int8_t play_y = lev.player.y;
    fwrite(&play_y, 1, 1, f);
    fwrite(&lev.hardness_map, 1, 1680, f);
    int16_t be_num_rooms = htobe16(lev.num_rooms);
    fwrite(&be_num_rooms, 2, 1, f);
    for (int i = 0; i < lev.num_rooms; i++)
    {
        fwrite(&lev.rooms[i].start.x, 1, 1, f);
        fwrite(&lev.rooms[i].start.y, 1, 1, f);
        fwrite(&lev.rooms[i].width, 1, 1, f);
        fwrite(&lev.rooms[i].height, 1, 1, f);
    }
    int16_t num_up_be = htobe16(lev.num_up_stair);
    fwrite(&num_up_be, 2, 1, f);
    for (int i = 0; i < lev.num_up_stair; i++)
    {
        fwrite(&lev.upstairs[i].x, 1, 1, f);
        fwrite(&lev.upstairs[i].y, 1, 1, f);
    }
    int16_t num_dn_be = htobe16(lev.num_dn_stair);
    fwrite(&num_dn_be, 2, 1, f);
    for (int i = 0; i < lev.num_dn_stair; i++)
    {
        fwrite(&lev.downstairs[i].x, 1, 1, f);
        fwrite(&lev.downstairs[i].y, 1, 1, f);
    }
    return false;
}