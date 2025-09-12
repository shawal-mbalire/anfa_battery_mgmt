#include <stdio.h>
// these are event driven active objects with queueing of events.
// states
typedef enum {
    WAIT_BUTTON,
    BLINK_LED,
    PAUSE_LED,
    BOOM,
    STATES_MAX
} State;

// event signals
typedef enum {
    BUTTON_PRESSED,
    TIMER_EXPIRED,
    INIT_SIGNAL,
    LED_ON,
    LED_OFF,
    SIGNALS_MAX
} EventSignal;


// LED
typedef struct {
    int is_on;
    char port[3];
} LED;

// machine 
typedef struct {
    LED led;
    State state;
    EventSignal event;
    // add event queue here if needed
} Machine;

typedef void (*StateHandler)(Machine *machine);
