#include "MKL25Z4.h"
#include "cmsis_os2.h"

#define MASK(x) (1 << (x))

// LED control pin definitions
#define GREEN_LED_01 0  // PortC Pin 0
#define GREEN_LED_02 7  // PortC Pin 7
#define GREEN_LED_03 1  // PortA Pin 1
#define GREEN_LED_04 2  // PortA Pin 2
#define GREEN_LED_05 4  // PortD Pin 4
#define GREEN_LED_06 12 // PortA Pin 12
#define GREEN_LED_07 4  // PortA Pin 4
#define GREEN_LED_08 5  // PortA Pin 5
#define RED_LED_01 13   // PortA Pin 13

// Buzzer control pin definition
#define BUZZER_PIN 5  // PortD Pin 5 for buzzer output

// Function declarations
void initGPIO(void);
void initLED(void);
void initBUZZER(void);
