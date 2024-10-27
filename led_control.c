#include "led_control.h"

void blinkRedLED(void) {
    PTA->PTOR |= MASK(RED_LED_01);  // Toggle red LED
}

void setGreenLED(bool turnOn) {
    if (turnOn) {
        // Turn on all green LEDs
        PTA->PSOR |= MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08);
        PTD->PSOR |= MASK(GREEN_LED_05);
        PTC->PSOR |= MASK(GREEN_LED_02) | MASK(GREEN_LED_01);
    } else {
        // Turn off all green LEDs
        PTA->PCOR |= MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08);
        PTD->PCOR |= MASK(GREEN_LED_05);
        PTC->PCOR |= MASK(GREEN_LED_02) | MASK(GREEN_LED_01);
    }
}

void cycleGreenLED(int led_pos) {
    setGreenLED(false);  // Turn off all green LEDs
    switch (led_pos) {
        case 0: PTC->PSOR |= MASK(GREEN_LED_01); break;
        case 1: PTC->PSOR |= MASK(GREEN_LED_02); break;
        case 2: PTA->PSOR |= MASK(GREEN_LED_03); break;
        case 3: PTA->PSOR |= MASK(GREEN_LED_04); break;
        case 4: PTD->PSOR |= MASK(GREEN_LED_05); break;
        case 5: PTA->PSOR |= MASK(GREEN_LED_06); break;
        case 6: PTA->PSOR |= MASK(GREEN_LED_07); break;
        case 7: PTA->PSOR |= MASK(GREEN_LED_08); break;
        default: break;
    }
}
