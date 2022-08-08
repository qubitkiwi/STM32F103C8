/*
 * sonar.cpp
 *
 *  Created on: Jul 15, 2022
 *      Author: song
 */

#include "sonar.h"


SONAR::SONAR(TIM_HandleTypeDef* tim, GPIO_TypeDef *gpio_grup, uint16_t trig, uint16_t echo){

	this->tim = tim;
	this->gpio_grup = gpio_grup;
	this->trig = trig;
	this->echo = echo;
	us_t = 0;
	state = false;

	HAL_GPIO_WritePin(gpio_grup, trig, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = trig;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(gpio_grup, &GPIO_InitStruct);

	GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = echo;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(gpio_grup, &GPIO_InitStruct);

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

void SONAR::scan(){

	HAL_GPIO_WritePin(gpio_grup, trig, GPIO_PIN_SET);
	us_delay(11);
	HAL_GPIO_WritePin(gpio_grup, trig, GPIO_PIN_RESET);

	state = true;

	while(state);
}

void SONAR::sonar_EXTI(){
	static bool rising = true;

	if(rising){ // rising
		tim->Instance->CNT = 0;

		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = echo;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(gpio_grup, &GPIO_InitStruct);

		rising = false;

	} else { // falling

		us_t = tim->Instance->CNT;

		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = echo;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(gpio_grup, &GPIO_InitStruct);

		rising = true;
		state = false;
	}
}

float SONAR::get_dist(){
	//0.072 cm/us
//	return us_t*0.072;
	return us_t*0.017;
}

void SONAR::us_delay(uint32_t us){
	tim->Instance->CNT = 0;
	while(tim->Instance->CNT < us);
}

