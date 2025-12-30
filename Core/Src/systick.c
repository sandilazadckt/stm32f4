#include "systick.h"

// Global tick counter
static volatile uint32_t tick_count = 0;

void systick_init(uint32_t system_clock) {
    // Calculate reload value for 1ms interrupts
    // SysTick counts down from reload value to 0
    // For 1ms:  reload = (clock_frequency / 1000) - 1
    uint32_t reload_value = (system_clock / 1000) - 1;

    // Disable SysTick during configuration
    SysTick->CTRL = 0;

    // Set reload value
    SysTick->LOAD = reload_value;

    // Clear current value
    SysTick->VAL = 0;

    // Enable SysTick, use processor clock, enable interrupt
    SysTick->CTRL = (1U << 0) |  // ENABLE:  Enable counter
                    (1U << 1) |  // TICKINT: Enable interrupt
                    (1U << 2);   // CLKSOURCE: Use processor clock
}

void delay_ms(uint32_t ms) {
    uint32_t start_tick = tick_count;

    // Wait until the desired time has elapsed
    while ((tick_count - start_tick) < ms) {
        __NOP();
    }
}

uint32_t get_tick(void) {
    return tick_count;
}

void systick_handler(void) {
    tick_count++;
}

