// main.h - Shared types for state machine demo
#ifndef STATE_PRACTICE_MAIN_H
#define STATE_PRACTICE_MAIN_H

#include <stdio.h>

// States
typedef enum State {
    STATE_ON = 0,
    STATE_OFF = 1,
} State;

// Signals: These signal events
typedef enum Signal {
    TIMEOUT_SIG = 0,
    INIT_SIG = 1,
    BUTTON_PRESS_SIG = 2,
    BUTTON_RELEASE_SIG = 3,
} Signal;

// LEDs
typedef enum LedState {
    LED_ON = 0,
    LED_OFF = 1,
} LED;

// Button
typedef enum ButtonState {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED = 1,
} Button;

// State machine structure
typedef struct {
    State state;
    Signal signal;
} Machine;

#endif // STATE_PRACTICE_MAIN_H