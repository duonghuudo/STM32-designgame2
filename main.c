#include "game_logic.h"


void SystemClock_Config(void) {
    // B�t HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Cho HSE san s�ng

    // C�u h�nh PLL: HSE l�m ngu�n d�u v�o, nh�n 9 (16 MHz * 9 = 72 MHz)
    RCC->CFGR |= RCC_CFGR_PLLSRC;        // HSE l�m ngu�n PLL
    RCC->CFGR |= RCC_CFGR_PLLMULL9;      // Nh�n 9

    // B�t PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Cho PLL san s�ng

    // Ch?n PLL l�m SYSCLK
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Cho PLL duoc chon l�m SYSCLK

    // C�u h�nh prescaler cho AHB, APB1, APB2
    RCC->CFGR &= ~RCC_CFGR_HPRE;         // AHB prescaler = 1 (SYSCLK = HCLK)
    RCC->CFGR &= ~RCC_CFGR_PPRE1;        // APB1 prescaler = 2 (HCLK / 2 = 36 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR &= ~RCC_CFGR_PPRE2;        // APB2 prescaler = 1 (HCLK / 1 = 72 MHz)
}
/*
// Bi�n to�n cuc d? d?m ms
volatile uint32_t tick_ms = 0;

// H�m kh?i t?o SysTick
void SysTick_Init(void) {
    // SysTick ch?y ? HCLK (80 MHz) chia 8 -> 10 MHz -> m?i tick = 0.1 �s
    SysTick_Config(SystemCoreClock / 1000); // C?u h�nh 1 ms m?i ng?t
}


void SysTick_Handler(void) {
    tick_ms++; // Tang bi?n d?m ms
}

uint32_t GetTick(void) {
    return tick_ms;
}

// H�m delay (don vi ms)
void Delay_ms(uint32_t ms) {
    uint32_t start = GetTick();
    while ((GetTick() - start) < ms) {
        // Cho
    }
}

// H�m delay 5 gi�y
void Delay_5s(void) {
    Delay_ms(5000); // G?i h�m delay 5000 ms
}*/
int main(void) {
    // Khoi tao phan cung
    SystemInit();
		SystemClock_Config();
    GPIO_Button_Init();
		GPIO_Config();
		SPI_Config();
		ST7735_Init();
		Game_Init();
		Gameplay_Init();


    while (1) {
        Game_Loop();  // V�ng lap ch�nh cua game
    }
}