#include <msp430.h>
#include <stdint.h>
#include <stdio.h>


#define SRAM_START 0x1C00 // Define start of SRAM
#define SECTOR_SIZE 2048 // Define size of each sector

void delay_ms(unsigned int ms);


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

void writeHexToSRAM(unsigned char data, unsigned int length) {
    unsigned int i;
    for (i = 0; i < length; i++) {
      ((volatile unsigned char *) SRAM_START)[i] = data;
    // Write data to SRAM
    }
}

void main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    char s = 0; // counts 2 KB SRAM sectors
    unsigned int i = 0; // counts bytes in an SRAM sector
    unsigned int t = 1;
    unsigned int p = 0;
    char c; // byte read from SRAM



    for (p=0; p<800; p++) {
        char * mypc = (char *) SRAM_START;
        //unsigned char imageData[] = "ImageDataPlaceholder";
        // Placeholder for image data

        //writeHexToSRAM(imageData, sizeof(imageData));
        writeHexToSRAM(0xFF, 2024);

        RCCTL0 = RCKEY | RCRS7OFF; //this is for Sector 7

        delay_ms(t);
        RCCTL0 = RCKEY;

        USCIA1_setup(); // Initialize USCI_A1 module in UART mode

        __delay_cycles(50000); // Wait a half second or so

        for (s=0; s<1; s++) {
            // print next RAM sector of 2 KB
            for (i=0; i < SECTOR_SIZE; i++) {
                c = *mypc;
                UARTA1_puthex(c);
                mypc++;
             }
       }
        char buffer[5];
        int k = 0;

        sprintf(buffer, "end%d", 1);
        for(k=0; k<5; k++) UARTA1_putchar(buffer[k]);

        t=t+1;
    }
}
