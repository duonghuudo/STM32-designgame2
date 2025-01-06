#include "game_logic.h"

uint8_t isWin = 0;
uint16_t finalScore = 0;
uint8_t level = 1; // Giá tr? m?c d?nh

bool menuInitialized = false;
bool manualInitialized = false;
bool playInitialized = false;
bool overInitialized = false;

typedef struct {
    bool menuInitialized;
    bool manualInitialized;
		bool playInitialized;
		bool overInitialized;
    
} StateFlags;

StateFlags stateFlags = {false, false, false, false};

void resetStateFlags(StateFlags *flags) {
    flags->menuInitialized = false;
    flags->manualInitialized = false;
    flags->playInitialized = false;
		flags->overInitialized = false;
}

// Trang thái cua trò choi
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
		GAME_MANUAL,
    GAME_OVER
} GameState;

// Biên trang thái game
GameState gameState;
GameState previousState; // Trang thái truoc dó
int selectedOption = 1; // Lua chon ban dau trong menu

void Game_Init() {
    gameState = GAME_MENU; // Bat dâu voi màn hình menu
		previousState = GAME_OVER;
}

void Game_Loop() {
		if (gameState != previousState) {
            resetStateFlags(&stateFlags);
        }
    switch (gameState) {
        case GAME_MENU:
            handleMenu();
            break;
        case GAME_PLAYING:
            handleGamePlaying();
            break;
				case GAME_MANUAL:
						handleManual();
						break;
        case GAME_OVER:
            handleGameOver();
            break;
    }
		previousState = gameState;
}

void handleMenu() {
     // Ve menu voi lua chon hien tai
		if(gameState != previousState || !menuInitialized) {
			DrawMainMenu(selectedOption);
			menuInitialized = true;
		}
		else{
			updateMenuLCD(selectedOption);
		}
    int button = getButtonInput(); // Ðoc tín hieu tu nút bam

    if (button == BUTTON_UP) {
        selectedOption = (selectedOption - 1 < 1) ? 3 : selectedOption - 1; // Chuyen lên
    } else if (button == BUTTON_DOWN) {
        selectedOption = (selectedOption + 1 > 3) ? 1 : selectedOption + 1; // Chuyen xuong
    } else if (button == BUTTON_SELECT) {
        switch (selectedOption) {
            case 1: // Play
                gameState = GAME_PLAYING;
								menuInitialized = false;
                break;
            case 2: // Manual
								gameState = GAME_MANUAL;
								menuInitialized = false;
                break;
            case 3: // Exit
                ST7735_Clear(COLOR_BLACK);
								menuInitialized = false;
                while (1); // Ket thúc chuong trình
        }
    }
		
}

void handleGamePlaying() {
		if(gameState != previousState || !playInitialized) {
			// Xóa toàn bo màn hình voi nen den
			ST7735_FillRect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, COLOR_BLACK);
			playInitialized = true;
		}
		else{
			DrawGameFullScreen(level, &player, bullets, bulletCount, enemies, enemyCount, enemyBullets, enemyBulletCount); // Ve màn hình choi game day du
		}
		//DrawGameFullScreen(level, &player, bullets, bulletCount, enemies, enemyCount, enemyBullets, enemyBulletCount); // Ve màn hình choi game day du
	
    handlePlayerInput(&player, bullets, &bulletCount); // Xu lý nút bam cua nguoi choi

    //updatePlayerPosition(&player); // Câp nhat vi trí nguoi choi
    updateEnemyPositions(enemies, enemyCount); // Câp nhât vi trí ke thù
    updatePlayerBullets(bullets, &bulletCount); // Câp nhât vi trí dan cua nguoi choi
    updateEnemyBullets(enemyBullets, &enemyBulletCount); // Cap nhat vi trí dan cua ke thù

    enemyShootRandomly(enemies, enemyCount, enemyBullets, &enemyBulletCount); // Ke thù ban dan

    checkPlayerBulletCollisions(bullets, &bulletCount, enemies, &enemyCount); // Ðan nguoi choi ban trúng ke thù
    checkEnemyBulletCollisions(enemyBullets, &enemyBulletCount, &player);    // Ðan ke thù ban trúng nguoi choi
    checkPlayerEnemyCollisions(&player, enemies, &enemyCount);               // Va cham giua nguoi choi và ke thù

    cleanUpBullets(bullets, &bulletCount);             // Don dep dan nguoi choi ra khoi màn hình
    cleanUpBullets(enemyBullets, &enemyBulletCount);   // Don dap dan ke thù ra khoi màn hình
    cleanUpEnemies(enemies, &enemyCount);              // Don dep ke thù dã chet
		
    if (player.health <= 0) {
        gameState = GAME_OVER; // Chuyên sang trang thái Game Over neu nguoi choi het máu
				playInitialized = false;
				isWin = 0;
				finalScore = MAX_ENEMIES - enemyCount;
    }
		if (enemyCount == 0) {
			gameState = GAME_OVER;
			playInitialized = false;
			isWin = 1;
			finalScore = 5;
		}
		/*
		previousPlayer = player;
    memcpy(previousPlayerBullets, bullets, sizeof(Bullet) * bulletCount);
    memcpy(previousEnemies, enemies, sizeof(Enemy) * enemyCount);
    memcpy(previousEnemyBullets, enemyBullets, sizeof(Bullet) * enemyBulletCount);
*/    
}


void handleManual() {
	if(gameState != previousState  || !manualInitialized) {
			DrawManual();
			manualInitialized = true;
		}
	int button = getButtonInput();
	if (button == BUTTON_SELECT){
		gameState = GAME_MENU;
		manualInitialized = false;
	}
}

void handleGameOver() {
		if(gameState != previousState || !overInitialized) {
			DrawEndScreen(isWin, finalScore, selectedOption);
			overInitialized = true;
		}
		else{
			updateEndLCD(selectedOption);
		}
	  int button = getButtonInput(); // Ðoc tín hieu tu nút bam

    if (button == BUTTON_UP) {
        selectedOption = (selectedOption - 1 < 1) ? 2 : selectedOption - 1; // Chuyen lên
    } else if (button == BUTTON_DOWN) {
        selectedOption = (selectedOption + 1 > 2) ? 1 : selectedOption + 1; // Chuyen xuong
    } else if (button == BUTTON_SELECT) {
        switch (selectedOption) {
            case 1: // Choi lai
								Gameplay_Init();
                gameState = GAME_PLAYING;
                break;
            case 2: // Exit
								Gameplay_Init();
								gameState = GAME_MENU;
                break;
        }
    }
}

