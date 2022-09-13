/*
 * ssd1315.h
 *
 *  Created on: Sep 3, 2022
 *      Author: asong
 */

#ifndef SSD1315_H_
#define SSD1315_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include <string.h>
#include "fonts.h"


#define  SSD1315_LCD_PIXEL_WIDTH    ((uint16_t)128)
#define  SSD1315_LCD_PIXEL_HEIGHT   ((uint16_t)64)

#define  SSD1315_LCD_COLUMN_NUMBER  ((uint16_t)128)
#define  SSD1315_LCD_PAGE_NUMBER    ((uint16_t)8)

#define  SSD1315_COLOR_WHITE  0xFF
#define  SSD1315_COLOR_BLACK  0x00

enum ScrollMode {
	SCROLL_RIGHT = 0x26,
	SCROLL_LEFT = 0x27
};

enum SCROLL_FREQ {
	SCROLL_FREQ_2FRAMES   = 0x07,
	SCROLL_FREQ_3FRAMES   = 0x04,
	SCROLL_FREQ_4FRAMES   = 0x05,
	SCROLL_FREQ_5FRAMES   = 0x00,
	SCROLL_FREQ_25FRAMES  = 0x06,
	SCROLL_FREQ_64FRAMES  = 0x01,
	SCROLL_FREQ_128FRAMES = 0x02,
	SCROLL_FREQ_256FRAMES = 0x03,
};

class SSD1315{
private:
 	I2C_HandleTypeDef* hi2c;
 	uint8_t device_address;

 	uint16_t CurrentX;
 	uint16_t CurrentY;

	uint8_t pixel_buffer[SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER];

	void ssd1315_i2c_command(uint8_t command);
	void ssd1315_i2c_Multi_Command(uint8_t *command, uint32_t size);
	void ssd1315_i2c_sand_data(uint8_t data);
	void ssd1315_i2c_Multi_sand_data(uint8_t *data, uint32_t size);

public:
	SSD1315(I2C_HandleTypeDef* hi2ca, uint8_t address);

	void init();

	void DisplayOn();
	void DisplayOff();
	void clear(uint8_t ColorCode);
	void update(void);

	void gotoXY(uint16_t x, uint16_t y);
	void WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t ColorCode);
	void ScrollingHorizontalSetup(ScrollMode Mode, uint8_t StartPage, uint8_t EndPage, SCROLL_FREQ Frequency);
	void ScrollingdownSetup(uint8_t StartPage, uint8_t EndPage, uint8_t speed);
	void ScrollingStart(void);
	void ScrollingStop(void);

	void Hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
	void Wline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
	void rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

	char Putc(char ch, FontDef_t* Font, uint16_t color);
	char Puts(char* str, FontDef_t* Font, uint16_t color);

};



#endif /* SSD1315_H_ */
