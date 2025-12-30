//#include <stdio.h>
//#include <stm32f401xe.h>
//#include "uart.h"
//#include "systick.h"
//
//#define GPIOAEN     (1U<<0)
//#define PA5         (1U<<5)
//#define LED_PIN     PA5
//#define SYSTEM_CLOCK 84000000  // 84MHz
//
//void FPU_Enable(void);
//
//int main(void) {
//    // Enable GPIOA clock
//    RCC->AHB1ENR |= GPIOAEN;
//
//    // Set PA5 as output
//    GPIOA->MODER &= ~(3U << 10);
//    GPIOA->MODER |= (1U << 10);
//
//    // Enable FPU
//    FPU_Enable();
//
//    // Initialize SysTick (1ms tick)
//    systick_init(SYSTEM_CLOCK);
//
//    // Initialize UART
//    uart_debug_init();
//
//    // Startup message
//    uart_write_string("\r\nSTM32F401 SysTick Demo\r\n");
//    uart_write_string("Hello World!\r\n");
//    uart_write_string("AT\r\n\r\n");
//
//    while (1) {
//        // Toggle LED
//        GPIOA->ODR ^= LED_PIN;
//        uart_write_string("LED জ্বলবে Toggle\r\n");
//
//        // Accurate 500ms delay using SysTick
//        delay_ms(500);
//    }
//}
//
//void FPU_Enable(void) {
//    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
//    __DSB();
//    __ISB();
//}





#include <stdio.h>
#include "main.h"
#include "uart.h"

#define PA5         (1U<<5)
#define LED_PIN     PA5

void int_to_string(uint32_t num, char *str);

int main(void) {
    // HAL initialization (this includes SysTick setup)
    HAL_Init();

    // System Clock Configuration (if you have SystemClock_Config function)
    //SystemClock_Config();

    // Enable GPIOA clock
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Set PA5 as output for LED
    GPIOA->MODER &= ~(3U << 10);
    GPIOA->MODER |= (1U << 10);

    // Initialize UART
    uart_debug_init();

    // Send startup messages
    uart_write_string("\r\n========================================\r\n");
    uart_write_string("  STM32F401 UART + SysTick Demo\r\n");
    uart_write_string("========================================\r\n");
    uart_write_string("Hello World!\r\n");
    uart_write_string("AT\r\n");
    uart_write_string("System Started!\r\n\r\n");

    uint32_t counter = 0;
    uint32_t last_tick = 0;

    while (1) {
        // Toggle LED every 500ms using HAL_GetTick()
        if ((HAL_GetTick() - last_tick) >= 500) {
            last_tick = HAL_GetTick();

            // Toggle LED
            GPIOA->ODR ^= LED_PIN;

            // Send status message
            uart_write_string("[");
            char buffer[20];
            int_to_string(HAL_GetTick(), buffer);
            uart_write_string(buffer);
            uart_write_string(" ms] LED Toggle #");
            int_to_string(counter++, buffer);
            uart_write_string(buffer);
            uart_write_string("\r\n");
        }

        // Check if any data received
        if(uart_data_available()){
            uint8_t received = uart_read();
            uart_write_string("Received:  '");
            uart_write(received);
            uart_write_string("'\r\n");
        }
    }
}





void int_to_string(uint32_t num, char *str) {
    char temp[20];
    int i = 0;
    int j = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}
