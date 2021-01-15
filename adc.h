/*
 * adc.h
 *
 *  Created on: 28 дек. 2020 г.
 *      Author: sasho
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdio.h>


uint16_t analogRead(uint16_t adc_data);
//#define

//void adc_test(void);

#endif /* INC_ADC_H_ */
