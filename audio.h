#include "MKL25Z4.h"
#include "init.h"

// Frequency definitions for musical notes used in melodies
#define B4 494
#define C5 523
#define CS5 554
#define D5 587
#define DS5 622
#define E5 659
#define F5 698
#define FS5 740
#define G5 784
#define GS5 831
#define A5 880
#define AS5 932
#define B5 988
#define C6 1047
#define E6 1319
#define G6 1568

#define A4 440
#define GS4 415
#define REST 0

// Volume and tempo constants
#define VOLUME 10         // Higher value = quieter sound
#define TEMPO 1200        // Base tempo for melody timing

// External variables for current note positions in melodies
extern volatile int currNoteAlt;
extern volatile int currNoteMain;

// Function declarations for audio control
void changeNoteAlt(void);     // Plays the alternate melody (Pacman)
void changeNoteMain(void);    // Plays the main melody (Tetris)
void stopNote(void);          // Stops the buzzer output
void setNote(uint32_t freq);  // Sets the frequency of the buzzer output
