//header files
#include <stdio.h>
#include <stdint.h>
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"



void ledBlink(void)
{
 
 //PORTA - PA0,PA1,PA2,PA3,PA4
 RCC->AHBENR 	|= RCC_AHBENR_GPIOAEN;
 GPIOA->MODER 	|= 0X00000155;
 GPIOA->OTYPER 	&= ~(0X0000001F);
 GPIOA->OSPEEDR |= 0X000003FF;
 GPIOA->PUPDR   &= ~(0x000003FF);

 //PORTF - PF4
 RCC->AHBENR 	|= RCC_AHBENR_GPIOFEN;
 GPIOF->MODER 	|= 0X00000100;
 GPIOF->OTYPER 	&= ~(0X00000010);
 GPIOF->OSPEEDR |= 0X00000300;
 GPIOF->PUPDR   &= ~(0x00000300);

 while(1)
 {
    GPIOF->BSRRL |= 0X00000010;
    GPIOA->BSRRL |= 0X0000001F;
 }

}



void CmdBlinkLED(int mode)
{

	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}

        ledBlink();
        
}
ADD_CMD("onled", CmdBlinkLED, "		Glow external LED");

