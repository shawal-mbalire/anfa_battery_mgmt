#ifndef BMS_EVENTS_H
#define BMS_EVENTS_H

#include "../../lib/state_machine/event_queue.h"

// Extend the base events with battery management specific events
typedef enum {
    // Base events from library
    BMS_NO_EVENT = EVENT_MAX + 1,
    
    // Power management events
    POWER_SOURCE_CONNECTED,
    SOLAR_POWER_CONNECTED,
    POWER_SOURCE_DISCONNECTED,
    CHARGE_COMPLETE,
    
    // User interface events
    LIGHT_SWITCH_TOGGLED,
    
    // Fault events
    BATTERY_TEMP_FAULT,
    BMS_INTERRUPT,
    USB_FAULT,
    SYSTEM_FAULT,
    FAULT_RS485_12V,
    BMS_COMM_FAULT,
    CHARGER_COMM_FAULT,
    BATTERY_CELL_UVLO,
    BATTERY_CELL_OVLO,
    HIGH_CURRENT_DETECTED,
    
    // Power management
    GO_TO_SLEEP,
    WAKE_UP
} BmsEvent;

// Function to post BMS-specific events
void PostBmsEvent(BmsEvent event);

// Function to get BMS events (converts to base Event type)
Event GetBmsEvent(void);

#endif // BMS_EVENTS_H