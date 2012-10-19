#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define CLOCKPIN   0x00
#define RESETPIN   0x01

#include "characters.c"

char screen[8];

void clock(char pin) {
    
    PORTD |= (1 << pin);
    PORTD &= (0 << pin);
}

void init() {
    
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

void clearScreen() {
    
    int i;
    
    for(i = 0; i < 8; i++)
        screen[i] = 0x00;
}


void writeScreen(char *data) {
    
    int i;
    
    for(i = 0; i < 8; i++)
        screen[i] = *(data++);
    
}

void marquee(char *data) {
    
    int i,j;

    while(*data) {
        
        // loop to shift full character
        for(i = 0; i < 8; i++) {
            
            _delay_ms(100);
            
            // loop to shift column
            for(j = 0; j < 8; j++)
                screen[j] = (character[(*data - 0x30)][j] >> (8-i));            
        }
        
        data++;
        
    }
    
    clearScreen();
}


int main(void) {
    
    init();
    
    char str[16];
    sprintf(str,"CHRIS");
    
    marquee(str);
    
    for(;;) {
        
        
    }
    
	return 0;
}

ISR(TIMER0_OVF_vect) {

    static char rowNumber = 0;
    
    PORTB = 0x00;
    
    clock(CLOCKPIN);
    
    // Check for last row
    if (rowNumber > 7) {
        rowNumber = 0;
        clock(RESETPIN);
    }
    
    PORTB = screen[rowNumber++];
    
}
