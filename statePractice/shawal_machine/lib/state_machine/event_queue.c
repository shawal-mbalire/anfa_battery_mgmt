#include "event_queue.h"
#include "interrupt.h"

// Global event queue instance
EventQueue g_eventQueue = {.head = 0, .tail = 0, .is_empty = true};

// Function to push an event into the queue
void PushEvent(Event event) {
    disable_interrupts(); // --- Start of critical section ---

    int next_head = (g_eventQueue.head + 1) % EVENT_QUEUE_SIZE;

    // Overwrite oldest event if the queue is full
    if (next_head == g_eventQueue.tail && !g_eventQueue.is_empty) {
        // The queue is full. Overwrite the oldest event at the tail.
        g_eventQueue.tail = (g_eventQueue.tail + 1) % EVENT_QUEUE_SIZE;
    }

    g_eventQueue.queue[g_eventQueue.head] = event;
    g_eventQueue.head = next_head;
    g_eventQueue.is_empty = false;

    enable_interrupts(); // --- End of critical section ---
}

// Function to pop an event from the queue
Event PopEvent(void) {
    disable_interrupts(); // --- Start of critical section ---

    if (g_eventQueue.is_empty) {
        enable_interrupts(); // --- End of critical section ---
        return EVENT_MAX; // Return "no event"
    }

    Event event = g_eventQueue.queue[g_eventQueue.tail];
    g_eventQueue.tail = (g_eventQueue.tail + 1) % EVENT_QUEUE_SIZE;

    if (g_eventQueue.head == g_eventQueue.tail) {
        g_eventQueue.is_empty = true;
    }

    enable_interrupts(); // --- End of critical section ---
    return event;
}
