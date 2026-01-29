#include "stm32f4xx.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "gpio_config.h"  
#include "pin_system.h"

int main(void)
{
    char key;
		GPIO_Config();     // setup GPIO for LCD + keypad
    LCD_Init();        // LCD init
    PinSystem_Init();  // LED + PIN system init

    while(1)
    {
				PinSystem_Ask();   // continuously ask for PIN
        key = Keypad_GetKey();
        if(key != 0)
        {
            LCD_Data(key);     // show key on LCD
            Delay_ms(200);     // debounce delay
        }
    }
}
