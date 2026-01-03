#include <stdio.h>
#include "main.h"
#include "uart.h"
#include  "w5500_driver.h"
#include  "w5500.h"

// LED on PC13 (moved from PA5 to avoid SPI conflict)
#define LED_GPIO_PORT   GPIOC
#define LED_PIN         (1U<<13)

//SPI_HandleTypeDef hspi1;
//
//static void MX_SPI1_Init(void)
//{
//    __HAL_RCC_SPI1_CLK_ENABLE();
//
//    hspi1.Instance = SPI1;
//    hspi1.Init.Mode = SPI_MODE_MASTER;
//    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//    hspi1.Init. DataSize = SPI_DATASIZE_8BIT;
//    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//    hspi1.Init.NSS = SPI_NSS_SOFT;
//    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
//    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//    hspi1.Init.CRCPolynomial = 10;
//
//    if (HAL_SPI_Init(&hspi1) != HAL_OK)
//    {
//        // SPI init failed - halt
//        while(1);
//    }
//}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hspi->Instance == SPI1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_SPI1_CLK_ENABLE();

        // PA5=SCK, PA6=MISO, PA7=MOSI
        GPIO_InitStruct. Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct. Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct. Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void int_to_string(uint32_t num, char *str);

int main(void)
{
    // HAL initialization (this includes SysTick setup)
    HAL_Init();

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Initialize SPI1 (this will call HAL_SPI_MspInit automatically)
    //MX_SPI1_Init();


    wizchip_w5500_spi_init();

    // Set PC13 as output for LED
    LED_GPIO_PORT->MODER &= ~(3U << (13 * 2));
    LED_GPIO_PORT->MODER |=  (1U << (13 * 2));

    // Initialize UART
    uart_debug_init();

    // Send startup messages
    uart_write_string("\r\n========================================\r\n");
    uart_write_string("  STM32F401 SPI1 + UART + LED Demo\r\n");
    uart_write_string("========================================\r\n");
    uart_write_string("SPI1 Enabled on PA5/PA6/PA7\r\n");
    uart_write_string("LED moved to PC13\r\n");
    uart_write_string("System Started!\r\n\r\n");

    // Send "hello" on SPI1
    uint8_t spi_data[] = "hello";
    HAL_StatusTypeDef spi_status;

    uart_write_string("Sending 'hello' on SPI1...\r\n");
//    spi_status = HAL_SPI_Transmit(&hspi1, spi_data, 5, 100);  // 5 bytes, 100ms timeout
//
//    if (spi_status == HAL_OK)
//    {
//        uart_write_string("SPI1 Transmit:  SUCCESS\r\n\r\n");
//    }
//    else
//    {
//        uart_write_string("SPI1 Transmit: FAILED\r\n\r\n");
//    }



     w5500_init();

    uint32_t counter = 0;
    uint32_t last_tick = 0;

    while (1)
    {
        // Toggle LED every 500ms using HAL_GetTick()
        if ((HAL_GetTick() - last_tick) >= 500)
        {
            last_tick = HAL_GetTick();

            // Toggle LED on PC13
            LED_GPIO_PORT->ODR ^= LED_PIN;

            // Send status message
            uart_write_string("[");
            char buffer[20];
            int_to_string(HAL_GetTick(), buffer);
            uart_write_string(buffer);
            uart_write_string(" ms] LED Toggle #");
            int_to_string(counter++, buffer);
            uart_write_string(buffer);
            uart_write_string("\r\n");

            // Send "hello" on SPI1 every 500ms
//            spi_status = HAL_SPI_Transmit(&hspi1, spi_data, 5, 100);
//            if (spi_status == HAL_OK)
//            {
//                uart_write_string("    -> SPI1 sent:  hello\r\n");
//            }
        }

        // Check if any data received
        if(uart_data_available())
        {
            uint8_t received = uart_read();
            uart_write_string("Received:  '");
            uart_write(received);
            uart_write_string("'\r\n");
        }
    }
}

void int_to_string(uint32_t num, char *str)
{
    char temp[20];
    int i = 0;
    int j = 0;

    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0)
    {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0)
    {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}
