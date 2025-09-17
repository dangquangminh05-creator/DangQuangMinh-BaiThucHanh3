/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : UART1 LED Control Example
  ******************************************************************************
  */
 /* USER CODE END Header */

#include "stm32f10x.h"   // CMSIS cho STM32F1

/* Prototype */
void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(char *str);
char UART1_ReceiveChar(void);
void Delay_ms(uint32_t ms);

/* LED state */
uint8_t LED_State = 0; // 0 = OFF, 1 = ON

int main(void)
{
    /* Bật clock GPIOC (LED) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    /* PC13: Output Push-Pull, 2 MHz */
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1; // Output 2 MHz
    GPIOC->CRH |= 0x0;               // Push-pull

    /* Tắt LED ban đầu (PC13 = 1) */
    GPIOC->BSRR = GPIO_BSRR_BS13;
    LED_State = 0;

    /* Khởi tạo UART1 */
    UART1_Init();

    UART1_SendString("UART LED Control Ready!\r\n");
    UART1_SendString("Send '1'=ON, '0'=OFF, '?'=STATUS\r\n");

    while (1)
    {
        char c = UART1_ReceiveChar();

        if (c == '1')
        {
            GPIOC->BRR = GPIO_BRR_BR13;  // LED ON (PC13=0)
            LED_State = 1;
            UART1_SendString("LED is turned ON\r\n");
        }
        else if (c == '0')
        {
            GPIOC->BSRR = GPIO_BSRR_BS13; // LED OFF (PC13=1)
            LED_State = 0;
            UART1_SendString("LED is turned OFF\r\n");
        }
        else if (c == '?')
        {
            if (LED_State)
                UART1_SendString("Current LED state: ON\r\n");
            else
                UART1_SendString("Current LED state: OFF\r\n");
        }
        else
        {
            UART1_SendString("Invalid command! Use 1, 0, or ?\r\n");
        }
    }
}

/* --- UART Init --- */
void UART1_Init(void)
{
    /* Bật clock GPIOA và USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* PA9 (TX) cấu hình: AF Push-pull, Output 50MHz */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1);
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    /* PA10 (RX) cấu hình: Input Floating */
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    /* Baudrate = 9600, PCLK2 = 8MHz → BRR = 0x0341 */
    USART1->BRR = 0x341;

    /* Enable USART, TX, RX */
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/* --- Gửi 1 ký tự --- */
void UART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

/* --- Gửi chuỗi --- */
void UART1_SendString(char *str)
{
    while (*str)
    {
        UART1_SendChar(*str++);
    }
}

/* --- Nhận 1 ký tự (chờ đến khi có dữ liệu) --- */
char UART1_ReceiveChar(void)
{
    while (!(USART1->SR & USART_SR_RXNE));
    return USART1->DR;
}

/* --- Delay ms bằng SysTick --- */
void Delay_ms(uint32_t ms)
{
    SysTick->LOAD = 8000 - 1; // 8MHz / 8000 = 1kHz
    SysTick->VAL = 0;
    SysTick->CTRL = 5;

    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SysTick->CTRL & (1 << 16)));
    }
    SysTick->CTRL = 0;
}
