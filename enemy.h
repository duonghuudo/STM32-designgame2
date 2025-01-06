// enemy.h
#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#define MAX_ENEMIES 5

// Struct dinh nghia thông tin cua ke thù
typedef struct {
    uint8_t x;
    uint8_t y;
		int8_t directionX; // Huong di chuyen ngang (1: phai, -1: trái)
    int8_t directionY; // Huong di chuyen doc (1: xuông, -1: lên)
		uint8_t health;
    uint8_t isActive;
} Enemy;

// Khoi tao danh sách ke thù
void Enemy_Init(Enemy enemies[], uint8_t count);

// Cap nhat vi trí và trang thái cua ke thù
void Enemy_Update(Enemy enemies[], uint8_t count);

#endif // ENEMY_H