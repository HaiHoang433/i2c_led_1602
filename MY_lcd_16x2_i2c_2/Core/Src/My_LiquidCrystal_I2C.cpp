/*
 * My_LiquidCrystal_I2C.cpp
 *
 *  Created on: May 9, 2025
 *      Author: HoangHai
 */

#include "My_LiquidCrystal_I2C.h"
#include "main.h"
#include <string.h>

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows) {
    _Addr = lcd_Addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _backlightval = LCD_NOBACKLIGHT;
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
}

void LiquidCrystal_I2C::begin(uint8_t cols, uint8_t rows, I2C_HandleTypeDef* hi2c, uint8_t charsize) {
    _hi2c = hi2c;
    if (rows > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = rows;

    // For some 1 line displays you can select a 10 pixel high font
    if ((charsize != 0) && (rows == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // Initialize the LCD
    init();
}

void LiquidCrystal_I2C::init()
{
    _backlightval = LCD_BACKLIGHT;
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

    HAL_Delay(50);  // Wait for LCD to power up

    // Initialization sequence
    expanderWrite(_backlightval);
    HAL_Delay(1000);

    // Put LCD into 4-bit mode
    write4bits(0x03 << 4);
    HAL_Delay(5);

    write4bits(0x03 << 4);
    HAL_Delay(5);

    write4bits(0x03 << 4);
    HAL_Delay(1);

    write4bits(0x02 << 4);

    // Set # lines, font size, etc.
    command(LCD_FUNCTIONSET | 0x08);
    HAL_Delay(5);

    // Turn display on
    command(LCD_DISPLAYCONTROL | _displaycontrol);
    HAL_Delay(5);

    // Clear display
    clear();

    // Set entry mode
    command(LCD_ENTRYMODESET | 0x02);
    HAL_Delay(5);
}

void LiquidCrystal_I2C::clear()
{
	command(LCD_CLEARDISPLAY);
    HAL_Delay(2);  // This command takes longer
}

void LiquidCrystal_I2C::home()
{
	command(LCD_RETURNHOME);
	HAL_Delay(2);  // This command takes longer
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > _rows) {
        row = _rows - 1;
    }
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LiquidCrystal_I2C::noDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::display()
{
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::noBacklight()
{
	_backlightval = LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2C::backlight()
{
	_backlightval = LCD_BACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2C::printstr(const char* str)
{
	while (*str)
	{
	    send(*str++, Rs);
	}
}

/********************************
 * Private Functions            *
 ********************************/

inline void LiquidCrystal_I2C::command(uint8_t _data) {
	send(_data, 0);
}


void LiquidCrystal_I2C::send(uint8_t _data, uint8_t mode)
{
	uint8_t highnib = _data & 0xf0;
	uint8_t lownib = (_data << 4) & 0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void LiquidCrystal_I2C::write4bits(uint8_t _data)
{
	expanderWrite(_data);
	pulseEnable(_data);
}

void LiquidCrystal_I2C::expanderWrite(uint8_t _data)
{
	uint8_t buffer[1];
	buffer[0] = _data | _backlightval;
	HAL_I2C_Master_Transmit(_hi2c, _Addr << 1, buffer, 1, HAL_MAX_DELAY);
}

void LiquidCrystal_I2C::pulseEnable(uint8_t _data)
{
	expanderWrite(_data | En);	// En high
	HAL_Delay(1);

	expanderWrite(_data & ~En);	// En low
	HAL_Delay(50);
}
