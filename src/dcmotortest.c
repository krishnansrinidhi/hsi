//header files
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f3_discovery.h"
#include "stm32f303xc.h"
#include "common.h"



void TIM_DELAY(uint16_t value)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;	  // Enable Clock APB1(36Mhz), value is 0x10
  TIM6->ARR = value;      		  // delay in ms
  TIM6->PSC = 36000-1;  		  // APB1 = 36Mhz so eqn :  CLK_CNT = (36000000)/36000,i.e. CLK_CNT = 1Khz
  TIM6->CCR1 = value/4;			  // duty cycle: 25%
  TIM6->CR1 = 0x01;         		  // Enable TIMER 
  while(!((TIM6->SR & 0x01) == 0x01));    // Check for Update Event
  TIM6->CR1 &= ~(0x01);                   // Disable timer
  TIM6->SR &= ~(0x01);                    // Reset the update event
}



void configure_DCmotor(void)
{

//initialize the LEDs
BSP_LED_Init(3);

//enable GPIOC clock
RCC->AHBENR 	|= RCC_AHBENR_GPIOCEN;  
RCC->AHBENR 	|= RCC_AHBENR_GPIOBEN; 
//Configure PC0 - PC3
GPIOC->MODER 	|= 0X00000055;          //GPIO Mode: output, IN1 of L293D 
GPIOC->OTYPER 	|= 0X0000000F;          //Output Type: Open Drain
GPIOC->OSPEEDR 	|= 0X000000FF;          //GPIO Speed: 50MHz    
GPIOC->PUPDR 	|= 0X00000055;          //PULL UP	

//Configure PB14 & PB15 pin enable input
GPIOB->MODER   &= ~(0XF0000000);       //GPIO Mode:output, IN2 of L293D 
GPIOB->OTYPER  &= ~(0X0000C000);          
GPIOB->OSPEEDR |= 0XF0000000;          //GPIO Speed: 50MHz    
GPIOB->PUPDR   |= 0X50000000;          //PULL UP 
GPIOB->ODR     |= 0X0000C000;
}




void CmdDCmotor_anti(int mode){
	
	configure_DCmotor();

	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}
        while(1){
        GPIOC->ODR	|= 0X00000001;	//anti-clockwise
	GPIOC->ODR	|= 0X00000004;
        BSP_LED_On(3);
        TIM_DELAY(1000);
	GPIOC->ODR	|= 0X00000000;
	BSP_LED_Off(3);	
        TIM_DELAY(1000);
        }
}
ADD_CMD("dcmotortesta", CmdDCmotor_anti, "		Run the DC Motor anti-clockwise");




void CmdDCmotor_C(int mode){
	
  	configure_DCmotor();
	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}
        while(1){
        GPIOC->ODR	|= 0X00000002;	
	GPIOC->ODR	|= 0X00000008;
        TIM_DELAY(1000);
	BSP_LED_On(6);
	TIM_DELAY(1000);
	BSP_LED_Off(6);
        }
}
ADD_CMD("dcmotortestc", CmdDCmotor_C, "		Run the DC Motor clockwise");
