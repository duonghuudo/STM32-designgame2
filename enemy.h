// enemy.h
#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#define MAX_ENEMIES 5

// Struct dinh nghia th�ng tin cua ke th�
typedef struct {
    uint8_t x;
    uint8_t y;
		int8_t directionX; // Huong di chuyen ngang (1: phai, -1: tr�i)
    int8_t directionY; // Huong di chuyen doc (1: xu�ng, -1: l�n)
		uint8_t health;
    uint8_t isActive;
} Enemy;

// Khoi tao danh s�ch ke th�
void Enemy_Init(Enemy enemies[], uint8_t count);

// Cap nhat vi tr� v� trang th�i cua ke th�
void Enemy_Update(Enemy enemies[], uint8_t count);

#endif // ENEMY_H