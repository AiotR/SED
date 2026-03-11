#include <reg552.h> /* 80C552 espezifikoa: P4, ADC, PWM, I2C SFR guztiak barnebiltzen ditu */

void init_sys(void) {
    // Initial configuration for specific SFRs, Timer, UART, interrupts, etc.
}

void main(void) {
    init_sys();
    
    while(1) {
        // Main loop
    }
}
