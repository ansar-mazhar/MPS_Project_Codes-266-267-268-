#include "stm32f4xx.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "pin_system.h"
#include "gpio_config.h"
#include "servo.h"

#define GREEN_LED   (1<<2)   // PE0
#define RED_LED     (1<<3)   // PE1
#define YELLOW_LED  (1<<4)   // PE2

/* Choose a Flash sector address that is safe for data storage.
   Example: Sector 11 base address (for STM32F407, 0x080E0000).
   Make sure your linker script does not place code here. */
#define FLASH_SECTOR_ADDR 0x080E0000

static uint8_t special_pin[5] = {0, 0, 0, 0, 0};
static uint8_t saved_pin[5];  // runtime copy

/* === Flash helpers === */
static void Flash_ReadPin(uint8_t *pin)
{
    int i;
    uint32_t *base = (uint32_t *)FLASH_SECTOR_ADDR;
    uint8_t *pbyte = (uint8_t *)base;

    for(i = 0; i < 5; i++)
    {
        pin[i] = pbyte[i];
    }

    /* If flash is blank (0xFF), set default PIN = 12345 */
    if(pin[0] == 0xFF)
    {
        pin[0] = 1; pin[1] = 2; pin[2] = 3; pin[3] = 4; pin[4] = 5;
    }
}

static void Flash_WritePin(uint8_t *pin)
{
    int i;
    uint8_t *addr;

    /* Unlock Flash */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;

    /* Erase sector 11 */
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR &= ~(FLASH_CR_SNB);
    FLASH->CR |= (11 << 3);
    FLASH->CR |= FLASH_CR_STRT;
    while(FLASH->SR & FLASH_SR_BSY) { }
    FLASH->CR &= ~FLASH_CR_SER;

    /* Program bytes (note: production code should use proper word programming and alignment) */
    FLASH->CR |= FLASH_CR_PG;
    addr = (uint8_t *)FLASH_SECTOR_ADDR;
    for(i = 0; i < 5; i++)
    {
        addr[i] = pin[i];
        while(FLASH->SR & FLASH_SR_BSY) { }
    }
    FLASH->CR &= ~FLASH_CR_PG;

    /* Lock Flash */
    FLASH->CR |= FLASH_CR_LOCK;
}

/* === Compare two PINs === */
static int Compare_Pin(uint8_t *a, uint8_t *b)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        if(a[i] != b[i]) return 0;
    }
    return 1;
}

/* === Get PIN from user, show centered on row 2 === */
static void Get_Pin(uint8_t *pin)
{
    int idx = 0;
    char key;
    int start_col = 5;

    LCD_SetCursor(1, start_col);

    while(idx < 5)   // loop until user presses #
    {
        key = Keypad_GetKey();

        if(key >= '0' && key <= '9')
        {
             pin[idx] = (uint8_t)(key - '0');
             LCD_Data('*');   // masked display
             idx++;
        }
        else if(key == '*')   // Clear (backspace)
        {
            if(idx > 0)
            {
                idx--;
                LCD_SetCursor(1, start_col + idx);
                LCD_Data(' ');   // erase last digit
                LCD_SetCursor(1, start_col + idx);
            }
        }
    }
}

void Timeout_InstantLock(void)
{
    int elapsed = 0;
    char key;

    // Keep door open for up to 10 seconds
    while(elapsed < 10000)
    {
        key = Keypad_GetKey();

        if(key == '#')   // user pressed lock immediately
        {
            return;      // exit function right away
        }
        Delay_ms(100);    // wait 10 ms
        elapsed += 100;   // accumulate time
    }
	}

/* === Initialize system (LEDs + PIN from Flash) === */
void PinSystem_Init(void)
{
    /* Load PIN from Flash */
    Flash_ReadPin(saved_pin);
}

/* === Normal PIN entry (auto-lock after 10 seconds on success) === */
void PinSystem_Ask(void)
{
		int idx;
    char key;
	  uint8_t entered_pin[5];
		Servo_Init();
		Servo_DoorLock();
	
		GPIOE->ODR |= RED_LED; /* locked by default */
    GPIOE->ODR &= ~(GREEN_LED | YELLOW_LED);

    LCD_Command(0x01);
    LCD_PrintCentered(0, "Enter PIN:");
    Get_Pin(entered_pin);

    /* === Special case: 00000 enters setting mode === */
    if(Compare_Pin(entered_pin, special_pin))
    {
        /* Check if user pressed * first and # last */
        /* Simplest way: detect sequence separately */
        LCD_Command(0x01);
        LCD_PrintCentered(0, "Enter Current:");
        Get_Pin(entered_pin);

        if(Compare_Pin(entered_pin, saved_pin))
        {
            PinSystem_Setting();
            return;
        }
        else
        {
            LCD_Command(0x01);
            LCD_PrintCentered(0, "Wrong PIN!");
            Delay_ms(1000);
						return;					
        }
    }

    /* === Normal PIN check === */
    if(Compare_Pin(entered_pin, saved_pin))
    {
        GPIOE->ODR |= GREEN_LED;
        GPIOE->ODR &= ~RED_LED;

        LCD_Command(0x01);
        LCD_PrintCentered(0, "Door Unlocked");
				Servo_DoorUnlock();

				Timeout_InstantLock();

        GPIOE->ODR &= ~GREEN_LED;
        GPIOE->ODR |= RED_LED;

        LCD_Command(0x01);
        LCD_PrintCentered(0, "Door Locked");
				Servo_DoorLock();
				Delay_ms(1000);
    }
    else
    {
        GPIOE->ODR |= RED_LED;
        LCD_Command(0x01);
        LCD_PrintCentered(0, "Wrong PIN!");
        Delay_ms(1000);
				PinSystem_Ask();
    }
}
/* === Setting Mode: triggered when 00000 is entered === */
void PinSystem_Setting(void)
{
    uint8_t new_pin[5];
    uint8_t confirm_pin[5];
    int i;

    GPIOE->ODR |= YELLOW_LED;

    LCD_Command(0x01);
    LCD_PrintCentered(0, "New PIN:");
    Get_Pin(new_pin);
	
		if(Compare_Pin(new_pin, special_pin))
		{
			    LCD_Command(0x01);
					LCD_PrintCentered(0, "Kindly Try a");
					LCD_PrintCentered(1, "Different Pin");
					Delay_ms(2000);
					PinSystem_Ask();
					return;
		}

    LCD_Command(0x01);
    LCD_PrintCentered(0, "Confirm PIN:");
    Get_Pin(confirm_pin);
		
		LCD_Command(0x01);
    LCD_PrintCentered(0, "Verifying PIN...");

    if(Compare_Pin(new_pin, confirm_pin))
    {
        for(i = 0; i < 5; i++)
        {
            saved_pin[i] = new_pin[i];
        }
        Flash_WritePin(saved_pin);

        LCD_Command(0x01);
        LCD_PrintCentered(0, "PIN Changed!");
				Delay_ms(1000);
    }
    else
    {
        LCD_Command(0x01);
        LCD_PrintCentered(0, "Mismatch!");
				Delay_ms(1000);
    }

    GPIOE->ODR &= ~YELLOW_LED;
    GPIOE->ODR |= RED_LED;  /* back to locked */
}
