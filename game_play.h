#ifndef GAME_PLAY_H
#define GAME_PLAY_H

#include <stdint.h>
#include "lcd.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "button.h"
#include <stdlib.h>
#include "stm32f10x.h"

#define PLAYER_SPEED 3
#define SHOOT_PROBABILITY 3 // Xác suât 10% môi khung hình ke thù ban

// Khai báo bi?n toàn c?c v?i t? khóa extern
extern Player player;                     
extern Bullet bullets[MAX_BULLETS];       
extern uint8_t bulletCount;               
extern Enemy enemies[MAX_ENEMIES];        
extern uint8_t enemyCount;                
extern Bullet enemyBullets[MAX_BULLETS];  
extern uint8_t enemyBulletCount;





void Gameplay_Init(void);
void createPlayerBullet(Player *player, Bullet *bullets, uint8_t *bulletCount);
void createEnemyBullet(Enemy *enemy, Bullet *enemyBullets, uint8_t *enemyBulletCount);
void spawnEnemies(Enemy *enemies, uint8_t *enemyCount);
void updateEnemyPositions(Enemy *enemies, uint8_t enemyCount);
void checkPlayerBulletCollisions(Bullet *bullets, uint8_t *bulletCount, Enemy *enemies, uint8_t *enemyCount);
void checkEnemyBulletCollisions(Bullet *enemyBullets, uint8_t *enemyBulletCount, Player *player);
void checkPlayerEnemyCollisions(Player *player, Enemy *enemies, uint8_t *enemyCount);
void updatePlayerBullets(Bullet *bullets, uint8_t *bulletCount);
void updateEnemyBullets(Bullet *enemyBullets, uint8_t *enemyBulletCount);
void cleanUpBullets(Bullet *bullets, uint8_t *bulletCount);
void cleanUpEnemies(Enemy *enemies, uint8_t *enemyCount);
void handlePlayerInput(Player *player, Bullet *bullets, uint8_t *bulletCount);
void enemyShootRandomly(Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t *enemyBulletCount);

#endif