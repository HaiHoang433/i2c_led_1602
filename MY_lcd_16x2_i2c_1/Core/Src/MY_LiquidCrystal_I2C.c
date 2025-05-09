#include "MY_LiquidCrystal_I2C.h"
#include "main.h"
#include <string.h>

// Private function prototypes
static void LCD_Send(LiquidCrystal_I2C* lcd, uint8_t value, uint8_t mode);
static void LCD_Write4Bits(LiquidCrystal_I2C* lcd, uint8_t value);
static void LCD_ExpanderWrite(LiquidCrystal_I2C* lcd, uint8_t data);
static void LCD_PulseEnable(LiquidCrystal_I2C* lcd, uint8_t data);
static void LCD_Command(LiquidCrystal_I2C* lcd, uint8_t command);

void LCD_Init(LiquidCrystal_I2C* lcd, I2C_HandleTypeDef* hi2c, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows) {
    lcd->addr = lcd_addr;
    lcd->cols = lcd_cols;
    lcd->rows = lcd_rows;
    lcd->backlightval = LCD_BACKLIGHT;
    lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd->hi2c = hi2c;

    HAL_Delay(50);  // Wait for LCD to power up

    // Initialization sequence
    LCD_ExpanderWrite(lcd, lcd->backlightval);
    HAL_Delay(1000);

    // Put LCD into 4-bit mode
    LCD_Write4Bits(lcd, 0x03 << 4);
    HAL_Delay(5);

    LCD_Write4Bits(lcd, 0x03 << 4);
    HAL_Delay(5);

    LCD_Write4Bits(lcd, 0x03 << 4);
    HAL_Delay(1);

    LCD_Write4Bits(lcd, 0x02 << 4);

    // Set # lines, font size, etc.
    LCD_Command(lcd, LCD_FUNCTIONSET | 0x08);
    HAL_Delay(5);

    // Turn display on
    LCD_Command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
    HAL_Delay(5);

    // Clear display
    LCD_Clear(lcd);

    // Set entry mode
    LCD_Command(lcd, LCD_ENTRYMODESET | 0x02);
    HAL_Delay(5);
}

void LCD_Clear(LiquidCrystal_I2C* lcd) {
    LCD_Command(lcd, LCD_CLEARDISPLAY);
    HAL_Delay(2);  // This command takes longer
}

void LCD_Home(LiquidCrystal_I2C* lcd) {
    LCD_Command(lcd, LCD_RETURNHOME);
    HAL_Delay(2);  // This command takes longer
}

void LCD_SetCursor(LiquidCrystal_I2C* lcd, uint8_t col, uint8_t row) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > lcd->rows) {
        row = lcd->rows - 1;
    }
    LCD_Command(lcd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_NoDisplay(LiquidCrystal_I2C* lcd) {
    lcd->displaycontrol &= ~LCD_DISPLAYON;
    LCD_Command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_Display(LiquidCrystal_I2C* lcd) {
    lcd->displaycontrol |= LCD_DISPLAYON;
    LCD_Command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_NoBacklight(LiquidCrystal_I2C* lcd) {
    lcd->backlightval = LCD_NOBACKLIGHT;
    LCD_ExpanderWrite(lcd, 0);
}

void LCD_Backlight(LiquidCrystal_I2C* lcd) {
    lcd->backlightval = LCD_BACKLIGHT;
    LCD_ExpanderWrite(lcd, 0);
}

void LCD_Print(LiquidCrystal_I2C* lcd, const char* str) {
    while (*str) {
        LCD_Send(lcd, *str++, Rs);
    }
}

/********************************
 * Private Functions            *
 ********************************/

static void LCD_Command(LiquidCrystal_I2C* lcd, uint8_t command) {
    LCD_Send(lcd, command, 0);
}

static void LCD_Send(LiquidCrystal_I2C* lcd, uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    LCD_Write4Bits(lcd, highnib | mode);
    LCD_Write4Bits(lcd, lownib | mode);
}

static void LCD_Write4Bits(LiquidCrystal_I2C* lcd, uint8_t value) {
    LCD_ExpanderWrite(lcd, value);
    LCD_PulseEnable(lcd, value);
}

static void LCD_ExpanderWrite(LiquidCrystal_I2C* lcd, uint8_t data) {
    uint8_t buffer[1];
    buffer[0] = data | lcd->backlightval;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->addr << 1, buffer, 1, HAL_MAX_DELAY);
}

static void LCD_PulseEnable(LiquidCrystal_I2C* lcd, uint8_t data) {
    LCD_ExpanderWrite(lcd, data | En);  // En high
    HAL_Delay(1);  // Enable pulse must be >450ns

    LCD_ExpanderWrite(lcd, data & ~En); // En low
    HAL_Delay(1);  // Commands need >37us to settle
}
