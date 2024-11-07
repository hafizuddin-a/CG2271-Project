#include "RTE_Components.h"
#include CMSIS_device_header
#include "init.h"
#include "led_control.h"  // LED control functions
#include "audio.h"        // Audio control functions
#include "motor_control.h"// Motor control functions
#define baudRate 9600

uint8_t rx_IRQ_data = 0x00;

// Message queues for inter-task communication
osMessageQueueId_t msgGreenLED, msgRedLED, msgBuzzer, msgMotor, msgBrain;

// UART communication
void UART2_IRQHandler(void) {
  NVIC_ClearPendingIRQ(UART2_IRQn);
  // If receiver is full
  if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_IRQ_data = UART2->D;
  }
	osMessageQueuePut(msgBrain, &rx_IRQ_data, NULL, 0);

  // Error checking
  if (UART2->S1 & (UART_S1_OR_MASK | 
									 UART_S1_NF_MASK | 
                   UART_S1_FE_MASK | 
                   UART_S1_PF_MASK)) {
    // TODO: Handle error
    // TODO: Clear Flag
    return;
  }
}

// Brain 
void tBrain(void *argument) {
	uint8_t rx = 0x00;
	int playAltMelody = 0;
	int isStopped = 0;
	for (;;) {
		osMessageQueueGet(msgBrain, &rx, NULL, 0);
		isStopped = rx ? 0 : 1;
		playAltMelody = (rx == 42) ? !playAltMelody : playAltMelody;
		
		osMessageQueuePut(msgMotor, &rx, NULL, 0);
		osMessageQueuePut(msgGreenLED, &isStopped, NULL, 0);
		osMessageQueuePut(msgRedLED, &isStopped, NULL, 0);
		osMessageQueuePut(msgBuzzer, &playAltMelody, NULL, 0);
	}
}

// LED control task for green LEDs
void tGreenLED(void *argument) {
    uint8_t isStopped = 1;
    int i = 0;

    for (;;) {
        osMessageQueueGet(msgGreenLED, &isStopped, NULL, 0);
        if (isStopped) {
            setGreenLED(true);  // Turn on all green LEDs when stopped
        } else {
            i = (i >= 7) ? 0 : i + 1;  // Cycle only LEDs 1-8
            cycleGreenLED(i);
        }
        osDelay(100);
    }
}

// LED control task for red LEDs
void tRedLED(void *argument) {
    uint8_t isStopped = 1;
    uint16_t delay = 250;

    for (;;) {
        osMessageQueueGet(msgRedLED, &isStopped, NULL, 0);
        delay = isStopped ? 250 : 500;
        blinkRedLED();
        osDelay(delay);
    }
}

// Audio control task
void tBuzzer(void *argument) {
    uint8_t playAltMelody = 0;

    for (;;) {
        osMessageQueueGet(msgBuzzer, &playAltMelody, NULL, 0);
        
        if (playAltMelody) {
            changeNoteAlt();  // Play alternate melody
        } else {
            changeNoteMain(); // Play main melody
        }
        osDelay(50); // Delay to ensure notes are not skipped
    }
}

void tMotor(void *argument) {
	uint8_t rx = 0x00;
	for (;;) {
		osMessageQueueGet(msgMotor, &rx, NULL, 0);
		switch(rx) {
			case 18:
				forward();
				break;
			case 22: // FR
				forwardRight();
				break;
			case 26: // FL
				forwardLeft();
				break;
			case 15: // BR
				backwardRight();
				break;
			case 19: // BL
				backwardLeft();
				break;
			case 11: // B
				backward();
				break;
			case 4: // clckwise
				clockwise();
				break;
			case 8: //anticlckwise
				antiClockwise();
				break;
			default:
				stop();
				break;
		}
	}
}

int main(void) {
    SystemCoreClockUpdate();
    initGPIO();
    initLED();
    initBUZZER();
		initMotor();
		initUART2(baudRate);

    osKernelInitialize();

    // Create LED tasks and message queues
    osThreadNew(tGreenLED, NULL, NULL);
    msgGreenLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    osThreadNew(tRedLED, NULL, NULL);
    msgRedLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    // Create audio task and message queue
    osThreadNew(tBuzzer, NULL, NULL);
    msgBuzzer = osMessageQueueNew(2, sizeof(uint8_t), NULL);
	
		// Create Motor task and message queue
		osThreadNew(tMotor, NULL, NULL);
		msgMotor = osMessageQueueNew(2, sizeof(uint8_t), NULL);
		
		// Create brain task and message queue
		osThreadNew(tBrain, NULL, NULL);
		msgBrain = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    osKernelStart();  // Start RTOS kernel

    for (;;) { }
}
