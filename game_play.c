#include "game_play.h"

// �inh nghia bi�n to�n cuc
Player player;                     
Bullet bullets[MAX_BULLETS];       
uint8_t bulletCount = 0;           
Enemy enemies[MAX_ENEMIES];        
uint8_t enemyCount = MAX_ENEMIES;  
Bullet enemyBullets[MAX_BULLETS];  
uint8_t enemyBulletCount = 0;      




// H�m khoi to tr� choi
void Gameplay_Init() {
    // Khoi tao nguoi choi
    //Player_Init(&player);
    player.x = 64;  // Giua m�n h�nh (LCD_WIDTH / 2)
    player.y = 140; // Duoi m�n h�nh (g?n LCD_HEIGHT)
    player.health = 3;

    // Khoi tao ke th�
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].x = i * 20 + 10;  // Ph�n bo deu theo chieu ngang
        enemies[i].y = 10;           // o tr�n c�ng m�n h�nh
        enemies[i].health = 1;
        enemies[i].isActive = 1;
				enemies[i].directionX = (i % 2 == 0) ? 1 : -1; // Huong ban dau: tr�i hoac phai
				enemies[i].directionY = 1; // Bat dau di chuyen xuong
    }

    // Khoi tao so luong dan ban dau
    bulletCount = 0;
    enemyBulletCount = 0;
}


// H�m khoi tao vi tr� dan nguoi choi
void createPlayerBullet(Player *player, Bullet *bullets, uint8_t *bulletCount) {
    if (*bulletCount < MAX_BULLETS) {
        bullets[*bulletCount].x = player->x + PLAYER_WIDTH / 2;
        bullets[*bulletCount].y = player->y - 2;
        bullets[*bulletCount].isActive = 1;
        (*bulletCount)++;
    }
}

// H�m khoi tao vi tr� dan ke th�
void createEnemyBullet(Enemy *enemy, Bullet *enemyBullets, uint8_t *enemyBulletCount) {
    if (*enemyBulletCount < MAX_BULLETS) {
        enemyBullets[*enemyBulletCount].x = enemy->x + ENEMY_WIDTH / 2;
        enemyBullets[*enemyBulletCount].y = enemy->y + ENEMY_HEIGHT;
        enemyBullets[*enemyBulletCount].isActive = 1;
        (*enemyBulletCount)++;
    }
}

// H�m khoi tao vi tr� ke th�
void spawnEnemies(Enemy *enemies, uint8_t *enemyCount) {
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].x = (i * (ENEMY_WIDTH + 5)) % ST7735_WIDTH;
        enemies[i].y = (i / (ST7735_WIDTH / (ENEMY_WIDTH + 5))) * (ENEMY_HEIGHT + 5);
        enemies[i].health = 1;
        enemies[i].isActive = 1;
    }
    *enemyCount = MAX_ENEMIES;
}

// H�m cap nhat vi tr� ke th�
void updateEnemyPositions(Enemy *enemies, uint8_t enemyCount) {
    for (uint8_t i = 0; i < enemyCount; i++) {
        if (enemies[i].isActive) {
            // Di chuy?n ngang (x)
            if (enemies[i].x >= ST7735_WIDTH - ENEMY_WIDTH || enemies[i].x <= 0) {
                // N?u ch?m c?nh ngang, d?i hu?ng
                enemies[i].directionX *= -1; // �?i hu?ng x
                enemies[i].y += ENEMY_HEIGHT / 2; // Di chuy?n xu?ng m?t d�ng
            }

            // Di chuy?n d?c (y)
            if (enemies[i].y >= ST7735_HEIGHT - ENEMY_HEIGHT || enemies[i].y <= 0) {
                // N?u ch?m c?nh d?c, d?i hu?ng
                enemies[i].directionY *= -1; // �?i hu?ng y
            }

            // C?p nh?t v? tr� x, y
            enemies[i].x += enemies[i].directionX;
            enemies[i].y += enemies[i].directionY;
        }
    }
}


// H�m kiem tra va cham dan nguoi choi -> ke th�
void checkPlayerBulletCollisions(Bullet *bullets, uint8_t *bulletCount, Enemy *enemies, uint8_t *enemyCount) {
    for (uint8_t i = 0; i < *bulletCount; i++) {
        if (bullets[i].isActive) {
            for (uint8_t j = 0; j < *enemyCount; j++) {
                if (enemies[j].isActive &&
                    bullets[i].x >= enemies[j].x &&
                    bullets[i].x <= enemies[j].x + ENEMY_WIDTH &&
                    bullets[i].y >= enemies[j].y &&
                    bullets[i].y <= enemies[j].y + ENEMY_HEIGHT) {

                    // Ke th� bi ti�u diet
                    enemies[j].isActive = 0;
                    bullets[i].isActive = 0;
                }
            }
        }
    }
}

// H�m ki�m tra va cham dan ke th� -> nguoi choi
void checkEnemyBulletCollisions(Bullet *enemyBullets, uint8_t *enemyBulletCount, Player *player) {
    for (uint8_t i = 0; i < *enemyBulletCount; i++) {
        if (enemyBullets[i].isActive &&
            enemyBullets[i].x >= player->x &&
            enemyBullets[i].x <= player->x + PLAYER_WIDTH &&
            enemyBullets[i].y >= player->y &&
            enemyBullets[i].y <= player->y + PLAYER_HEIGHT) {

            // Nguoi choi bi t�n c�ng
            player->health--;
            enemyBullets[i].isActive = 0;
        }
    }
}

void checkPlayerEnemyCollisions(Player *player, Enemy *enemies, uint8_t *enemyCount) {
    for (uint8_t i = 0; i < *enemyCount; i++) {
        if (enemies[i].isActive &&
            player->x < enemies[i].x + ENEMY_WIDTH &&
            player->x + PLAYER_WIDTH > enemies[i].x &&
            player->y < enemies[i].y + ENEMY_HEIGHT &&
            player->y + PLAYER_HEIGHT > enemies[i].y) {

            // Va cham xay ra
            player->health--;      // Giam mang nguoi choi
            enemies[i].isActive = 0; // Ke th� bi loai bo

            // Tho�t neu nguoi choi het mang
            if (player->health <= 0) {
                break;
            }
        }
    }
}

void updatePlayerBullets(Bullet *bullets, uint8_t *bulletCount) {
    for (uint8_t i = 0; i < *bulletCount; i++) {
        if (bullets[i].isActive) {
            bullets[i].y -= 2; // �an bay l�n
            
            // Ki�m tra n�u dan vuot ra ngo�i m�n h�nh
            if (bullets[i].y < 0) {
                bullets[i].isActive = 0; // Huy vi�n dan
            }
        }
    }
}

void updateEnemyBullets(Bullet *enemyBullets, uint8_t *enemyBulletCount) {
    for (uint8_t i = 0; i < *enemyBulletCount; i++) {
        if (enemyBullets[i].isActive) {
            enemyBullets[i].y += 2; // �an roi xu�ng
            
            // Ki�m tra n�u dan vuot ra ngo�i m�n h�nh
            if (enemyBullets[i].y > ST7735_HEIGHT) {
                enemyBullets[i].isActive = 0; // Huy vi�n dan
            }
        }
    }
}

void cleanUpBullets(Bullet *bullets, uint8_t *bulletCount) {
    uint8_t activeIndex = 0;
    for (uint8_t i = 0; i < *bulletCount; i++) {
        if (bullets[i].isActive) {
            // Giu lai c�c vi�n dan c�n hoat d�ng
            if (i != activeIndex) {
                bullets[activeIndex] = bullets[i];
            }
            activeIndex++;
        }
    }
    *bulletCount = activeIndex; // C�p nh�t s� luong dan c�n hoat d�ng
}

void cleanUpEnemies(Enemy *enemies, uint8_t *enemyCount) {
    uint8_t activeIndex = 0;
    for (uint8_t i = 0; i < *enemyCount; i++) {
        if (enemies[i].isActive) {
            // Giu lai c�c ke th� c�n hoat d�ng
            if (i != activeIndex) {
                enemies[activeIndex] = enemies[i];
            }
            activeIndex++;
        }
    }
    *enemyCount = activeIndex; // C�p nh�t so luong ke th� c�n hoat d�ng
}

void handlePlayerInput(Player *player, Bullet *bullets, uint8_t *bulletCount) {
    int button = getButtonInput();

    // Xu l� c�c trang th�i n�t
    switch (button) {
        case BUTTON_LEFT:
            if (player->x > 0) {
                player->x -= PLAYER_SPEED; // Di chuyen tr�i
            }
            break;
        case BUTTON_RIGHT:
            if (player->x < ST7735_WIDTH - PLAYER_WIDTH) {
                player->x += PLAYER_SPEED; // Di chuyen phai
            }
            break;
        case BUTTON_UP:
            if (player->y > 0) {
                player->y -= PLAYER_SPEED; // Di chuyen l�n
            }
            break;
        case BUTTON_DOWN:
            if (player->y < ST7735_HEIGHT - PLAYER_HEIGHT) {
                player->y += PLAYER_SPEED; // Di chuyen xuong
            }
            break;
        case BUTTON_SELECT:
            createPlayerBullet(player, bullets, bulletCount); // Tao dan
            break;
        default:
            // Kh�ng l�m g� khi kh�ng c� n�t n�o duoc bam
            break;
    }
}

void enemyShootRandomly(Enemy *enemies, uint8_t enemyCount, Bullet *enemyBullets, uint8_t *enemyBulletCount) {
    for (uint8_t i = 0; i < enemyCount; i++) {
        if (enemies[i].isActive && (rand() % 100 < SHOOT_PROBABILITY)) {
            createEnemyBullet(&enemies[i], enemyBullets, enemyBulletCount);
        }
    }
}

