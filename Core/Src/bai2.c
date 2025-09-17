
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : UART1 Register Level Example
  ******************************************************************************
  */
 /* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"   // Thư viện CMSIS cho STM32F1

/* --- Prototype --- */
void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(char *str);
void Delay_ms(uint32_t ms);

/* --- Main program --- */
int main(void)
{
    UART1_Init();  // Khởi tạo UART1

    while (1)
    {
        UART1_SendString("Hello ten_sinh_vien via UART!\r\n");
        Delay_ms(1000); // chờ 1 giây
    }
}

/* --- Hàm khởi tạo UART1 --- */
void UART1_Init(void)
{
    /* Bật clock GPIOA và USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* PA9 (TX) cấu hình: Alternate Function Push-pull, Output 50MHz */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1); // Output 50MHz
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                       // AF Push-pull

    /* PA10 (RX) cấu hình: Input Floating */
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0; // Input floating

    /* Cấu hình Baudrate = 9600, với PCLK2 = 8MHz */
    // USARTDIV = 8MHz / (16 * 9600) ≈ 52.08
    // Mantissa = 52, Fraction ≈ 1 -> BRR = 0x0341
    USART1->BRR = 0x0341;

    /* Enable Transmitter, Receiver và USART */
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/* --- Gửi 1 ký tự qua UART1 --- */
void UART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE)); // chờ TXE=1 (Data register empty)
    USART1->DR = c;
}

/* --- Gửi chuỗi ký tự qua UART1 --- */
void UART1_SendString(char *str)
{
    while (*str)
    {
        UART1_SendChar(*str++);
    }
}

/* --- Hàm Delay ms dùng SysTick --- */
void Delay_ms(uint32_t ms)
{
    SysTick->LOAD = 8000 - 1; // 8 MHz / 8000 = 1 kHz (1ms)
    SysTick->VAL = 0;
    SysTick->CTRL = 5;        // Enable SysTick (CLKSOURCE=CPU, ENABLE=1)

    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SysTick->CTRL & (1 << 16))); // đợi COUNTFLAG=1
    }
    SysTick->CTRL = 0; // tắt SysTick sau khi dùng
}
