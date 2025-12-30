#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "stm32f401xe.h"

/**
 * @brief Initialize SysTick timer for 1ms interrupts
 *
 * @param system_clock System clock frequency in Hz (e.g., 84000000 for 84MHz)
 */
void systick_init(uint32_t system_clock);

/**
 * @brief Delay for specified milliseconds (blocking)
 *
 * @param ms Delay time in milliseconds
 */
void delay_ms(uint32_t ms);

/**
 * @brief Get current system tick count
 *
 * @return Current tick count in milliseconds
 */
uint32_t get_tick(void);

/**
 * @brief Increment tick counter (called by SysTick interrupt)
 */
void systick_handler(void);

#endif // SYSTICK_H
