#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "characters.c"

#define CLOCKPIN   0x00
#define RESETPIN   0x01

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

void writeCharacter(char *data) {
    
    int i;
    
    for(i = 0; i < 8; i++)
        screen[i] = pgm_read_byte(&character[*data][i]);
    
}

void marquee(char *data) {
    
    int i,j;

    while (*data) {
        
        // loop to shift full character
        for(i = 0; i < 8; i++) {
            
            _delay_ms(12);
            
            // loop to shift column
            for(j = 0; j < 8; j++)
                screen[j] = (screen[j] << 1) | (pgm_read_byte(&character[*data][j]) >> (8-i));
        }
        
        if (!(*(++data))) break;
        
        // loop to shift full character
        for(i = 0; i < 8; i++) {
            
            _delay_ms(12);
            
            // loop to shift column
            for(j = 0; j < 8; j++)
                screen[j] = (screen[j] << 1) | (pgm_read_byte(&character[*data][j]) >> (8-i)) ;
        }
        
        data++;
        
    }
    
    // loop to shift full character
    for(i = 0; i < 8; i++) {
        
        _delay_ms(15);
        
        // loop to shift column
        for(j = 0; j < 8; j++)
            screen[j] <<= 1 ;
    }
    
    
}


int main(void) {
    
    init();
    
    char str[32];
    int number = 0;
    
    sprintf(str,"CMJDEV");
    
    marquee(str);
    
    sprintf(str,"%i",number);
    writeCharacter(str);
    
    for(;;) {
        
        if (!(PIND & 0b10000000)) {
            number++;
            sprintf(str,"%i",number);
            writeCharacter(str);
            _delay_ms(25);
        }
        
    }
    
	return 0;
}

ISR(TIMER0_OVF_vect) {

    static int rowNumber = 0;
    
    PORTB = 0x00;
    
    clock(CLOCKPIN);
    
    // Check for last row
    if (rowNumber > 7) {
        rowNumber = 0;
        clock(RESETPIN);
    }
    
    PORTB = screen[rowNumber++];
    
}
