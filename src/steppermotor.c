/*
   FILE          : steppermotor.c
   PROJECT       : CNTR8000-16S - Stepper Motor Lab
   PROGRAMMER    : Krishnan-Bengaluru,Srinidhi  Student
   FIRST VERSION : June 30, 2016
   DESCRIPTION   : This program configures GIPOC(PC0,PC1,PC2,PC3) to send
		   binary data to control stepper motor in clockwise and 
                   anti-clockwise directions with two modes each:Half-step 
                   and Full-step modes respectively.

		   Timer 6 is configured to generate the required delay
                   and also an Interrupt flag(UIF) is monitored to check
		   the update event.
*/



//header files
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f3_discovery.h"
#include "stm32f303xc.h"
#include "common.h"



//globals variables 
//const unsigned char halfstep[] = {0x05,0x01,0x09,0x08,0x0A,0x02,0x04,0x06}; //PC0-PC3
//const unsigned char fullstep[] = {0x0A,0x06,0x05,0x09}; PC0-PC3


/* Stepper motor array values for half step */
//const uint16_t halfstep[] = {0x0140,0X0040,0X0240,0X0200,0X0280,0X0080,0X0100,0X0180}; //PC6-PC9
const uint16_t halfstep[] = {0x1400,0X0400,0X2400,0X2000,0X2800,0X0800,0X1000,0X1800}; //PC10-PC13


/* Stepper motor array values for full step */
//const uint16_t fullstep[] = {0x0280,0X0180,0X0140,0X0240}; //PC6-PC9
const uint16_t fullstep[] = {0x2800,0X1800,0X1400,0X2400}; //PC10-PC13



//global constant
#define DELAY 50



// FUNCTION      : TIMER_DELAY()
// DESCRIPTION   : The function creates delay in ms
// PARAMETERS    : The function accepts a uint16_t value for delay
// RETURNS       : returns nothing
void TIMER_DELAY(uint16_t value)
{
        
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;	  // Enable Clock APB1(36Mhz), value is 0x10
  TIM6->ARR = value;       		  // delay in ms
  TIM6->PSC = 36000-1;  		  // APB1 = 36Mhz so eqn :  CLK_CNT = (36000000)/36000,i.e. CLK_CNT = 1Khz
  TIM6->CR1 = 0x01;         		  // Enable TIMER 
  while(!((TIM6->SR & 0x01) == 0x01));    // Check for Update Event
  TIM6->CR1 &= ~(0x01);                   // Disable timer
  TIM6->SR &= ~(0x01);                    // Reset the update event

}



// FUNCTION      : runStepperCLK()
// DESCRIPTION   : The function activates the stepper motor to operate
//                 in clockwise direction and also in 2 different modes:
//		   Half-step and Full-Step
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void runStepperCLK( int mode )
{

  int rc;		// status flag to check if user inputs a value in terminal
  uint32_t val,STEPS;   // val stores the mode of stepper: Half-step or Full-step
  
  //initialize the LEDs
  BSP_LED_Init(3);
  BSP_LED_Init(4);

  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }

  //read the mode entered by user on terminal: half step or full step
  rc = fetch_uint32_arg(&val);

  //error message if user doesn't enter mode
  if(rc) 
   {
    printf("Please enter 1 for half step or 2 for full step sequence\n");
    return;
   }

  //read the Number of Steps entered by the user on terminal
  rc = fetch_uint32_arg(&STEPS);

  //error message if user doesn't enter Number of Steps
  if(rc) 
   {
    printf("Please enter number of steps\n");
    return;
   }
  
	
  //Configure GPIOC[15:0] as output, PP, high speed, no pullup/pulldown
  RCC->AHBENR |= 0x00080000; 		// Activating GPIOC clock source
  GPIOC->MODER |= 0x55555555; 		// Configuring GPIOC[15:0] as outputs
  GPIOC->OSPEEDR &= 0XAAAAAAAA; 	// Selecting low-speed on GPIOC[15:0]


  //run stepper motor for the desired No of Steps in Clockwise direction
  for(int i = 0; i<STEPS; i++)
   {
   
      //operate stepper motor in Half step sequence
      if(val == 1)
       {
	 for (int i=0;i<8;i++) 
	  {
            TaskInput();
	    GPIOC->ODR = halfstep[i];   // half step sequence
	    BSP_LED_Toggle(3);	        // toggle LED3 on STM32F3
	    TIMER_DELAY(DELAY);         // delay in ms
	  }
       }

      //operate stepper motor in Full step sequence
      else if(val == 2)
       {
	 for (int i=0;i<4;i++) 
	  {
	    TaskInput();
	    GPIOC->ODR = fullstep[i];   // full step sequence
	    BSP_LED_Toggle(4);	        // toggle LED4 on STM32F3
	    TIMER_DELAY(DELAY);	        // delay in ms
	  }
       } 
    }
}
ADD_CMD("stepclk", runStepperCLK, "<mode> <steps>	Start the Stepper Motor in clock wise");



// FUNCTION      : runStepperACLK()
// DESCRIPTION   : The function activates the stepper motor to operate
//                 in Anti-clockwise direction and also in 2 different 
//		   modes: Half-step and Full-Step
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void runStepperACLK( int mode )
{
  int rc;		// status flag to check if user inputs a value in terminal
  uint32_t val,STEPS;   // val stores the mode of stepper: Half-step or Full-step

  //initialize the LEDs
  BSP_LED_Init(5);
  BSP_LED_Init(6);

  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }

  //read the mode entered by user on terminal: half step or full step
  rc = fetch_uint32_arg(&val);
   
  //error message if user doesn't enter mode
  if(rc) 
   {
    printf("Please enter 1 for half step or 2 for full step sequence\n");
    return;
   }

  //read the Number of Steps entered by the user on terminal
  rc = fetch_uint32_arg(&STEPS);
  
  //error message if user doesn't enter Number of Steps
  if(rc) 
   {
    printf("Please enter number of steps\n");
    return;
   }
  
  //Configure GPIOC[15:0] as output, PP, high speed, no pullup/pulldown
  RCC->AHBENR |= 0x00080000; 	      // Activating GPIOC clock source
  GPIOC->MODER |= 0x55555555; 	      // Configuring GPIOC[15:0] as outputs
  GPIOC->OSPEEDR &= 0XFFFAAFFF;       // Selecting low-speed on GPIOC[15:0]

  //run stepper motor for the desired No of Steps in Anti-Clockwise direction
  for(int i = STEPS; i>0; i--)
  {
   
    //operate stepper motor in Half step sequence
    if(val == 1) 
     {
        for (int i=7;i>=0;i--) 
	 {
	   TaskInput();
	   GPIOC->ODR = halfstep[i];   // half step sequence
	   BSP_LED_Toggle(5);	       // Toggle LED5 on STM32F3
	   TIMER_DELAY(DELAY);         // delay in ms
	 }
     }

    //operate stepper motor in Full step sequence
    else if(val == 2)
     {
        for (int i=3;i>=0;i--) 
	 {
           TaskInput();
	   GPIOC->ODR = fullstep[i];   // full step sequence
	   BSP_LED_Toggle(6);          // Toggle LED6 on STM32F3
	   TIMER_DELAY(DELAY);         // delay in ms
	 }
     } 
   }
}
ADD_CMD("stepaclk", runStepperACLK, "<mode> <steps>	Start the Stepper Motor in anti clock wise");
