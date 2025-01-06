#include "button.h"

// Bi�n to�n cuc da luu trang th�i n�t b�m
volatile int buttonState = BUTTON_NONE;

// C�u h�nh GPIO cho c�c n�t b�m
void GPIO_Button_Init(void) {
    RCC->APB2ENR |= (1 << 3); // Enable clock GPIOB
    RCC->APB2ENR |= (1 << 0); // Enable clock AFIO

    // GPIOB Input Pull-Up cho c�c ch�n PB0, PB1, PB3, PB4, PB5
    GPIOB->CRL &= ~((0xF << (4 * 0)) | (0xF << (4 * 1)) | (0xF << (4 * 3)) | (0xF << (4 * 4)) | (0xF << (4 * 5)));
    GPIOB->CRL |=  ((0x8 << (4 * 0)) | (0x8 << (4 * 1)) | (0x8 << (4 * 3)) | (0x8 << (4 * 4)) | (0x8 << (4 * 5)));
    GPIOB->ODR |= (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);

    // C�u h�nh EXTI cho PB0, PB1, PB3, PB4, PB5
    AFIO->EXTICR[0] |= (0x1 << 0) | (0x1 << 4) | (0x1 << 12); // EXTI0, EXTI1, EXTI3
    AFIO->EXTICR[1] |= (0x1 << 0) | (0x1 << 4);              // EXTI4, EXTI5

    EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);
    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);

    // Enable NVIC Interrupt
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

volatile uint32_t lastButtonPressTime[5] = {0};
volatile uint32_t msTicks = 0; // Bi�n luu s� mili gi�y d� tr�i qua

// H�m SysTick Handler (ISR)
void SysTick_Handler(void) {
    msTicks++; // Tang gi� tri m�i mili gi�y
}

// H�m tra v� s� mili gi�y d� tr�i qua
uint32_t millis(void) {
    return msTicks;
}

// H�m khoi tao SysTick Timer
void SysTick_Init(void) {
    // T�n s� SysTick (1ms)
    if (SysTick_Config(SystemCoreClock / 1000)) {
        // N�u c�u h�nh l�i (t�n s� kh�ng chia h�t), xu l� o d�y
        while (1);
    }
}

// H�m v� hieu h�a ngat
void disableInterrupt(uint8_t line) {
    EXTI->IMR &= ~(1 << line); // Tat ngt d�ng `line`
}

// H�m k�ch hoat lai ngat
void enableInterrupt(uint8_t line) {
    EXTI->IMR |= (1 << line); // Bat lai ngat d�ng `line`
}
/*
// H�m xu l� n�t b�m debounce
void handleButtonPress(uint8_t buttonIndex) {
    uint32_t currentTime = millis();

    // Ki�m tra khoang thoi gian giua hai lan bam
    if ((currentTime - lastButtonPressTime[buttonIndex]) >= DEBOUNCE_DELAY) {
        lastButtonPressTime[buttonIndex] = currentTime; // Cap nhat thoi gian b�m cu�i
        // Thuc hi�n h�nh d�ng mong mu�n cho n�t 
				switch (buttonIndex) {
					case 0:
						buttonState = BUTTON_0;
						break;
					case 1:
						buttonState = BUTTON_1;
						break;
					case 3:
						buttonState = BUTTON_3;
						break;
					case 4:
						buttonState = BUTTON_4;
						break;
					case 5:
						buttonState = BUTTON_5;
						break;
				}
			}
}*/

// Tr�nh phuc vu ngat cho EXTI d�ng 0, 1, 3, 4, 5
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {
				buttonState = BUTTON_0;
        EXTI->PR |= (1 << 0);
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        buttonState = BUTTON_1;
        EXTI->PR |= (1 << 1);
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1 << 3)) {
        buttonState = BUTTON_3;
        EXTI->PR |= (1 << 3);
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1 << 4)) {
        buttonState = BUTTON_4;
        EXTI->PR |= (1 << 4);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << 5)) {
        buttonState = BUTTON_5;
        EXTI->PR |= (1 << 5);
    }
}

// H�m doc trang th�i n�t b�m
int getButtonInput(void) {
    int currentState = buttonState; // L�y trang th�i hi�n tai
    buttonState = BUTTON_NONE;      // Reset trang th�i d� tr�nh lap lai
    return currentState;            // Tra ve trang th�i d� doc
}
