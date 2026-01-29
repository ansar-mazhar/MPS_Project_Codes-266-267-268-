#include "stm32f4xx.h"
#include "delay.h"

char Keypad_GetKey(void)
{
    /* Keypad layout mapping */
    static const char keys[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    int row, col;

    for(row = 0; row < 4; row++)
    {
        /* Drive one row low, others high (PC0–PC3) */
        GPIOC->ODR = (GPIOC->ODR & ~0x0F) | (1 << row);

        Delay_ms(2); /* small debounce delay */

        for(col = 0; col < 4; col++)
        {
            if(GPIOC->IDR & (1 << (col + 4)))   /* check column input PC4–PC7 */
            {
                while(GPIOC->IDR & (1 << (col + 4))) { } /* wait for release */
                return keys[row][col];  /* return mapped key */
            }
        }
    }

    return '\0';  /* no key pressed */
}