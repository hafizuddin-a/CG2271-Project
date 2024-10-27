#include "RTE_Components.h"
#include CMSIS_device_header
#include "init.h"
#include "led_control.h"  // LED control functions
#include "audio.h"        // Audio control functions

// Message queues for inter-task communication
osMessageQueueId_t msgGreenLED, msgRedLED, msgBuzzer;

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

int main(void) {
    SystemCoreClockUpdate();
    initGPIO();
    initLED();
    initBUZZER();

    osKernelInitialize();

    // Create LED tasks and message queues
    osThreadNew(tGreenLED, NULL, NULL);
    msgGreenLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    osThreadNew(tRedLED, NULL, NULL);
    msgRedLED = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    // Create audio task and message queue
    osThreadNew(tBuzzer, NULL, NULL);
    msgBuzzer = osMessageQueueNew(2, sizeof(uint8_t), NULL);

    osKernelStart();  // Start RTOS kernel

    for (;;) { }
}
