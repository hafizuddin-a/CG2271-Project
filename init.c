#include "init.h"

// Initialize general GPIO configurations
void initGPIO(void) {
    // Enable Clock to all ports used (A, C, D, E)
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);
}

// Initialize GPIO for LED control (Green LEDs 1-8 and Red LED 1)
void initLED(void) {
    // Configure each LED pin as GPIO and set as output for Green LEDs 1-8
    PORTC->PCR[GREEN_LED_01] &= ~PORT_PCR_MUX_MASK; PORTC->PCR[GREEN_LED_01] |= PORT_PCR_MUX(1);
    PORTC->PCR[GREEN_LED_02] &= ~PORT_PCR_MUX_MASK; PORTC->PCR[GREEN_LED_02] |= PORT_PCR_MUX(1);
    PORTA->PCR[GREEN_LED_03] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[GREEN_LED_03] |= PORT_PCR_MUX(1);
    PORTA->PCR[GREEN_LED_04] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[GREEN_LED_04] |= PORT_PCR_MUX(1);
    PORTD->PCR[GREEN_LED_05] &= ~PORT_PCR_MUX_MASK; PORTD->PCR[GREEN_LED_05] |= PORT_PCR_MUX(1);
    PORTA->PCR[GREEN_LED_06] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[GREEN_LED_06] |= PORT_PCR_MUX(1);
    PORTA->PCR[GREEN_LED_07] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[GREEN_LED_07] |= PORT_PCR_MUX(1);
    PORTA->PCR[GREEN_LED_08] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[GREEN_LED_08] |= PORT_PCR_MUX(1);

    // Configure Red LED
    PORTA->PCR[RED_LED_01] &= ~PORT_PCR_MUX_MASK; PORTA->PCR[RED_LED_01] |= PORT_PCR_MUX(1);

    // Set data direction registers to output
    PTA->PDDR |= MASK(GREEN_LED_03) | MASK(GREEN_LED_04) | MASK(GREEN_LED_06) | MASK(GREEN_LED_07) | MASK(GREEN_LED_08) | MASK(RED_LED_01);
    PTC->PDDR |= MASK(GREEN_LED_01) | MASK(GREEN_LED_02);
    PTD->PDDR |= MASK(GREEN_LED_05);
}

// Initialize GPIO for Buzzer control
void initBUZZER(void) {
    // Configure buzzer pin for PWM (PortD Pin 5, TPM0_CH5)
    PORTD->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BUZZER_PIN] |= PORT_PCR_MUX(4);

    // Enable clock gating for Timer0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // Set up TPM for Edge-Aligned PWM
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  // Select clock source

    TPM0->MOD = 3500;  // Set modulo value for the timer

    // Configure TPM0 Channel 5 for PWM
    TPM0_C5SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
    TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

    // Set prescaler and start the timer
    TPM0->SC &= ~((TPM_SC_PS_MASK) | (TPM_SC_CMOD_MASK));
    TPM0->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);

    // Turn off the buzzer initially
    TPM0_C5V = 0;
}