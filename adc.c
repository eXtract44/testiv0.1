

#include "adc.h"


extern volatile uint16_t adc_data[7];


//HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &ADC_Data,7);


//buffer for ADC convert to uint16_t
char ch[4];  //channel 1 in0
char ch2[4];  //channel 2 in1
char ch3[4];  //channel 3 in2
char ch5[4];  //channel 5 in4
char ch6[4];  //channel 6 in5
char ch7[4];  //channel 7 inter. temp

uint16_t analogRead(uint16_t adc_data)
{
char ch[4];
uint16_t data;
sprintf(ch,"%4u", adc_data);
data = ch;
return data;
}



