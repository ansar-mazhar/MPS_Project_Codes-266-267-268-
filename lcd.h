#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* ================= LCD FUNCTIONS ================= */
void LCD_Init(void);                  // initialize LCD
void LCD_Command(uint8_t cmd);        // send command
void LCD_Data(uint8_t data);          // send data (character)

/* Cursor and printing helpers */
void LCD_SetCursor(uint8_t row, uint8_t col);   // set cursor (row: 0–1, col: 0–15)
void LCD_Print(const char *s);                  // print string
void LCD_PrintCentered(uint8_t row, const char *s); // print centered text on row

#endif /* LCD_H */