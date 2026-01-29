#include "lcd.h"
#include "delay.h"

#define RS 0   // PB0
#define RW 1   // PB1
#define EN 2   // PB2

void LCD_Command(uint8_t cmd)
{
    GPIOD->ODR = (GPIOD->ODR & ~0xFF) | cmd;   // PD0–PD7 only
    GPIOB->ODR &= ~(1<<RS);   // RS=0
    GPIOB->ODR &= ~(1<<RW);   // RW=0
    GPIOB->ODR |=  (1<<EN);   // EN=1
    Delay_ms(2);
    GPIOB->ODR &= ~(1<<EN);   // EN=0
}

void LCD_Data(uint8_t data)
{
    GPIOD->ODR = (GPIOD->ODR & ~0xFF) | data;  // PD0–PD7 only
    GPIOB->ODR |=  (1<<RS);   // RS=1
    GPIOB->ODR &= ~(1<<RW);   // RW=0
    GPIOB->ODR |=  (1<<EN);   // EN=1
    Delay_ms(2);
    GPIOB->ODR &= ~(1<<EN);   // EN=0
}

void LCD_Init(void)
{
    Delay_ms(20);       // wait for LCD power-up

    LCD_Command(0x38);  // 8-bit, 2 line
    LCD_Command(0x0C);  // display ON, cursor OFF
    LCD_Command(0x06);  // entry mode
    LCD_Command(0x01);  // clear
    Delay_ms(2);        // wait for clear
}

/* ================= Extra Helpers ================= */

// Set cursor position (row: 0 or 1, col: 0–15)
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0 ? 0x80 : 0xC0) + col;
    LCD_Command(addr);
}

// Print string
void LCD_Print(const char *s)
{
    while(*s) {
        LCD_Data(*s++);
    }
}

// Center text on 16x2 LCD (row: 0 or 1)
void LCD_PrintCentered(uint8_t row, const char *s)
{
    int len = 0, col; 
    const char *p = s;
    while(*p++) len++;

    col = (16 - len) / 2;   // center calculation
    LCD_SetCursor(row, col);
    LCD_Print(s);
}