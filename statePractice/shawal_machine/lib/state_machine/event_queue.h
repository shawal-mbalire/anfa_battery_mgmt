#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stdbool.h>

// 1. Define events
typedef enum {
    EVENT_BUTTON_PRESS,
    EVENT_TIMEOUT,
    EVENT_DOOR_OPEN,
    EVENT_QUEUE_FULL,
    EVENT_MAX // Represents no event
} Event;

#define EVENT_QUEUE_SIZE 10

// Define the queue structure
typedef struct {
    Event queue[EVENT_QUEUE_SIZE];
    int head;
    int tail;
    bool is_empty;
} EventQueue;

// Function prototypes
void PushEvent(Event event);
Event PopEvent(void);

#endif // EVENT_QUEUE_H
