#include "bms_events.h"

// Simple event queue for BMS events (similar to the library's but for extended events)
#define BMS_EVENT_QUEUE_SIZE 10
static volatile BmsEvent bms_event_queue[BMS_EVENT_QUEUE_SIZE];
static volatile int bms_head = 0;
static volatile int bms_tail = 0;

// Function to post BMS-specific events
// In a real system, this would be called from ISRs
void PostBmsEvent(BmsEvent event) {
    int next_tail = (bms_tail + 1) % BMS_EVENT_QUEUE_SIZE;
    if (next_tail != bms_head) {
        bms_event_queue[bms_tail] = event;
        bms_tail = next_tail;
    }
}

// Function to get BMS events
Event GetBmsEvent(void) {
    if (bms_head == bms_tail) {
        return EVENT_MAX; // No event
    }
    BmsEvent bms_event = bms_event_queue[bms_head];
    bms_head = (bms_head + 1) % BMS_EVENT_QUEUE_SIZE;
    
    // Convert BMS events to base library events when possible
    // or return a generic event for BMS-specific events
    switch (bms_event) {
        case POWER_SOURCE_CONNECTED:
        case SOLAR_POWER_CONNECTED:
        case POWER_SOURCE_DISCONNECTED:
        case LIGHT_SWITCH_TOGGLED:
        case BATTERY_TEMP_FAULT:
        case BMS_INTERRUPT:
        case USB_FAULT:
        case SYSTEM_FAULT:
        case GO_TO_SLEEP:
        case WAKE_UP:
            // These are handled as custom events beyond the library's scope
            return (Event)bms_event;
        default:
            return EVENT_MAX;
    }
}