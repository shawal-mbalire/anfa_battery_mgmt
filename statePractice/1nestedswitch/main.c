/*
 * main.c
 * Author: Shawal Mbalire
 * Description: This is the main invocation of the state machine
 */
#include "main.h"

static void handle_state_off(Machine *machine, LED *led) {
    switch (machine->signal) {
        case TIMEOUT_SIG:
            printf("OFF state: TIMEOUT_SIG received, transitioning to ON state\n");
            machine->state = STATE_ON;
            *led = LED_ON;
            break;
        case BUTTON_PRESS_SIG:
            printf("OFF state: BUTTON_PRESS_SIG received, transitioning to ON state\n");
            machine->state = STATE_ON;
            *led = LED_ON;
            break;
        case BUTTON_RELEASE_SIG:
            printf("OFF state: BUTTON_RELEASE_SIG received, staying in OFF state\n");
            *led = LED_OFF;
            break;
        default:
            printf("No transition from OFF state on this signal\n");
            break;
    }
}

static void handle_state_on(Machine *machine, LED *led) {
    switch (machine->signal) {
        case TIMEOUT_SIG:
            printf("ON state: TIMEOUT_SIG received, transitioning to OFF state\n");
            machine->state = STATE_OFF;
            *led = LED_OFF;
            break;
        case BUTTON_PRESS_SIG:
            printf("ON state: BUTTON_PRESS_SIG received, staying in ON state\n");
            *led = LED_ON;
            break;
        case BUTTON_RELEASE_SIG:
            printf("ON state: BUTTON_RELEASE_SIG received, transitioning to OFF state\n");
            machine->state = STATE_OFF;
            *led = LED_OFF;
            break;
        default:
            printf("No transition from ON state on this signal\n");
            break;
    }
}

static void dispatch(Machine *machine, LED *led) {
    if (machine->signal == INIT_SIG) {
        machine->state = STATE_OFF;
        printf("Machine initialized to OFF state\n");
        return;
    }

    switch (machine->state) {
        case STATE_OFF:
            handle_state_off(machine, led);
            break;
        case STATE_ON:
            handle_state_on(machine, led);
            break;
        default:
            printf("Unknown state\n");
            break;
    }
}
int main(void) {
    Machine machine = {STATE_OFF, INIT_SIG};
    LED led = LED_OFF;

    // Initialize the state machine
    dispatch(&machine, &led);

    // Simulate events
    Signal events[] = {BUTTON_PRESS_SIG, TIMEOUT_SIG, BUTTON_RELEASE_SIG, TIMEOUT_SIG};
    for (int i = 0; i < (int)(sizeof(events)/sizeof(events[0])); i++) {
        machine.signal = events[i];
        dispatch(&machine, &led);
        printf("Current State: %s, LED State: %s\n",
               machine.state == STATE_ON ? "ON" : "OFF",
               led == LED_ON ? "ON" : "OFF");
    }

    return 0;
}