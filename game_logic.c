#include "game_logic.h"

uint8_t isWin = 0;
uint16_t finalScore = 0;
uint8_t level = 1; // Gi� tr? m?c d?nh

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

// Trang th�i cua tr� choi
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
		GAME_MANUAL,
    GAME_OVER
} GameState;

// Bi�n trang th�i game
GameState gameState;
GameState previousState; // Trang th�i truoc d�
int selectedOption = 1; // Lua chon ban dau trong menu

void Game_Init() {
    gameState = GAME_MENU; // Bat d�u voi m�n h�nh menu
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
    int button = getButtonInput(); // �oc t�n hieu tu n�t bam

    if (button == BUTTON_UP) {
        selectedOption = (selectedOption - 1 < 1) ? 3 : selectedOption - 1; // Chuyen l�n
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
                while (1); // Ket th�c chuong tr�nh
        }
    }
		
}

void handleGamePlaying() {
		if(gameState != previousState || !playInitialized) {
			// X�a to�n bo m�n h�nh voi nen den
			ST7735_FillRect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, COLOR_BLACK);
			playInitialized = true;
		}
		else{
			DrawGameFullScreen(level, &player, bullets, bulletCount, enemies, enemyCount, enemyBullets, enemyBulletCount); // Ve m�n h�nh choi game day du
		}
		//DrawGameFullScreen(level, &player, bullets, bulletCount, enemies, enemyCount, enemyBullets, enemyBulletCount); // Ve m�n h�nh choi game day du
	
    handlePlayerInput(&player, bullets, &bulletCount); // Xu l� n�t bam cua nguoi choi

    //updatePlayerPosition(&player); // C�p nhat vi tr� nguoi choi
    updateEnemyPositions(enemies, enemyCount); // C�p nh�t vi tr� ke th�
    updatePlayerBullets(bullets, &bulletCount); // C�p nh�t vi tr� dan cua nguoi choi
    updateEnemyBullets(enemyBullets, &enemyBulletCount); // Cap nhat vi tr� dan cua ke th�

    enemyShootRandomly(enemies, enemyCount, enemyBullets, &enemyBulletCount); // Ke th� ban dan

    checkPlayerBulletCollisions(bullets, &bulletCount, enemies, &enemyCount); // �an nguoi choi ban tr�ng ke th�
    checkEnemyBulletCollisions(enemyBullets, &enemyBulletCount, &player);    // �an ke th� ban tr�ng nguoi choi
    checkPlayerEnemyCollisions(&player, enemies, &enemyCount);               // Va cham giua nguoi choi v� ke th�

    cleanUpBullets(bullets, &bulletCount);             // Don dep dan nguoi choi ra khoi m�n h�nh
    cleanUpBullets(enemyBullets, &enemyBulletCount);   // Don dap dan ke th� ra khoi m�n h�nh
    cleanUpEnemies(enemies, &enemyCount);              // Don dep ke th� d� chet
		
    if (player.health <= 0) {
        gameState = GAME_OVER; // Chuy�n sang trang th�i Game Over neu nguoi choi het m�u
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
	  int button = getButtonInput(); // �oc t�n hieu tu n�t bam

    if (button == BUTTON_UP) {
        selectedOption = (selectedOption - 1 < 1) ? 2 : selectedOption - 1; // Chuyen l�n
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

