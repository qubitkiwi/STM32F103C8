/*
 * ssd1315.cpp
 *
 *  Created on: Sep 3, 2022
 *      Author: asong
 */

#include "ssd1315.h"


SSD1315::SSD1315(I2C_HandleTypeDef* hi2c, uint8_t address) {
	this->hi2c = hi2c;
	device_address = address;
}


void SSD1315::ssd1315_i2c_command(uint8_t command) {
	uint8_t data[2] = { 0x80, command};
	HAL_I2C_Master_Transmit(hi2c, device_address, data, 2, HAL_MAX_DELAY);
}

void SSD1315::ssd1315_i2c_Multi_Command(uint8_t *command, uint32_t size) {
	uint8_t data[size+1];
	data[0] = 0x00;

	for (uint32_t idx=0; idx<size; idx++) {
		data[idx+1] = command[idx];
	}

	HAL_I2C_Master_Transmit(hi2c, device_address, data, size+1, HAL_MAX_DELAY);
}

void SSD1315::ssd1315_i2c_sand_data(uint8_t data) {
	uint8_t sand_data[2] = { 0xC0, data};
	HAL_I2C_Master_Transmit(hi2c, device_address, sand_data, 2, HAL_MAX_DELAY);
}

void SSD1315::ssd1315_i2c_Multi_sand_data(uint8_t *data, uint32_t size) {
	uint8_t sand_data[size+1];
	sand_data[0] = 0x40;

	for (uint32_t idx=0; idx<size; idx++) {
		sand_data[idx+1] = data[idx];
	}

	HAL_I2C_Master_Transmit(hi2c, device_address, sand_data, size+1, HAL_MAX_DELAY);
}




void SSD1315::init() {

	uint8_t Cmd[] = {0x8D, 0x14, 0x20, 0x00, 0x40, 0xC8, 0xA1, 0xAF};
	ssd1315_i2c_Multi_Command(Cmd, sizeof(Cmd));
	clear(SSD1315_COLOR_BLACK);
	ScrollingStop();
	update();
}

void SSD1315::DisplayOn() {
	uint8_t Cmd[] = {0x8D, 0x14, 0xAF};
	ssd1315_i2c_Multi_Command(Cmd, 3);
}

void SSD1315::DisplayOff() {
	uint8_t Cmd[] = {0x8D, 0x10, 0xAE};
	ssd1315_i2c_Multi_Command(Cmd, 3);
}

void SSD1315::clear(uint8_t ColorCode)
{
  /* Check color */
  if (ColorCode == SSD1315_COLOR_WHITE) {
    memset(pixel_buffer, 0xFF, SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER);
  } else {
    memset(pixel_buffer, 0x00, SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER);
  }
}

void SSD1315::update(void) {

	uint8_t Cmd[] = {0x21, 0x00, 0x7F, /* Set Column Address Setup column start(0) and end address (127)*/
					0x22, 0x00, 0x07}; /* Set Page Address Setup page start (0)  and end address (7)*/
	ssd1315_i2c_Multi_Command(Cmd, sizeof(Cmd));

	/* Fill Buffer in GDDRAM of LCD */
	ssd1315_i2c_Multi_sand_data((uint8_t*)pixel_buffer, SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER);
}

void SSD1315::gotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	CurrentX = x;
	CurrentY = y;
}

void SSD1315::WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t ColorCode) {
  /* Set color */
  if (ColorCode == SSD1315_COLOR_WHITE) {
	  pixel_buffer[Xpos + (Ypos / 8) * SSD1315_LCD_PIXEL_WIDTH] |= 1 << (Ypos % 8);
  } else {
	  pixel_buffer[Xpos + (Ypos / 8) * SSD1315_LCD_PIXEL_WIDTH] &= ~(1 << (Ypos % 8));
  }
}

void SSD1315::Hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color) {
	uint16_t end = y+len < SSD1315_LCD_PIXEL_HEIGHT ? y+len : SSD1315_LCD_PIXEL_HEIGHT-1;
	for (int i=y; i<end; i++) {
		WritePixel(x, i, color);
	}
}

void SSD1315::Wline(uint16_t x, uint16_t y, uint16_t len, uint16_t color) {
	uint16_t end = x+len < SSD1315_LCD_PIXEL_WIDTH ? x+len : SSD1315_LCD_PIXEL_WIDTH-1;
	for (int i=x; i<end; i++) {
		WritePixel(i, y, color);
	}
}

void SSD1315::rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	Hline(x1, y1, y2-y1, color);
	Hline(x2, y1, y2-y1, color);
	Wline(x1, y1, x2-x1, color);
	Wline(x1, y2, x2-x1, color);
}

void SSD1315::ScrollingHorizontalSetup(ScrollMode Mode, uint8_t StartPage, uint8_t EndPage, SCROLL_FREQ Frequency) {

	uint8_t Cmd[] = {
			(uint8_t) Mode,
			0x00,
			StartPage,
			(uint8_t) Frequency,
			EndPage,
			0x00,
			0xFF
	};
	ssd1315_i2c_Multi_Command(Cmd, sizeof(Cmd));
}

void SSD1315::ScrollingdownSetup(uint8_t StartPage, uint8_t EndPage, uint8_t speed) {

	uint8_t Cmd[] = {
			0x29,
			0x00,
			StartPage,
			(uint8_t) 0b111,
			EndPage,
			(0x40-speed),
			0x00,
			0x7F
	};
	ssd1315_i2c_Multi_Command(Cmd, sizeof(Cmd));
}

void SSD1315::ScrollingStart(void) {
	ssd1315_i2c_command(0x2F);
}

void SSD1315::ScrollingStop(void) {
	ssd1315_i2c_command(0x2E);
}


char SSD1315::Putc(char ch, FontDef_t* Font, uint16_t color) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
			SSD1315_LCD_PIXEL_WIDTH <= (CurrentX + Font->FontWidth) ||
			SSD1315_LCD_PIXEL_HEIGHT <= (CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				WritePixel(CurrentX + j, (CurrentY + i), (uint16_t) color);
			} else {
				WritePixel(CurrentX + j, (CurrentY + i), (uint16_t)!color);
			}
		}
	}

	/* Increase pointer */
	CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
}

char SSD1315::Puts(char* str, FontDef_t* Font, uint16_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}

