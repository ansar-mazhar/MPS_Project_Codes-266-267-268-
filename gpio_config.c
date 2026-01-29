#include "stm32f4xx.h"

void GPIO_Config(void)
{
    // 1. Enable clocks for GPIOB, GPIOD, GPIOC
    RCC->AHB1ENR |= (1<<1); // GPIOB
    RCC->AHB1ENR |= (1<<3); // GPIOD
    RCC->AHB1ENR |= (1<<2); // GPIOC
    RCC->AHB1ENR |= (1<<4); // GPIOE
	
			    /* === Enable clock for Timer === */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
    // 2. LCD Data pins PD0–PD7 as output
    GPIOD->MODER &= ~0x0000FFFF;   // clear mode
    GPIOD->MODER |=  0x00005555;   // set output mode

    // 3. LCD Control pins PB0–PB2 as output
    GPIOB->MODER &= ~0x0000003F;
    GPIOB->MODER |=  0x00000015;

    //    // PC0–PC3 as outputs (rows)
    GPIOC->MODER &= ~0x000000FF;   // clear mode bits
    GPIOC->MODER |=  0x00000055;   // set outputs

		// PC4–PC7 input
		GPIOC->MODER &= ~(0xFF << 8);

		// Pull-down PC4–PC7
		GPIOC->PUPDR &= ~(0xFF << 8);
		GPIOC->PUPDR |=  (0xAA << 8);
		
    /* Configure PE2–PE4 as outputs */
    GPIOE->MODER &= ~(0x3F << 4);   // clear mode bits for PE2–PE4
    GPIOE->MODER |=  (0x15 << 4);   // set PE2–PE4 as outputs (01b each)

		/* PB6 ? AF2 (TIM4_CH1) */
		GPIOB->MODER &= ~(3 << (6 * 2));
		GPIOB->MODER |=  (2 << (6 * 2));   // Alternate function mode

		GPIOB->AFR[0] &= ~(0xF << (6 * 4));
		GPIOB->AFR[0] |=  (2 << (6 * 4));  // AF2 = TIM4

	}
