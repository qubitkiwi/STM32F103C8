/*
 * joystick.cpp
 *
 *  Created on: Aug 12, 2022
 *      Author: asong
 */

#include "joystick.h"

JOYSTICK::JOYSTICK(ADC_HandleTypeDef * ADC_handle, uint32_t ADCx, uint32_t ADCy){

	this->ADC_handle = ADC_handle;
	ADCx_CH = ADCx;
	ADCy_CH = ADCy;

	x_offset = 0;
	y_offset = 0;

	sConfig = {0};
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
}

void JOYSTICK::offset(){

	x_offset = y_offset = 0;

	int avg = 0;
	for(int i=0; i<10; i++){
		readx();
		avg += x;
	}
	x_offset = (int)avg/10;

	avg = 0;

	for(int i=0; i<10; i++){
			ready();
			avg += y;
	}
	y_offset = (int)avg/10;
}

void JOYSTICK::readx(){
	sConfig.Channel = ADCx_CH;
	HAL_ADC_ConfigChannel(ADC_handle, &sConfig);

	HAL_ADC_Start(ADC_handle);
	HAL_ADC_PollForConversion(ADC_handle, 1);
	x = HAL_ADC_GetValue(ADC_handle) - x_offset;
}

void JOYSTICK::ready(){
	sConfig.Channel = ADCy_CH;
	HAL_ADC_ConfigChannel(ADC_handle, &sConfig);

	HAL_ADC_Start(ADC_handle);
	HAL_ADC_PollForConversion(ADC_handle, 1);
	y = HAL_ADC_GetValue(ADC_handle) - y_offset;
}
