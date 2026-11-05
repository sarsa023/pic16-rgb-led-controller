
#include<pic16f877a.h>
//#include <xc.h>

//#define _XTAL_FREQ 8000000
#define row1 PORTBbits.RB0
#define row2 PORTBbits.RB1
#define row3 PORTBbits.RB2
#define row4 PORTBbits.RB3
#define col1 PORTBbits.RB7
#define col2 PORTBbits.RB6
#define col3 PORTBbits.RB5
#define col4 PORTBbits.RB4

unsigned char red_duty, green_duty, blue_duty, Display0, Display1, Display2, DispColor;
unsigned char in, color;
unsigned char r = 10, g = 11, b = 12, enter = 13, reset = 14, erase = 15, red_upd = 0, red_counter = 0;

unsigned char segRed = 0b00110001, segGreen = 0b01101111, segBlue = 0b01111100, segDash = 0b01000000;
unsigned char seg[10] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, // 0 1 2 3 4
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111  //5 6 7 8 9
};



void update_leds(void);
unsigned char key(void);


void __interrupt() ISR(){
    if(INTCONbits.T0IF){ // TMR0 CHECK
        if(PORTCbits.RC0){
            PORTCbits.RC0 = 0;
            TMR0 = red_duty;
        } else {
            if(red_duty != 0) PORTCbits.RC0 = 1;
            TMR0 = -red_duty;
        }
        
        INTCONbits.T0IF = 0;
        INTCONbits.T0IE = 1;
    }
    if(INTCONbits.RBIF){ //RBIF CHECK
        in = key();
        if(in == r || in == g || in == b){
            color = in;
            if(color == r){
                red_upd = 0;
                DispColor = segRed;
            }
            if(color == g){
                green_duty = 0;
                DispColor = segGreen;
            }
            if(color == b){
                blue_duty = 0;
                DispColor = segBlue;
            }
            Display0 = Display1 = Display2 = seg[0];
        }
        else if(in == enter) update_leds();
        else if(in == reset){
            red_upd = blue_duty = green_duty = 0;
            Display0 = Display1 = Display2 = DispColor = segDash;
            update_leds();
        } else if(in == erase) {
             if(color == r){
                red_upd /= 10;
            }
            else if(color == g){
                green_duty /= 10;
            }
            else if(color == b){
                blue_duty /= 10;
            }
            Display0 = Display1;
            Display1 = Display2;
            Display2 = seg[0];
        }
        else {
            if(color == r){
                red_upd *= 10;
                red_upd += in;
            }
            else if(color == g){
                green_duty *= 10;
                green_duty += in;
            }
            else if(color == b){
                blue_duty *= 10;
                blue_duty += in;
            }
            Display2 = Display1;
            Display1 = Display0;
            Display0 = seg[in];
        }
        PORTB = 0xF0;
        INTCONbits.RBIF = 0;
        INTCONbits.RBIE = 1; // RBIE ON
    }
    return;
}

int main(void) {
    TRISC = 0;
    TRISD = 0; // set as outputs
    TRISB = 0xF0; // MSBs as inputs for keypad
    OPTION_REGbits.T0CS = 0; // timer0 to system clock
    
    PORTB = 0xF0; // initialize PORTB inputs to 1
    PORTC = 0xF0; // initializing, LEDs off, 7seg enablers to 1, digits are enabled by 0
    PORTD = 0xFF;
    
    PR2 = 0xFF;
    CCP1CON = 0x0C; //PWM mode
    CCP2CON = 0x0C;
    
    INTCONbits.RBIF = 0; //clear flags
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.RBIE = 1; // then enable interrupts
    INTCONbits.GIE = 1;
    
    
    red_duty = blue_duty = green_duty = 0;
    update_leds();
    Display0 = Display1 = Display2 = DispColor = segDash;
    
    TMR0 = 0;
    T2CONbits.TMR2ON = 1; // T2ON set
    
    for(unsigned char i = 0; ; i = (i + 1) % 4){
        PORTC |= 0b11110000; //RC7 = RC6 = RC5 = RC4 = 1 all digits off
        if(i == 0){
            PORTCbits.RC7 = 0; 
            PORTD = DispColor;
        }
        if(i == 1){
            PORTCbits.RC6 = 0; 
            PORTD = Display2;
        }
        if(i == 2){
            PORTCbits.RC5 = 0;
            PORTD = Display1;
        }
        if(i == 3){
            PORTCbits.RC4 = 0;
            PORTD = Display0;
        }
        //__delay_ms(1);
        for(unsigned char j = 0; j < 2; j++) for(unsigned char k = 0; k < 255; k++);
    }
}


void update_leds(void){
    CCP1CONbits.CCP1X = blue_duty & 1; // checking xxx1
    CCP1CONbits.CCP1Y = blue_duty & 2; // checking xx1x
    CCPR1L = blue_duty >> 2;
    
    CCP2CONbits.CCP2X = green_duty & 1;
    CCP2CONbits.CCP2Y = green_duty & 2;
    CCPR2L = green_duty >> 2;
       
    PORTCbits.RC0 = 1;
    red_duty = red_upd;
    TMR0 = -red_duty;  

    
    INTCONbits.T0IF = 0; 
    INTCONbits.T0IE = 1; 
}

unsigned char keypad[4][4]={
    {7,8,9,10},
    {4,5,6,11},
    {1,2,3,12},
    {15,0,14,13}};
unsigned char rowloc, colloc;


unsigned char key()
{
    PORTB = 0;
    while(col1 && col2 && col3 && col4); // if called without change (rare))
    while(!col1 || !col2 || !col3 || !col4) { // some key pressed
        row1 = 0;
        row2 = row3 = row4 = 1;
        if(!col1 || !col2 || !col3 || !col4) {
            rowloc = 0;
            break;
        }
        row2 = 0; row1 = 1;
        if(!col1 || !col2 || !col3 || !col4) {
            rowloc = 1;
            break;
        }
        row3 = 0; row2 =  1;
        if(!col1||!col2||!col3||!col4) {
            rowloc = 2;
            break;
        }
        row4 = 0; row3 = 1;
        if(!col1 || !col2 || !col3 || !col4){
            rowloc = 3;
            break;
        }
    }
    if(col1 == 0 && col2 != 0 && col3 != 0 && col4 != 0)
            colloc = 0;
    else if(col1 != 0 && col2 == 0 && col3 != 0 && col4 != 0)
            colloc = 1;
    else if(col1 != 0 && col2 != 0 && col3 == 0 && col4 != 0)
            colloc = 2;
    else if(col1 != 0 && col2 != 0 && col3 != 0 && col4 == 0)
            colloc = 3;
    while(col1 == 0 || col2 == 0 || col3 == 0 || col4 == 0);
    return (keypad[rowloc][colloc]);
}