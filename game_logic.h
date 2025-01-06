#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game_play.h"
#include "stdbool.h"
#include "string.h"

extern uint8_t level;


void Game_Init();
void Game_Loop();
void handleMenu();
void handleGamePlaying();
void handleGameOver();
void handleManual();
#endif
