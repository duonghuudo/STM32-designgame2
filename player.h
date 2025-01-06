// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

// Struct dinh nghia thông tin cua nguoi choi
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t health;
} Player;

// Khoi tao nguoi choi
void Player_Init(Player *player);

// Cap nhat vi trí nguoi choi dua vào dau vào
void Player_Update(Player *player, int input);

#endif // PLAYER_H