#include "button.h"

// Biên toàn cuc da luu trang thái nút bâm
volatile int buttonState = BUTTON_NONE;

// Câu hình GPIO cho các nút bâm
void GPIO_Button_Init(void) {
    RCC->APB2ENR |= (1 << 3); // Enable clock GPIOB
    RCC->APB2ENR |= (1 << 0); // Enable clock AFIO

    // GPIOB Input Pull-Up cho các chân PB0, PB1, PB3, PB4, PB5
    GPIOB->CRL &= ~((0xF << (4 * 0)) | (0xF << (4 * 1)) | (0xF << (4 * 3)) | (0xF << (4 * 4)) | (0xF << (4 * 5)));
    GPIOB->CRL |=  ((0x8 << (4 * 0)) | (0x8 << (4 * 1)) | (0x8 << (4 * 3)) | (0x8 << (4 * 4)) | (0x8 << (4 * 5)));
    GPIOB->ODR |= (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);

    // Câu hình EXTI cho PB0, PB1, PB3, PB4, PB5
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
volatile uint32_t msTicks = 0; // Biên luu sô mili giây dã trôi qua

// Hàm SysTick Handler (ISR)
void SysTick_Handler(void) {
    msTicks++; // Tang giá tri môi mili giây
}

// Hàm tra vê sô mili giây dã trôi qua
uint32_t millis(void) {
    return msTicks;
}

// Hàm khoi tao SysTick Timer
void SysTick_Init(void) {
    // Tân sô SysTick (1ms)
    if (SysTick_Config(SystemCoreClock / 1000)) {
        // Nêu câu hình lôi (tân sô không chia hêt), xu lý o dây
        while (1);
    }
}

// Hàm vô hieu hóa ngat
void disableInterrupt(uint8_t line) {
    EXTI->IMR &= ~(1 << line); // Tat ngt dòng `line`
}

// Hàm kích hoat lai ngat
void enableInterrupt(uint8_t line) {
    EXTI->IMR |= (1 << line); // Bat lai ngat dòng `line`
}
/*
// Hàm xu lý nút bâm debounce
void handleButtonPress(uint8_t buttonIndex) {
    uint32_t currentTime = millis();

    // Kiêm tra khoang thoi gian giua hai lan bam
    if ((currentTime - lastButtonPressTime[buttonIndex]) >= DEBOUNCE_DELAY) {
        lastButtonPressTime[buttonIndex] = currentTime; // Cap nhat thoi gian bâm cuôi
        // Thuc hiên hành dông mong muôn cho nút 
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

// Trình phuc vu ngat cho EXTI dòng 0, 1, 3, 4, 5
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

// Hàm doc trang thái nút bâm
int getButtonInput(void) {
    int currentState = buttonState; // Lây trang thái hiên tai
    buttonState = BUTTON_NONE;      // Reset trang thái dê tránh lap lai
    return currentState;            // Tra ve trang thái dã doc
}
