#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h" // Header d�nh cho STM32F103C8

// Khai b�o tr?ng th�i n�t b?m
#define BUTTON_NONE  0
#define BUTTON_0     1
#define BUTTON_1     2
#define BUTTON_3     3
#define BUTTON_4     4
#define BUTTON_5     5
#define BUTTON_LEFT     	1
#define BUTTON_RIGHT     	2
#define BUTTON_UP     		3
#define BUTTON_DOWN     4
#define BUTTON_SELECT     	5

#define DEBOUNCE_DELAY 10 // Thoi gian debounce (ms)

void SysTick_Handler(void);
extern uint32_t millis(void);
void SysTick_Init(void);

void handleButtonPress(uint8_t buttonIndex, uint8_t extiLine);
void GPIO_Button_Init(void);   // H�m c?u h�nh GPIO v� ng?t
int getButtonInput(void);      // H�m d?c tr?ng th�i n�t b?m

#endif
