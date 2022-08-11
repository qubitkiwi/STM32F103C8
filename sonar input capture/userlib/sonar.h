/*
 * sonar.h
 *
 *  Created on: Jul 15, 2022
 *      Author: song
 */

#ifndef SONAR_H_
#define SONAR_H_

#include "stm32f1xx_hal.h"

class SONAR {
private:
	TIM_HandleTypeDef* tim;
	GPIO_TypeDef *gpio_grup;
	uint16_t trig;
	uint16_t echo;
	unsigned int us_t;
	bool state;

public:
	void us_delay(uint32_t us);

	SONAR(TIM_HandleTypeDef* tim, GPIO_TypeDef *gpio_grup, uint16_t trig, uint16_t echo);
	void sonar_EXTI();
	void scan();
	float get_dist();

};

class SONAR_IC{
private:
	TIM_HandleTypeDef* tim;
	GPIO_TypeDef *gpio_grup;
	uint16_t trig;
	uint32_t CHN;
	unsigned int t1, t2;
	float dist;
	bool edge;

public:

	void us_delay(uint32_t us);

	SONAR_IC(TIM_HandleTypeDef* tim, GPIO_TypeDef *gpio_grup, uint16_t trig, uint32_t CHN);
	void IC();
	void scan();
	float get_dist();
};

#endif /* SONAR_H_ */
