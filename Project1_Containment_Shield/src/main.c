/*
 * Project: Three-Zone Spatial Containment Shield
 * MCU: PIC18F25K20 | Compiler: XC8
 */

#include <xc.h>         // Include hardware definitions for PIC
#include <stdint.h>     // Include standard integer types


// PIC18F25K20 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 18        // Brown Out Reset Voltage bits (VBOR set to 1.8 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bit (WDT is controlled by SWDTEN bit of the WDTCON register)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC starts clocking the CPU without waiting for the oscillator to stablize.)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define _XTAL_FREQ 16000000 // Define clock speed as 16MHz for __delay_ms()

// Hardware Pin Aliases
#define TRIG    LATCbits.LATC0  // Pin 11: Sends trigger pulse
#define ECHO    PORTCbits.RC1   // Pin 12: Receives echo signal
#define BUZZER  LATCbits.LATC3  // Pin 14: Buzzer output
#define IR_SENS PORTCbits.RC4   // Pin 15: Infrared motion input
#define RED1    LATAbits.LATA0  // Pin 2: Red Alarm LED
#define GRN1    LATAbits.LATA2  // Pin 4: Green Safe LED

void main(void) {
    // Setup Oscillator to 16MHz and set all pins to digital mode
    OSCCONbits.IRCF = 0b111; ANSEL = 0x00; ANSELH = 0x00; 
    
    // Set Data Direction (TRIS): 0 = Output, 1 = Input
    TRISA = 0x00; TRISB = 0x00; 
    TRISCbits.TRISC0 = 0; TRISCbits.TRISC1 = 1; 
    TRISCbits.TRISC3 = 0; TRISCbits.TRISC4 = 1; 
    
    // Set initial output states to 0 (OFF)
    LATA = 0x00; TRIG = 0; BUZZER = 0; 
    
    __delay_ms(1500); // 1.5s warmup delay for IR sensor stability
    
    // Timer1 setup: 1:4 Prescaler, 16-bit mode, off initially
    T1CONbits.RD16 = 1; T1CONbits.TMR1CS = 0; T1CONbits.T1CKPS = 0b10; T1CONbits.TMR1ON = 0;    

    while(1) { // Infinite main loop
        uint16_t dist = 0;          // Variable to store calculated distance
        uint8_t warning = IR_SENS;  // Read the status of the IR sensor

        // Ultrasonic Distance Measurement
        TRIG = 1; __delay_us(10); TRIG = 0; // Send 10us trigger pulse
        uint16_t timeout = 0;
        while(ECHO == 0) { timeout++; if(timeout > 20000) break; } // Wait for echo
        if(ECHO == 1) { // If echo received
            TMR1H = 0; TMR1L = 0; PIR1bits.TMR1IF = 0; // Reset timer
            T1CONbits.TMR1ON = 1;                      // Start timer
            while(ECHO == 1 && PIR1bits.TMR1IF == 0);  // Measure pulse width
            T1CONbits.TMR1ON = 0;                      // Stop timer
            dist = ((TMR1H << 8) | TMR1L) / 58;        // Convert time to cm
        }

        // Logic Zones
        if (dist > 0 && dist < 20) { // ZONE 3: CRITICAL (Closer than 20cm)
            RED1 = 1; GRN1 = 0;      // Alarm LED ON
            for(int i = 0; i < 50; i++) { // Loop 50 times for warble duration
                // Tone 1: 4kHz frequency
                for(int j = 0; j < 15; j++) { BUZZER = 1; __delay_us(125); BUZZER = 0; __delay_us(125); }
                // Tone 2: 3.5kHz frequency (creates dissonance)
                for(int j = 0; j < 15; j++) { BUZZER = 1; __delay_us(143); BUZZER = 0; __delay_us(143); }
            }
        } 
        else if ((dist >= 20 && dist <= 100) || warning) { // ZONE 1 & 2: WARNING
            RED1 = 1; GRN1 = 0;      // Alarm LED ON
            for(int i = 0; i < 100; i++) { // Create monotone beep
                BUZZER = 1; __delay_us(500); // 1kHz tone frequency
                BUZZER = 0; __delay_us(500);
            }
        } 
        else { // SAFE ZONE
            RED1 = 0; GRN1 = 1; BUZZER = 0; // LEDs status for safe mode
            __delay_ms(100);                // Refresh rate delay
        }
    }
}
