#include "delay.h"

void Delay_ms(unsigned int ms)
{
    // Configure SysTick for 1 ms tick (assuming 16 MHz core clock)
    SysTick->LOAD = 16000 - 1; // 16,000 cycles = 1 ms
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;         // enable, use processor clock

    while(ms--)
    {
        // Wait until COUNTFLAG is set
        while((SysTick->CTRL & (1<<16)) == 0);
    }

    SysTick->CTRL = 0;         // stop SysTick
}