// bullet.h
#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>
#define MAX_BULLETS 10

// Struct dinh nghia thông tin cua dan
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t isActive;
} Bullet;

// Khoi tao danh sách dan
void Bullet_Init(Bullet bullets[], uint8_t count);

// Cap nhat vi trí dan
void Bullet_Update(Bullet bullets[], uint8_t count);

#endif // BULLET_H