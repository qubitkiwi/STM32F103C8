#include "LiquidCrystal.h"

#include <stdio.h>
#include <string.h>
#include "./DWT_Delay.h"
//#include <inttypes.h>
//#include "Arduino.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal::LiquidCrystal(GPIO_TypeDef *GPIOx, uint32_t rs, uint32_t rw, uint32_t enable,
			     uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3,
			     uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7)
{
	this->GPIOx = GPIOx;
  init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(GPIO_TypeDef *GPIOx, uint32_t rs, uint32_t enable,
			     uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3,
			     uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7)
{
	this->GPIOx = GPIOx;
  init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(GPIO_TypeDef *GPIOx, uint32_t rs, uint32_t rw, uint32_t enable,
			     uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
	this->GPIOx = GPIOx;
  init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

LiquidCrystal::LiquidCrystal(GPIO_TypeDef *GPIOx, uint32_t rs,  uint32_t enable,
			     uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
	this->GPIOx = GPIOx;
  init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void LiquidCrystal::init(uint32_t fourbitmode, uint32_t rs, uint32_t rw, uint32_t enable,
			 uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3,
			 uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7)
{
	DWT_Delay_Init();

  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 

  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  begin(16, 2);
}

void LiquidCrystal::pinOutputMode(uint32_t pin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	// IO Pin Configurations
	if(GPIOx == GPIOA)
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	else if(GPIOx == GPIOB)
	    __HAL_RCC_GPIOB_CLK_ENABLE();
	else if(GPIOx == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if(GPIOx == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOx, pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;

  setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	// IO Pin Configurations
	if(GPIOx == GPIOA)
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	else if(GPIOx == GPIOB)
	    __HAL_RCC_GPIOB_CLK_ENABLE();
	else if(GPIOx == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if(GPIOx == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin |= _rs_pin;
	//  pinOutputMode(_rs_pin);

  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) { 
//	  pinOutputMode(_rw_pin);
	  GPIO_InitStruct.Pin |= _rw_pin;
  }
//  pinOutputMode(_enable_pin);
  GPIO_InitStruct.Pin |= _enable_pin;
  
  // Do these once, instead of every time a character is drawn for speed reasons.
  for (int i=0; i<((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
  {
//	  pinOutputMode(_data_pins[i]);
	  GPIO_InitStruct.Pin |= _data_pins[i];
   } 

	HAL_GPIO_WritePin(GPIOx, GPIO_InitStruct.Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  DWT_Delay_us(50000);

  // Now we pull both RS and R/W low to begin commands
  HAL_GPIO_WritePin(GPIOx, _rs_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOx, _enable_pin, GPIO_PIN_RESET);
  if (_rw_pin != 255) { 
    HAL_GPIO_WritePin(GPIOx, _rw_pin, GPIO_PIN_RESET);
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
//    delayMicroseconds(4500); // wait min 4.1ms
    DWT_Delay_us(4500);

    // second try
    write4bits(0x03);
//    delayMicroseconds(4500); // wait min 4.1ms
    DWT_Delay_us(4500);
    
    // third go!
    write4bits(0x03); 
//    delayMicroseconds(150);
    DWT_Delay_us(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
//    delayMicroseconds(4500);  // wait more than 4.1ms
    DWT_Delay_us(4500);

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
//    delayMicroseconds(150);
    DWT_Delay_us(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

void LiquidCrystal::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
  DWT_Delay_us(2000);
}

void LiquidCrystal::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
  DWT_Delay_us(2000);
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
  const uint8_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= _numlines ) {
    row = _numlines - 1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal::command(uint8_t value) {
  send(value, GPIO_PIN_RESET);
}

inline size_t LiquidCrystal::write(uint8_t value) {
  send(value, GPIO_PIN_SET);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystal::send(uint8_t value, GPIO_PinState PinState) {
//  digitalWrite(_rs_pin, mode);
  HAL_GPIO_WritePin(GPIOx, _rs_pin, PinState);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
	  HAL_GPIO_WritePin(GPIOx, _rs_pin, GPIO_PIN_RESET);
  }
  
  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value); 
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
//  DWT_Delay_us(37);
}

void LiquidCrystal::pulseEnable(void) {
	HAL_GPIO_WritePin(GPIOx, _enable_pin, GPIO_PIN_RESET);
//  delayMicroseconds(1);
  DWT_Delay_us(1);
  HAL_GPIO_WritePin(GPIOx, _enable_pin, GPIO_PIN_SET);
//  delayMicroseconds(1);    // enable pulse must be >450ns
  DWT_Delay_us(1);
  HAL_GPIO_WritePin(GPIOx, _enable_pin, GPIO_PIN_RESET);
//  delayMicroseconds(100);   // commands need > 37us to settle
  DWT_Delay_us(100);
}

void LiquidCrystal::write4bits(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    HAL_GPIO_WritePin(GPIOx, _data_pins[i], (value >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }

  pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    HAL_GPIO_WritePin(GPIOx, _data_pins[i], (value >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  
  pulseEnable();
}
