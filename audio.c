#include "audio.h"
#include "cmsis_os2.h"

// Current note positions in each melody track
volatile int currNoteAlt = 0;
volatile int currNoteMain = 0;

// Main melody (Tetris) notes and durations
int tetrisMelody[] = {
    E5, B4, C5, D5, C5, B4,
    A4, A4, C5, E5, D5, C5,
    B4, C5, D5, E5,
    C5, A4, A4, A4, B4, C5,

    D5, F5, A5, G5, F5,
    E5, C5, E5, D5, C5,
    B4, B4, C5, D5, E5,
    C5, A4, A4, REST,

    E5, B4, C5, D5, C5, B4,
    A4, A4, C5, E5, D5, C5,
    B4, C5, D5, E5,
    C5, A4, A4, A4, B4, C5,

    D5, F5, A5, G5, F5,
    E5, C5, E5, D5, C5,
    B4, B4, C5, D5, E5,
    C5, A4, A4, REST,

    E5, C5,
    D5, B4,
    C5, A4,
    GS4, B4, REST,
    E5, C5,
    D5, B4,
    C5, E5, A5,
    GS5
};

int tetrisDurations[] = {
    4, 8, 8, 4, 8, 8,
    4, 8, 8, 4, 8, 8,
    -4, 8, 4, 4,
    4, 4, 8, 4, 8, 8,

    -4, 8, 4, 8, 8,
    -4, 8, 4, 8, 8,
    4, 8, 8, 4, 4,
    4, 4, 4, 4,

    4, 8, 8, 4, 8, 8,
    4, 8, 8, 4, 8, 8,
    -4, 8, 4, 4,
    4, 4, 8, 4, 8, 8,

    -4, 8, 4, 8, 8,
    -4, 8, 4, 8, 8,
    4, 8, 8, 4, 4,
    4, 4, 4, 4,

    2, 2,
    2, 2,
    2, 2,
    2, 4, 8,
    2, 2,
    2, 2,
    4, 4, 2,
    2
};

// Alternate melody (Pacman) notes and durations
int pacmanMelody[] = {
    B4, B5, FS5, DS5, 
    B5, FS5, DS5, C5, 
    C6, G6, E6, C6, G6, E6,

    B4, B5, FS5, DS5, B5, 
    FS5, DS5, DS5, E5, F5,
    F5, FS5, G5, G5, GS5, A5, B5
};

int pacmanDurations[] = {
    16, 16, 16, 16, 
    32, -16, 8, 16,
    16, 16, 16, 32, -16, 8,

    16, 16, 16, 16, 32,
    -16, 8, 32, 32, 32,
    32, 32, 32, 32, 32, 16, 8
};

// Sets the note frequency for the buzzer
void setNote(uint32_t freq) {
    if (freq == 0) {
        stopNote();
        return;
    }
    uint32_t mod = (375000 / freq);
    TPM0->MOD = mod;
    TPM0_C5V = mod / VOLUME;
}

// Stops the note
void stopNote(void) {
    TPM0_C5V = 0;
    osDelay(50);
}

// Plays the main melody (Tetris) notes in sequence
void changeNoteMain(void) {
    setNote(tetrisMelody[currNoteMain]);
    uint16_t note_length = tetrisDurations[currNoteMain];
    uint16_t delay = (uint16_t)((double)(TEMPO / note_length) * 1.45);
    currNoteMain = (currNoteMain + 1) % (sizeof(tetrisMelody) / sizeof(int));
    osDelay(delay);
}

// Plays the alternate melody (Pacman) notes in sequence
void changeNoteAlt(void) {
    setNote(pacmanMelody[currNoteAlt]);
    uint16_t note_length = pacmanDurations[currNoteAlt];
    uint16_t delay = (uint16_t)((double)(TEMPO / note_length) * 1.45);
    currNoteAlt = (currNoteAlt + 1) % (sizeof(pacmanMelody) / sizeof(int));
    osDelay(delay);
}
