/*-------------------------------------------------------------------------------------
 * File:main.c
 * Ram Address: 01C00h-03BFFh
 * Author: Habib Ur Rahman
 * Date:Sept. 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h> 
#include <stdint.h>
#include <stdio.h>

#define TARGET_MEMORY_START_ADDRESS0 ((uint8_t*)0x003400)


#define RCCTL0_BASE_ADDRESS 0x000158
#define RCCTL0_OFFSET_ADDRESS 0x00

#define RCCTL0 *((uint16_t*)(RCCTL0_OFFSET_ADDRESS + RCCTL0_BASE_ADDRESS))

#define RCKEY    (0x5A00)

#define RCRS2OFF (0x0004)
#define RCRS2ON  (0x0000)

#define RCRS7OFF (0x0080)
#define RCRS7ON  (0x0000)


#define BYTE_BLOCK0 2048

#define TIME_DISABLED_CHEKCK0_MS 1
#define TIME_DISABLED_CHEKCK1_MS 5
#define TIME_DISABLED_CHEKCK2_MS 10
#define TIME_DISABLED_CHEKCK3_MS 15
#define TIME_DISABLED_CHEKCK4_MS 18
#define TIME_DISABLED_CHEKCK5_MS 25
#define TIME_DISABLED_CHEKCK6_MS 10000


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
    RCCTL0 = (RCKEY | RCRS7OFF);
    uint8_t countMs = 0;

    while (1){
      if (TA0CCTL0 && CCIFG){
          countMs++;
      }
      if (countMs >= targetMs){
       RCCTL0 = (RCKEY | RCRS7ON);
       countMs = 0;
       break;
     }
    }
}


void ram_turn_off(){

}

void ram_turn_on(){

}

void delay_ms(unsigned int ms)
{
    while (ms)
    {
        __delay_cycles(1000); //1000 for 1MHz and 16000 for 16MHz
        ms--;
    }
}

void USCIA1_setup(void) {
    P4SEL |= BIT4 + BIT5; // Set USCI_A1 RXD/TXD to receive/transmit data
    UCA1CTL1 |= UCSWRST; // Set software reset during initialization
    UCA1CTL0 = 0; // Clear USCI_A1 control register
    UCA1CTL1 |= UCSSEL_2; // Set clock source to SMCLK
    UCA1BR0 = 0x09; // Set baud rate (lower byte)
    UCA1BR1 = 0x00; // Set baud rate (upper byte)
    UCA1MCTL = 0x02; // Set modulation
    UCA1CTL1 &= ~UCSWRST; // Initialize USCI state machine
}

void UARTA1_putchar(char c) {
    while (!(UCA1IFG & UCTXIFG)); // Wait for transmission buffer to be ready
    UCA1TXBUF = c; // Transmit character
}

void UARTA1_puthex(char c) {
    char hv[5];
    unsigned int j;
    sprintf(hv, "0x%x,", c); // Convert byte to hex string


    for(j = 0; j < 5; j++) {
       UARTA1_putchar(hv[j]); // Transmit each character
    }
}

int iterate = 1;

int main(void)
{
     P1DIR |= 00000001;

     WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
     USCIA1_setup(); // Initialize USCI_A1 module in UART mode
     char s = 0; // counts 2 KB SRAM sectors
     unsigned int i = 0; // counts bytes in an SRAM sector
     unsigned int t = 1;
     unsigned int p = 0;
     char c; // byte read from SRAM
     memory_continuous_write (TARGET_MEMORY_START_ADDRESS0, BYTE_BLOCK0, 0xFF);

     while(iterate == 1) {
        P1OUT ^= 0x01;
        char * mypc = (char *) TARGET_MEMORY_START_ADDRESS0;


        RCCTL0 = (RCKEY | RCRS2OFF);

        delay_ms(60);

        RCCTL0 = (RCKEY | RCRS2ON);



        __delay_cycles(50000); // Wait a half second or so

        // print next RAM sector of 2 KB
        for (i=0; i < 2048; i++) {
            c = *mypc;
            UARTA1_puthex(c);
            mypc++;
         }


        char buffer[5];
        int k = 0;

        sprintf(buffer, "end%d", 1);

        for(k=0; k<5; k++) UARTA1_putchar(buffer[k]);

        t=t+1;

        iterate = 0;
    }
}
