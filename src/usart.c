/*
   FILE          : usart.c
   PROJECT       : CNTR8000-16S
   PROGRAMMER    : Krishnan-Bengaluru,Srinidhi  Student
   FIRST VERSION : June 16, 2016
   DESCRIPTION   : This program configures GIPOC(PC.4 and PC.5) and
		   USART1 to send and receive data using a terminal
		   command 'usart'.
*/



//header files
#include <stdint.h>
#include <stdio.h>
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "common.h"




// FUNCTION      : configureUSART()
// DESCRIPTION   : The function configures the GPIOC and USART1
//                 send and receive data
// PARAMETERS    : The function accepts a uint32_t 
// RETURNS       : returns nothing
void configureUSART(uint32_t val) {

//enable GPIOC clock
RCC->AHBENR 	|= RCC_AHBENR_GPIOCEN;  

//Configure PC4(TX) pin
GPIOC->MODER 	|= 0X00000200;             //GPIO Mode: Alternate function
GPIOC->OTYPER 	|= 0X00000010;             //Output Type: Open Drain
GPIOC->OSPEEDR 	|= 0X00000300;             //GPIO Speed: 50MHz    
GPIOC->PUPDR 	|= 0X00000100;             //PULL UP
GPIOC->AFR [0] 	|= 0X00070000;             //AF7: Alternate function register
	
//Configure PC5(RX) pin
GPIOC->MODER   |= 0X00000800;              //GPIO Mode: Alternate Functions
GPIOC->OTYPER  |= 0X00000020;              //Output Type: Open Drain
GPIOC->OSPEEDR |= 0X00000C00;              //GPIO Speed: 50MHz    
GPIOC->PUPDR   |= 0X00000400;              //PULL UP 
GPIOC->AFR [0] |= 0X00700000;              //AF7: Alternate function register

//Initialize the USART1
RCC->APB2ENR |= RCC_APB2ENR_USART1EN;      //CLK enable for USART1
USART1->BRR   = (72000000/9600) & 0XFFFF;  //f=8Mhz, Baud Rate: 9600
USART1->CR1  &= ~USART_CR1_OVER8;     	   //Oversampling mode = 16
USART1->CR1  &= ~USART_CR1_M;              //Word length = 8 bits
USART1->CR1  &= ~USART_CR1_PCE;            //No parity
USART1->CR1  |= USART_CR1_TE;              //Transmitter enable
USART1->CR1  |= USART_CR1_RE;              //Receiver enable
USART1->CR1  |= USART_CR1_UE;	           //USART enable
USART1->CR2  &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0); // one stop bit

//transmit data
USART1->TDR = val;
printf("Transmit data = %x\n", USART1->TDR);

//wait for TX
while ((USART1->ISR & USART_ISR_TC) == 0);

//Wait until the data is ready to be received
while ((USART1->ISR & USART_ISR_RXNE) == 0);

// read RX data, combine with DR mask (we only accept a max of 9 Bits)
USART1->RDR = USART1->RDR & 0x1FF;
printf("Recieve data = %x\n", USART1->RDR);

}




// FUNCTION      : cmdUSART()
// DESCRIPTION   : The function accepts the input terminal command
//                 'usart' and data to Tx and Rx
// PARAMETERS    : The function accepts an int parameter
// RETURNS       : returns nothing
void CmdUSART(int mode){
	
        uint32_t val;
        int rc;

	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}

        rc = fetch_uint32_arg(&val);

        if(rc) {
         printf("Missing Transmit data\n");
         return;
        }
    
        configureUSART(val);
        
}
ADD_CMD("usart", CmdUSART, "		The USART value");

