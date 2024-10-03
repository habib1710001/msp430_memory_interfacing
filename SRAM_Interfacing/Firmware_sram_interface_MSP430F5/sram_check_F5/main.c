/*-------------------------------------------------------------------------------------
 * File:        main.c
 * Ram Address: 01C00h-03BFFh
 *
 *        MSP430f5529
 *     -----------------
 * /|\ |            XIN|-
 *  |  |               | 32kHz
 *  |--|RST        XOUT|-
 *     |               |
 *     |   P4.4/UCA1TXD|------------>
 *     |               | 115200 - 8N1
 *     |   P4.5/UCA1RXD|<------------
 *     |               |
 * Input:     None
 * Output:
 * Author:    Habib Ur Rahman
 * Date:      Sept. 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h> 
#include <stdint.h>

#define TARGET_MEMORY_START_ADDRESS0 ((uint8_t*)0x003400)
#define TARGET_MEMORY_START_ADDRESS1 ((uint8_t*)0x00356C)


#define RCCTL0_BASE_ADDRESS 0x000158
#define RCCTL0_OFFSET_ADDRESS 0x00

#define RCCTL0 *((uint16_t*)(RCCTL0_OFFSET_ADDRESS + RCCTL0_BASE_ADDRESS))

#define RCKEY    (0x5A00)
#define RCRS2OFF (0x0004)
#define RCRS2ON  (0x0000)


#define BYTE_BLOCK0 1024
#define BYTE_BLOCK1 20

#define TIME_DISABLED_CHEKCK0_MS 1
#define TIME_DISABLED_CHEKCK1_MS 18
#define TIME_DISABLED_CHEKCK2_MS 20
#define TIME_DISABLED_CHEKCK3_MS 22
#define TIME_DISABLED_CHEKCK4_MS 23
#define TIME_DISABLED_CHEKCK5_MS 48



void memory_continuous_write (uint8_t* startingAddress, uint16_t number_of_bytes, uint8_t content){
    uint16_t i;
    for (i = 0; i < number_of_bytes; i++){
      *(startingAddress + i) = content;
    }
}


void init_timer(void){
    //Timer Configuration
    TA0CCR0 = 0; //Initially stop the timer
    TA0CCTL0 |= CCIE ; //Enable the interrupt for CCR0
    TA0CTL = (TASSEL_2 +  ID_0 + MC_1 + TAIE); //Select the SMCLK, SMCLK/, Up Mode
}


void ram_turn_on_after(uint8_t targetMs){
    TA0CCR0 = 1000-1; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
    RCCTL0 = (RCKEY | RCRS2OFF);
    uint8_t countMs = 0;

    while (1){
      if (TA0CCTL0 && CCIFG){
          countMs++;
      }
      if (countMs >= targetMs){
       RCCTL0 = (RCKEY | RCRS2ON);
       countMs = 0;
       break;
     }
    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    init_timer();

    while(1){
        memory_continuous_write (TARGET_MEMORY_START_ADDRESS0, BYTE_BLOCK0, 0xFF);
        ram_turn_on_after(TIME_DISABLED_CHEKCK0_MS);
    }
}
