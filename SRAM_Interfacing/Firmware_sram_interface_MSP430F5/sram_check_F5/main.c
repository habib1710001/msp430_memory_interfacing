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

#define TARGET_MEMORY_START_ADDRESS0 ((uint8_t*)0x003500)
#define TARGET_MEMORY_START_ADDRESS1 ((uint8_t*)0x00356C)


#define RCCTL0_BASE_ADDRESS 0x000158
#define RCCTL0_OFFSET_ADDRESS 0x00

#define RCCTL0 *((uint16_t*)(RCCTL0_OFFSET_ADDRESS + RCCTL0_BASE_ADDRESS))

#define RCKEY    (0x5A00)
#define RCRS2OFF (0x0004)
#define RCRS2ON  (0x0000)


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


void ram_turn_off(){
    RCCTL0 = (RCKEY | RCRS2OFF);
    delay(10000);
    RCCTL0 = (RCKEY | RCRS2ON);
    delay(10000);
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    while(1){
        memory_continuous_write (TARGET_MEMORY_START_ADDRESS0, BYTE_BLOCK0, 0x00);

        ram_turn_off();

        memory_continuous_write (TARGET_MEMORY_START_ADDRESS1, BYTE_BLOCK1, 0x11);
    }
}
