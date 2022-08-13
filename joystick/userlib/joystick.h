/*
 * joystick.h
 *
 *  Created on: Aug 12, 2022
 *      Author: asong
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "main.h"

class JOYSTICK{
private:
//	GPIO_TypeDef * xGPIO;
//	GPIO_TypeDef * yGPIO;
//	uint16_t       xPIN;
//	uint16_t       yPIN;
//	ADC_TypeDef*   ADC_Instance;

	ADC_ChannelConfTypeDef sConfig;
	ADC_HandleTypeDef * ADC_handle;
	uint32_t       ADCx_CH;
	uint32_t       ADCy_CH;
	int x_offset, y_offset;

public:
	int x, y;
	JOYSTICK(ADC_HandleTypeDef * ADC_handle, uint32_t ADCx, uint32_t ADCy);
	void offset();
	void readx();
	void ready();
};



#endif /* JOYSTICK_H_ */
