#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define CLOCKPIN   0x00
#define RESETPIN   0x01

#include "characters.c"

char screenBuffer[128];

char bufferSize;

void clock(char pin)
{
    PORTD |= (1 << pin);
    PORTD &= (0 << pin);
}

void init()
{
    PORTB = 0b00000000;
	DDRB = 0b11111111;
    
	PORTC = 0b00000000;
	DDRC = 0b00000000;
    
	PORTD = 0b00000000;
	DDRD = 0b00000011;
    
    TIMSK0 |= (1 << TOIE0);
    //TCCR0B |= (1 << CS00);    // Slow down interrupt for debug
    TCCR0B |= (1 << CS01);
    
    sei();
    
    clock(RESETPIN);
}

void writeScreen(char *data)
{
    char i;
    
    for (i = 0; i < 8; i++)
        screenBuffer[i] = character[*(data++)-0x30][i];
}

void writeBuffer(char *data)
{
    char i;
    char j = 0;
    
    while (*data) {
        
        for (i = 0; i < 8; i++)
            screenBuffer[j++] = character[*data-0x30][i];
        
        data++;
    }
    
    bufferSize = j;
}

void shiftBufferLeft()
{
    char i,j,t;
    
    /*for (i = 0; i <= bufferSize; i++) {
        screenBuffer[i] = (screenBuffer[i] << 1) | ((screenBuffer[i+8] >> 7) & 0x01);
    }*/
    
    for (i=0; i<8; i++) { // loop on the eight lines
        t = (screenBuffer[i] >> 7) & 0x01;  // store first bit
        for (j=i; j<bufferSize-8; j+=8) { // loop on all the bytes of the line but the last one
            screenBuffer[j] = (screenBuffer[j] << 1) | ((screenBuffer[j+8] >> 7) & 0x01);
        }
        screenBuffer[j] = (screenBuffer[j] << 1) | t; // handle the last byte of the line
    }
    
}


int main(void)
{
    init();
    
    char str[16];
    
    (void)sprintf(str,"CHRIS.");
    writeBuffer(str);
    
    for(;;){
        
        shiftBufferLeft();
        _delay_ms(20);
        
    }
    
	return 0;
}

ISR(TIMER0_OVF_vect)
{
    static char rowNumber = 0;
    
    PORTB = 0x00;
    
    clock(CLOCKPIN);
    
    // Check for last row
    if (rowNumber > 7) {
        rowNumber = 0;
        clock(RESETPIN);
    }
    
    PORTB = screenBuffer[rowNumber++];
    
}
