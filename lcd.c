#include "stm32f10x.h"
#include "font.h"
#include "stdint.h"
#include <stdio.h>
#include "lcd.h"




// C�u h�nh GPIO
void GPIO_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // CS, DC, RST (PA1, PA2, PA3) -> Output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // SCL (PA5), SDA (PA7) -> Alternate function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void SPI_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitTypeDef SPI_InitStruct;

    SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
		SPI1->CR1 &= ~SPI_CR1_DFF;
    SPI_Cmd(SPI1, ENABLE);
}
void SPI_SendData(uint8_t data) {
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); // Wait TXE (Transmit buffer empty)
    SPI_I2S_SendData(SPI1, data);
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)); // Wait SPI not busy
}


void ST7735_SendCommand(uint8_t cmd) {
    CS_LOW();
    DC_COMMAND();
    SPI_SendData(cmd);
    CS_HIGH();
		//for (volatile int i = 0; i < 15000; i++); // Delay
}

void ST7735_SendData(uint8_t data) {
    CS_LOW();
    DC_DATA();
    SPI_SendData(data);
    CS_HIGH();
		//for (volatile int i = 0; i < 15000; i++); // Delay
}
void ST7735_Init(void) {
    // Reset m�n h�nh
    RST_LOW();
    for (volatile int i = 0; i < 100000; i++); // Delay
    RST_HIGH();
    for (volatile int i = 0; i < 100000; i++); // Delay

    // L?nh kh?i t?o (tham kh?o datasheet)
    ST7735_SendCommand(0x01); // Software reset
    for (volatile int i = 0; i < 2160000; i++); // Delay 150ms

    ST7735_SendCommand(0x11); // Exit sleep mode
    for (volatile int i = 0; i < 2160000; i++); // Delay 150ms

    ST7735_SendCommand(0x3A); // Interface Pixel Format
    ST7735_SendData(0x05);    // 16-bit color (RGB565)
		
    ST7735_SendCommand(0x36); // Memory Data Access Control
    ST7735_SendData(0xC8);    // MADCTL: row/column exchange, RGB
		
    ST7735_SendCommand(0x29); // Display ON
    for (volatile int i = 0; i < 2160000; i++); // Delay
}
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    ST7735_SendCommand(0x2A); // Column address set
    ST7735_SendData(0x00);
    ST7735_SendData(x0);
    ST7735_SendData(0x00);
    ST7735_SendData(x1);

    ST7735_SendCommand(0x2B); // Row address set
    ST7735_SendData(0x00);
    ST7735_SendData(y0);
    ST7735_SendData(0x00);
    ST7735_SendData(y1);

    ST7735_SendCommand(0x2C); // Memory write
}

void ST7735_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
    ST7735_SetAddressWindow(x, y, x, y); // Set window to 1 pixel
    ST7735_SendData(color >> 8);        // Send high byte
    ST7735_SendData(color & 0xFF);      // Send low byte
}


void ST7735_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1); // Set window

    for (uint16_t i = 0; i < w * h; i++) {
        ST7735_SendData(color >> 8);   // High byte
        ST7735_SendData(color & 0xFF); // Low byte
    }
}
void ST7735_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bgcolor) {
    if (ch < 0 || ch > 126) return; // K� tu kh�ng hop le
    const uint8_t *char_data = Font5x7[(int)ch]; // Lay du lieu font cua k� tu

    for (uint8_t col = 0; col < 5; col++) { // 5 c�t cua k� tu
        uint8_t line = char_data[col];
        for (uint8_t row = 0; row < 8; row++) { // 7 h�ng + 1 h�ng trong
            if (line & 0x01) { // Bit 1 => pixel s�ng
                ST7735_DrawPixel(x + col, y + row, color);
            } else { // Bit 0 => pixel tat
                ST7735_DrawPixel(x + col, y + row, bgcolor);
            }
            line >>= 1;
        }
    }
}
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bgcolor) {
    while (*str) {
        ST7735_DrawChar(x, y, *str, color, bgcolor);
        x += 6; // Moi k� tu chiem 6 pixel (5 pixel + 1 pixel khoang c�ch)
        str++;
    }
}

void uintToStr(unsigned int num, char *str) {
    int i = 0;

    // Chuy�n tang chi s� th�nh k� tu (nguoc)
    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    // Th�m k� tu ket th�c chuoi
    str[i] = '\0';

    // �ao chu�i (dang nguoc)
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void ST7735_Clear(uint16_t color) {
    // �at to�n b� m�n h�nh trong v�ng cua so (full window)
    ST7735_SetAddressWindow(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);

    // Gui du lieu m�u lop day to�n bo m�n h�nh
    for (uint16_t i = 0; i < ST7735_WIDTH; i++) {
        for (uint16_t j = 0; j < ST7735_HEIGHT; j++) {
            ST7735_SendData(color >> 8); // Gui byte cao cua m�u
            ST7735_SendData(color & 0xFF); // Gui byte thap cua m�u
        }
    }
}

int16_t const smallCircle[6][3]={
    {2,3,    2},
  {1  ,  4,  4},
 {0   ,   5, 6},
 {0   ,   5, 6},
  {1  ,  4,  4},
    {2,3,    2}};

static int16_t _width = ST7735_WIDTH;   // this could probably be a constant, except it is used in Adafruit_GFX and depends on image rotation
static int16_t _height = ST7735_HEIGHT;
void ST7735_DrawSmallCircle(int16_t x, int16_t y, uint16_t color) {
  uint32_t i,w;
  uint8_t hi = color >> 8, lo = color;
  // rudimentary clipping 
  if((x>_width-5)||(y>_height-5)) return; // doesn't fit
  for(i=0; i<6; i++){
    ST7735_SetAddressWindow(x+smallCircle[i][0], y+i, x+smallCircle[i][1], y+i);
    w = smallCircle[i][2];
    while (w--) {
      ST7735_SendData(hi);
      ST7735_SendData(lo);
    }
  }
}
int16_t const circle[10][3]={
      { 4,5,         2},
    {2   ,   7,      6},
  {1     ,      8,   8},
  {1     ,      8,   8},
 {0      ,       9, 10},
 {0      ,       9, 10},
  {1     ,      8,   8},
  {1     ,      8,   8},
    {2   ,    7,     6},
     {  4,5,         2}};
void ST7735_DrawCircle(int16_t x, int16_t y, uint16_t color) {
  uint32_t i,w;
  uint8_t hi = color >> 8, lo = color;
  // rudimentary clipping 
  if((x>_width-9)||(y>_height-9)) return; // doesn't fit
  for(i=0; i<10; i++){
    ST7735_SetAddressWindow(x+circle[i][0], y+i, x+circle[i][1], y+i);
    w = circle[i][2];
    while (w--) {
      ST7735_SendData(hi);
      ST7735_SendData(lo);
    }
  }
}

// H�m ve Main Menu voi lua chon duoc d�nh d�u
void DrawMainMenu(uint8_t selectedOption) {
    // X�a to�n b� m�n h�nh ve m�u den
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK);

    // Ve ti�u d� "Welcome to Alien Invaders!"
    ST7735_DrawString(10, 10, "Welcome to", COLOR_WHITE, COLOR_BLACK);
    ST7735_DrawString(10, 20, "Alien Invaders!", COLOR_WHITE, COLOR_BLACK);

    // Ve n�t Play
    if (selectedOption == 1) {
        ST7735_FillRect(40, 50, 80, 20, COLOR_WHITE); // Nen n�t Play m�u trang (dang chon)
        ST7735_DrawString(50, 58, "Play", COLOR_BLACK, COLOR_WHITE); // Text m�u den tr�n nen trang
    } else {
        ST7735_FillRect(40, 50, 80, 20, COLOR_GREEN); // Nen n�t Play m�u xanh
        ST7735_DrawString(50, 58, "Play", COLOR_BLACK, COLOR_GREEN); // Text m�u den tr�n nen xanh
    }

    // Ve n�t Manual
    if (selectedOption == 2) {
        ST7735_FillRect(40, 80, 80, 20, COLOR_WHITE); // N�n n�t Manual m�u trang (dang chon)
        ST7735_DrawString(50, 88, "Manual", COLOR_BLACK, COLOR_WHITE); // Text m�u den tr�n n�n trang
    } else {
        ST7735_FillRect(40, 80, 80, 20, COLOR_BLUE); // N�n n�t Manual m�u xanh duong
        ST7735_DrawString(50, 88, "Manual", COLOR_WHITE, COLOR_BLUE); // Text m�u trang tr�n n�n xanh duong
    }

    // Ve n�t Exit
    if (selectedOption == 3) {
        ST7735_FillRect(40, 110, 80, 20, COLOR_WHITE); // N�n n�t Exit m�u trang (dang chon)
        ST7735_DrawString(50, 118, "Exit", COLOR_BLACK, COLOR_WHITE); // Text m�u den tr�n n�n trang
    } else {
        ST7735_FillRect(40, 110, 80, 20, COLOR_RED); // N�n n�t Exit m�u do
        ST7735_DrawString(50, 118, "Exit", COLOR_WHITE, COLOR_RED); // Text m�u trang tr�n n�n do
    }
}

uint8_t currentMenuOption = 0; // Luu trang th�i cua lua chon hien tai

void updateMenuLCD(uint8_t selectedOption) {
    if (selectedOption == currentMenuOption) {
        // Kh�ng can cap nhat neu lua chon kh�ng thay doi
        return;
    }

    // Cap nhat n�t cu ve trang th�i kh�ng duoc chon
    switch (currentMenuOption) {
        case 1: // N�t Play
            ST7735_FillRect(40, 50, 80, 20, COLOR_GREEN);
            ST7735_DrawString(50, 58, "Play", COLOR_BLACK, COLOR_GREEN);
            break;
        case 2: // N�t Manual
            ST7735_FillRect(40, 80, 80, 20, COLOR_BLUE);
            ST7735_DrawString(50, 88, "Manual", COLOR_WHITE, COLOR_BLUE);
            break;
        case 3: // N�t Exit
            ST7735_FillRect(40, 110, 80, 20, COLOR_RED);
            ST7735_DrawString(50, 118, "Exit", COLOR_WHITE, COLOR_RED);
            break;
    }

    // Cap nhat n�t moi ve trang th�i duoc chon
    switch (selectedOption) {
        case 1: // N�t Play
            ST7735_FillRect(40, 50, 80, 20, COLOR_WHITE);
            ST7735_DrawString(50, 58, "Play", COLOR_BLACK, COLOR_WHITE);
            break;
        case 2: // N�t Manual
            ST7735_FillRect(40, 80, 80, 20, COLOR_WHITE);
            ST7735_DrawString(50, 88, "Manual", COLOR_BLACK, COLOR_WHITE);
            break;
        case 3: // N�t Exit
            ST7735_FillRect(40, 110, 80, 20, COLOR_WHITE);
            ST7735_DrawString(50, 118, "Exit", COLOR_BLACK, COLOR_WHITE);
            break;
    }

    // Luu lai trang th�i moi
    currentMenuOption = selectedOption;
}


void DrawGameScreen(uint8_t level, uint8_t enemyX, uint8_t enemyY, uint8_t playerX, uint8_t playerY, uint8_t bulletX, uint8_t bulletY) {
    // X�a to�n b� m�n h�nh v� m�u den
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK);

    // Hi�n thi Level
    char levelText[16];
    sprintf(levelText, "Level %d:", level);
    ST7735_DrawString(10, 10, levelText, COLOR_YELLOW, COLOR_BLACK);

    // Ve ke th� (h�nh chu nh�t)
    ST7735_FillRect(enemyX, enemyY, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_RED);

    // Ve nh�n v�t choi (h�nh chu nh�t)
    ST7735_FillRect(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_GREEN);

    // Ve dan

    ST7735_DrawSmallCircle(bulletX, bulletY, COLOR_BLUE);

}

void DrawGameScreen_2(uint8_t level, uint8_t enemyX, uint8_t enemyY, uint8_t playerX, uint8_t playerY, uint8_t bulletX, uint8_t bulletY, uint8_t enemyBulletX, uint8_t enemyBulletY) {
    // X�a to�n b� m�n h�nh v� m�u den
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK);

    // Hi�n thi Level
    char levelText[16];
    sprintf(levelText, "Level %d:", level);
    ST7735_DrawString(10, 10, levelText, COLOR_YELLOW, COLOR_BLACK);

    // Ve ke th� (h�nh chu nh�t)
    ST7735_FillRect(enemyX, enemyY, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_RED);

    // Ve nh�n v�t choi (h�nh chu nh�t)
    ST7735_FillRect(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_GREEN);

    // Ve dan cua nguoi choi
    ST7735_DrawSmallCircle(bulletX, bulletY, COLOR_BLUE);

    // Ve dan cua ke th�
    ST7735_DrawSmallCircle(enemyBulletX, enemyBulletY, COLOR_WHITE);

}


// H�m ve m�n h�nh k�t th�c (Game Over/Win Screen)
void DrawEndScreen(uint8_t isWin, uint16_t finalScore, uint8_t selectedOption) {
    // X�a to�n b� m�n h�nh v� m�u den
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK);

    // Hi�n thi th�ng b�o th�ng hoac thua
    if (isWin) {
        ST7735_DrawString(40, 20, "You Win!", COLOR_GREEN, COLOR_BLACK);
    } else {
        ST7735_DrawString(40, 20, "Game Over", COLOR_RED, COLOR_BLACK);
    }

    // Hi�n thi di�m s� cu�i c�ng
    char scoreText[16];
    sprintf(scoreText, "Score: %d", finalScore);
    ST7735_DrawString(30, 50, scoreText, COLOR_YELLOW, COLOR_BLACK);

    // Ve n�t Restart
    if (selectedOption == 1) {
        ST7735_FillRect(30, 80, 60, 20, COLOR_WHITE); // N�n n�t Restart m�u trang (dang chon)
        ST7735_DrawString(40, 88, "Restart", COLOR_BLACK, COLOR_WHITE); // Text m�u den tr�n n�n trang
    } else {
        ST7735_FillRect(30, 80, 60, 20, COLOR_GREEN); // N�n n�t Restart m�u xanh
        ST7735_DrawString(40, 88, "Restart", COLOR_BLACK, COLOR_GREEN); // Text m�u den tr�n n�n xanh
    }

    // Ve n�t Exit
    if (selectedOption == 2) {
        ST7735_FillRect(30, 110, 60, 20, COLOR_WHITE); // N�n n�t Exit m�u trang (dang chon)
        ST7735_DrawString(40, 118, "Exit", COLOR_BLACK, COLOR_WHITE); // Text m�u den tr�n n�n trang
    } else {
        ST7735_FillRect(30, 110, 60, 20, COLOR_RED); // N�n n�t Exit m�u do
        ST7735_DrawString(40, 118, "Exit", COLOR_WHITE, COLOR_RED); // Text m�u trang tr�n n�n do
    }
}

uint8_t currentEndOption = 0;

void updateEndLCD(uint8_t selectedOption) {
    // Ch? c?p nh?t n?u l?a ch?n thay d?i
    if (selectedOption != currentEndOption) {
        // C?p nh?t n�t Restart
        if (selectedOption == 1) {
            ST7735_FillRect(30, 80, 60, 20, COLOR_WHITE); // N?n tr?ng (dang ch?n)
            ST7735_DrawString(40, 88, "Restart", COLOR_BLACK, COLOR_WHITE);
        } else {
            ST7735_FillRect(30, 80, 60, 20, COLOR_GREEN); // N?n xanh
            ST7735_DrawString(40, 88, "Restart", COLOR_BLACK, COLOR_GREEN);
        }

        // C?p nh?t n�t Exit
        if (selectedOption == 2) {
            ST7735_FillRect(30, 110, 60, 20, COLOR_WHITE); // N?n tr?ng (dang ch?n)
            ST7735_DrawString(40, 118, "Exit", COLOR_BLACK, COLOR_WHITE);
        } else {
            ST7735_FillRect(30, 110, 60, 20, COLOR_RED); // N?n d?
            ST7735_DrawString(40, 118, "Exit", COLOR_WHITE, COLOR_RED);
        }

        // C?p nh?t `currentEndOption`
        currentEndOption = selectedOption;
    }
}

// Function to display a manual guide on the ST7735 LCD
void DrawManual() {
    // Clear the screen and set background
    ST7735_FillRect(0, 0, 128, 160, COLOR_BLACK); // X�a m�n h�nh (m�u den)

    // Title
    ST7735_DrawString(10, 2, "MANUAL", COLOR_WHITE,COLOR_BLACK);
    //ST7735_DrawLine(0, 12, 128, 12, COLOR_WHITE); // Underline for title

    // Instructions
    ST7735_DrawString(5, 20, "Button 1 (Left)", COLOR_CYAN, COLOR_BLACK);
    ST7735_DrawString(5, 35, "Button 2 (Right)", COLOR_CYAN, COLOR_BLACK);
    ST7735_DrawString(5, 50, "Button 3 (Up)", COLOR_CYAN, COLOR_BLACK);
    ST7735_DrawString(5, 65, "Button 4 (Down)", COLOR_CYAN, COLOR_BLACK);
    ST7735_DrawString(5, 80, "Button 5 (Select)", COLOR_CYAN, COLOR_BLACK);

    // Footer or additional info
    //ST7735_DrawLine(0, 120, 128, 120, COLOR_WHITE); // Separator line
    ST7735_DrawString(15, 130, "Press Select if OK", COLOR_YELLOW, COLOR_BLACK);
}

void DrawGameFullScreen(uint8_t level, Player *player, Bullet *playerBullets, uint8_t playerBulletCount, 
                        Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t enemyBulletCount) {
    

    // Ve nguoi choi
    ST7735_FillRect(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_BLUE);

    // Ve dan cua nguoi choi
    for (uint8_t i = 0; i < playerBulletCount; i++) {
        if (playerBullets[i].isActive) {
            ST7735_DrawSmallCircle(playerBullets[i].x, playerBullets[i].y, COLOR_YELLOW);
        }
    }

    // Ve ke th�
    for (uint8_t i = 0; i < enemyCount; i++) {
        if (enemies[i].isActive) {
            ST7735_FillRect(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_RED);
        }
    }

    // Ve dan cua ke th�
    for (uint8_t i = 0; i < enemyBulletCount; i++) {
        if (enemyBullets[i].isActive) {
            ST7735_DrawSmallCircle(enemyBullets[i].x, enemyBullets[i].y, COLOR_GREEN);
        }
    }

    // Hi�n thi level
    char levelText[10];
    sprintf(levelText, "Level: %d", level);
    ST7735_DrawString(2, 2, levelText, COLOR_WHITE, COLOR_BLACK);
		
		ClearPlayer(player);
		for (uint8_t i = 0; i < enemyCount; i++) {
			if (enemies[i].isActive) {
        ClearEnemy(&enemies[i]);
			}
		}
		for (uint8_t i = 0; i < playerBulletCount; i++) {
				ClearBullet(&playerBullets[i]);
		}

		for (uint8_t i = 0; i < enemyBulletCount; i++) {
				ClearBullet(&enemyBullets[i]);
		}
}
void ClearPlayer(Player *player) {
    // X�a nguoi choi bang c�ch t� den vi tr� hien tai cua nguoi choi
    ST7735_FillRect(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_BLACK);
}
void ClearEnemy(Enemy *enemy) {
    // X�a ke th� bang c�ch t� den vi tr� hien tai cua ke th�
    ST7735_FillRect(enemy->x, enemy->y, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_BLACK);
}
void ClearBullet(Bullet *bullet) {
    // X�a vi�n dan bang c�ch t� den toi vi tr� hien tai
    if (bullet->isActive) {
        ST7735_DrawSmallCircle(bullet->x, bullet->y, COLOR_BLACK);
    }
}

		
Player previousPlayer;
Bullet previousPlayerBullets[MAX_BULLETS];
Enemy previousEnemies[MAX_ENEMIES];
Bullet previousEnemyBullets[MAX_BULLETS];

void updatePlayLCD(uint8_t level, Player *player, Bullet *playerBullets, uint8_t playerBulletCount,
                   Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t enemyBulletCount) {
    // So s�nh v� c?p nh?t player
    if (player->x != previousPlayer.x || player->y != previousPlayer.y) {
        // X�a v? tr� cu
        ST7735_FillRect(previousPlayer.x, previousPlayer.y, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_BLACK);
        // V? v? tr� m?i
        ST7735_FillRect(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, COLOR_BLUE);
        // C?p nh?t tr?ng th�i tru?c d�
        previousPlayer = *player;
    }

    // So s�nh v� c?p nh?t d?n c?a ngu?i choi
    for (uint8_t i = 0; i < playerBulletCount; i++) {
        if (playerBullets[i].isActive != previousPlayerBullets[i].isActive ||
            playerBullets[i].x != previousPlayerBullets[i].x ||
            playerBullets[i].y != previousPlayerBullets[i].y) {
            // X�a d?n cu n?u c�
            if (previousPlayerBullets[i].isActive) {
                ST7735_DrawSmallCircle(previousPlayerBullets[i].x, previousPlayerBullets[i].y, COLOR_BLACK);
            }
            // V? d?n m?i n?u c�n ho?t d?ng
            if (playerBullets[i].isActive) {
                ST7735_DrawSmallCircle(playerBullets[i].x, playerBullets[i].y, COLOR_YELLOW);
            }
            // C?p nh?t tr?ng th�i tru?c d�
            previousPlayerBullets[i] = playerBullets[i];
        }
    }

    // So s�nh v� c?p nh?t k? th�
    for (uint8_t i = 0; i < enemyCount; i++) {
        if (enemies[i].isActive != previousEnemies[i].isActive ||
            enemies[i].x != previousEnemies[i].x ||
            enemies[i].y != previousEnemies[i].y) {
            // X�a v? tr� cu
            if (previousEnemies[i].isActive) {
                ST7735_FillRect(previousEnemies[i].x, previousEnemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_BLACK);
            }
            // V? v? tr� m?i
            if (enemies[i].isActive) {
                ST7735_FillRect(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, COLOR_RED);
            }
            // C?p nh?t tr?ng th�i tru?c d�
            previousEnemies[i] = enemies[i];
        }
    }

    // So s�nh v� c?p nh?t d?n c?a k? th�
    for (uint8_t i = 0; i < enemyBulletCount; i++) {
        if (enemyBullets[i].isActive != previousEnemyBullets[i].isActive ||
            enemyBullets[i].x != previousEnemyBullets[i].x ||
            enemyBullets[i].y != previousEnemyBullets[i].y) {
            // X�a d?n cu n?u c�
            if (previousEnemyBullets[i].isActive) {
                ST7735_DrawSmallCircle(previousEnemyBullets[i].x, previousEnemyBullets[i].y, COLOR_BLACK);
            }
            // V? d?n m?i n?u c�n ho?t d?ng
            if (enemyBullets[i].isActive) {
                ST7735_DrawSmallCircle(enemyBullets[i].x, enemyBullets[i].y, COLOR_GREEN);
            }
            // C?p nh?t tr?ng th�i tru?c d�
            previousEnemyBullets[i] = enemyBullets[i];
        }
    }

    // Hi?n th? level n?u c?n (kh�ng c?n t?i uu h�a trong tru?ng h?p n�y)
    char levelText[10];
    sprintf(levelText, "Level: %d", level);
    ST7735_DrawString(2, 2, levelText, COLOR_WHITE, COLOR_BLACK);
}
