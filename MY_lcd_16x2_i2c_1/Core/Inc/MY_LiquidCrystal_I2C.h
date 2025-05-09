#ifndef MY_LIQUIDCRYSTAL_I2C_H
#define MY_LIQUIDCRYSTAL_I2C_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

// Define LCD commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags cho display control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Flags cho backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

typedef struct {
    uint8_t addr;
    uint8_t cols;
    uint8_t rows;
    uint8_t backlightval;
    uint8_t displaycontrol;
    I2C_HandleTypeDef* hi2c;  // Add I2C handler
} LiquidCrystal_I2C;

// Function prototypes
void LCD_Init(LiquidCrystal_I2C* lcd, I2C_HandleTypeDef* hi2c, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows);
void LCD_Clear(LiquidCrystal_I2C* lcd);
void LCD_Home(LiquidCrystal_I2C* lcd);
void LCD_SetCursor(LiquidCrystal_I2C* lcd, uint8_t col, uint8_t row);
void LCD_NoDisplay(LiquidCrystal_I2C* lcd);
void LCD_Display(LiquidCrystal_I2C* lcd);
void LCD_NoBacklight(LiquidCrystal_I2C* lcd);
void LCD_Backlight(LiquidCrystal_I2C* lcd);
void LCD_Print(LiquidCrystal_I2C* lcd, const char* str);

#endif
