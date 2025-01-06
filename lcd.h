#ifndef LCD_H
#define LCD_H
#include <stdint.h>
#include "bullet.h"
#include "player.h"
#include "enemy.h"

#define CS_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define CS_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define DC_COMMAND() GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define DC_DATA()    GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define RST_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define RST_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_3)

#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160


// Color definitions
#define COLOR_BLACK   0x0000
#define COLOR_BLUE    0xF800
#define COLOR_RED     0x001F
#define COLOR_GREEN   0x07E0
#define COLOR_CYAN    0xFFE0
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW  0x07FF
#define COLOR_WHITE   0xFFFF

// kich thuoc
#define ENEMY_WIDTH 20
#define ENEMY_HEIGHT 15
#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 10

extern Player previousPlayer;
extern Bullet previousPlayerBullets[MAX_BULLETS];
extern Enemy previousEnemies[MAX_ENEMIES];
extern Bullet previousEnemyBullets[MAX_BULLETS];

extern uint8_t currentMenuOption;
extern uint8_t currentEndOption;

void GPIO_Config(void);
void SPI_Config(void);
void SPI_SendData(uint8_t data);
void ST7735_SendCommand(uint8_t cmd);
void ST7735_SendData(uint8_t data);
void ST7735_Init(void);
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void ST7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void ST7735_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bgcolor);
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bgcolor);
void uintToStr(unsigned int num, char *str);
void ST7735_Clear(uint16_t color);
void ST7735_DrawSmallCircle(int16_t x, int16_t y, uint16_t color);
void ST7735_DrawCircle(int16_t x, int16_t y, uint16_t color);

void DrawMainMenu(uint8_t selectedOption);
void updateMenuLCD(uint8_t selectedOption);

void DrawGameScreen(uint8_t level, uint8_t enemyX, uint8_t enemyY, uint8_t playerX, uint8_t playerY, uint8_t bulletX, uint8_t bulletY);
void DrawGameScreen_2(uint8_t level, uint8_t enemyX, uint8_t enemyY, uint8_t playerX, uint8_t playerY, uint8_t bulletX, uint8_t bulletY, uint8_t enemyBulletX, uint8_t enemyBulletY);
void DrawEndScreen(uint8_t isWin, uint16_t finalScore, uint8_t selectedOption);
void updateEndLCD(uint8_t selectedOption);
void DrawManual(void);
void DrawGameFullScreen(uint8_t level, Player *player, Bullet *playerBullets, uint8_t playerBulletCount, Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t enemyBulletCount);
void ClearPlayer(Player *player);
void ClearEnemy(Enemy *enemy);
void ClearBullet(Bullet *bullet);
void updatePlayLCD(uint8_t level, Player *player, Bullet *playerBullets, uint8_t playerBulletCount, Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t enemyBulletCount);
#endif