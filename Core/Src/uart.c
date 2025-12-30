#include "uart.h"

#define UART1EN     (1U<<4)
#define GPIOAENEN   (1U<<0)
#define SR_TXE      (1U<<7)
#define SR_RXNE     (1U<<5)

// Function to calculate BRR value automatically
uint32_t calculate_brr(uint32_t pclk, uint32_t baudrate) {
    return (pclk + (baudrate / 2)) / baudrate;
}

// Function to get APB2 clock frequency
uint32_t get_apb2_clock(void) {
    uint32_t sysclk;
    uint32_t apb2_prescaler;
    uint32_t pclk2;

    // Update system clock
    SystemCoreClockUpdate();
    sysclk = SystemCoreClock;

    // Get APB2 prescaler
    apb2_prescaler = (RCC->CFGR >> 13) & 0x07;

    // Calculate APB2 clock
    if (apb2_prescaler >= 4) {
        pclk2 = sysclk >> (apb2_prescaler - 3);
    } else {
        pclk2 = sysclk;
    }

    return pclk2;
}

void uart_debug_init(void){
    // Enable GPIOA clock
    RCC->AHB1ENR |= GPIOAENEN;

    // Enable USART1 clock
    RCC->APB2ENR |= UART1EN;

    // Configure PA9 (TX) as alternate function
    GPIOA->MODER &= ~(3U<<18);
    GPIOA->MODER |= (2U<<18);

    // Configure PA10 (RX) as alternate function
    GPIOA->MODER &= ~(3U<<20);
    GPIOA->MODER |= (2U<<20);

    // Set alternate function to AF7 (USART1) for PA9
    GPIOA->AFR[1] &= ~(0xF<<4);
    GPIOA->AFR[1] |= (7U<<4);

    // Set alternate function to AF7 (USART1) for PA10
    GPIOA->AFR[1] &= ~(0xF<<8);
    GPIOA->AFR[1] |= (7U<<8);

    // AUTOMATIC BAUD RATE CALCULATION
    uint32_t apb2_clock = get_apb2_clock();
    uint32_t brr_value = calculate_brr(apb2_clock, 115200);
    USART1->BRR = brr_value;

    // Enable UART, transmitter, and receiver
    USART1->CR1 = (1U<<13) |  // UE: USART enable
                  (1U<<2)  |  // RE: Receiver enable
                  (1U<<3);    // TE: Transmitter enable
}

void uart_write(uint8_t ch){
    while(!(USART1->SR & SR_TXE)){}
    USART1->DR = (ch & 0xFF);
}

uint8_t uart_read(void){
    while(!(USART1->SR & SR_RXNE)){}
    return (uint8_t)(USART1->DR & 0xFF);
}

void uart_write_string(const char *str){
    while(*str){
        uart_write(*str++);
    }
}

uint8_t uart_read_timeout(uint8_t *data, uint32_t timeout){
    uint32_t count = 0;
    while(!(USART1->SR & SR_RXNE)){
        if(++count > timeout){
            return 0;
        }
    }
    *data = (uint8_t)(USART1->DR & 0xFF);
    return 1;
}

uint8_t uart_data_available(void){
    return (USART1->SR & SR_RXNE) ? 1 : 0;
}
