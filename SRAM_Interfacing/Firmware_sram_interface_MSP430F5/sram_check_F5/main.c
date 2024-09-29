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

#define TARGET_MEMORY_START_ADDRESS0 ((uint8_t*)0x01C6C)
#define TARGET_MEMORY_START_ADDRESS1 ((uint8_t*)0x01D20)

#define BYTE_BLOCK0 30
#define BYTE_BLOCK1 20


void delay (uint32_t delayIteration){
    volatile uint32_t j;
    for (j = 1; j < delayIteration; j++){
      //looping delay
    }
}

void memory_continuous_write (uint8_t* startingAddress, uint8_t number_of_bytes, uint8_t content){
    uint16_t i;
    for (i = 0; i < number_of_bytes; i++){
      *(startingAddress + i) = content;
    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    while(1){

        delay(10000);

        memory_continuous_write (TARGET_MEMORY_START_ADDRESS0, BYTE_BLOCK0, 0x00);
        memory_continuous_write (TARGET_MEMORY_START_ADDRESS1, BYTE_BLOCK1, 0x11);
    }
}
