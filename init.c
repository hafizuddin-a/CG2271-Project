#include "init.h"

// Initialize general GPIO configurations
void initGPIO(void) {
    // Enable Clock to all ports used (A, C, D, E)
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);
}

void initUART2(uint32_t BR) {

		uint32_t divisor, bus_clock;

		SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
		PORTE->PCR[UART_TX] = PORT_PCR_MUX(4);
		PORTE->PCR[UART_RX] = PORT_PCR_MUX(4);
		// Ensure TX and RX are disabled before configuration
		UART2->C2 &=  ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
		bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
		divisor = bus_clock/(BR*16);
		UART2->BDH = UART_BDH_SBR(divisor>>8);
		UART2->BDL = UART_BDL_SBR(divisor);
		 // No parity, 8 bits, two stop bits, other settings;
		UART2->C1 = UART2->S2 = UART2->C3 = 0;

    // Queue
    NVIC_SetPriority(UART2_IRQn, 128);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_EnableIRQ(UART2_IRQn);

    // Enable TX and RX Interrupts
    UART2->C2 |= (UART_C2_RIE_MASK);

    // Initialise both tx_q and rx_q
    //Q_Init(&tx_q);
    //Q_Init(&rx_q);

    UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK)); //Enable TX and RX
}

void initMotor(void) {
			// Enable clock for port B
    SIM_SCGC5 |= (SIM_SCGC5_PORTB_MASK );
		// Enable TPM1 clock
		SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;
		SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK; 
	
		SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
		// Set Port B pin 0, 1 to PWM and 18 (red led) to GPIO
    PORTB->PCR[rightForward] &= ~PORT_PCR_MUX_MASK; 
    PORTB->PCR[rightForward] |= PORT_PCR_MUX(3);			// TPM1_CH0

    PORTB->PCR[rightBackawrd] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[rightBackawrd] |= PORT_PCR_MUX(3);			// TPM1_CH1
	
		PORTB->PCR[leftForward] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[leftForward] |= PORT_PCR_MUX(3);				// TPM2_CH0
	
		PORTB->PCR[leftBackawrd] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[leftBackawrd] |= PORT_PCR_MUX(3); 			// TPM2_CH1
	
		PORTB->PCR[LED_RED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LED_RED] |= PORT_PCR_MUX(1);
	
	// Clear CMOD to disabler timer and prescaler
		TPM1->SC &= ~TPM_SC_CMOD_MASK;
		TPM1->SC &= ~TPM_SC_PS_MASK;
		TPM2->SC &= ~TPM_SC_CMOD_MASK;
		TPM2->SC &= ~TPM_SC_PS_MASK;
		
		TPM1_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK |
		TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
		TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK |
		TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
		
		TPM2_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK |
		TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
		TPM2_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK |
		TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
		// Prescaler of 128
		TPM1->SC |= TPM_SC_PS(7);
		TPM2->SC |= TPM_SC_PS(7);
		// Edge aligned pwm
		TPM1->SC |= TPM_SC_CMOD(1); 		// Internal clock
		TPM1->SC |= TPM_SC_CPWMS_MASK; 
		TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		TPM1_C0V = 0;
		TPM1_C1V = 0;
		
		TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_CPWMS_MASK); 		// Internal clock
		TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		TPM2_C0V = 0;
		TPM2_C1V = 0;
		// MOD values
		TPM1_MOD = 3750; // 50hz
		TPM2_MOD = 3750;
		// Set Port B pin 0, 1, 18 to output. Set pin 1 and 0 to high and pin 18 to low (active low led)
		PTB->PDDR |= (MASK(rightForward) | MASK(rightBackawrd) | MASK(LED_RED));
		PTB->PDOR |= (MASK(rightForward) | MASK(rightBackawrd));
		PTB->PDOR |= MASK(LED_RED);
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
