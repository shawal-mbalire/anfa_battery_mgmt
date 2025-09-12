# Event-Driven State Machine for Embedded Systems in C

This project provides a robust, non-blocking, event-driven state machine implementation in C, specifically designed and optimized for resource-constrained embedded systems. It avoids common pitfalls like large `switch` statements and blocking calls, resulting in a modular, maintainable, and efficient architecture.

## Core Concepts and Optimizations

This implementation employs several key concepts crucial for modern embedded systems development:

### 1. Event-Driven Architecture
Instead of continuously polling for changes, the system is designed to react to asynchronous **events**. This is a more efficient paradigm, as the CPU can remain in a low-power state until an event (like a button press, timer timeout, or sensor reading) requires processing.

### 2. State Machine with Function Pointers
Each state's logic is encapsulated in its own set of functions. The current state is tracked by a function pointer (`state_handler_t`).

- **Advantages**:
    - **Modularity**: Eliminates monolithic `switch` statements. Each state's logic is self-contained.
    - **Maintainability**: Adding or modifying a state is as simple as defining a new set of action functions and updating the transitions.
    - **Efficiency**: A function pointer call is a direct, fast operation.

### 3. State-Action Model (Entry, Main, Exit)
Each state can have three distinct actions, providing a clean structure:
- `entry_action`: Executed once upon entering a state. Ideal for setup tasks (e.g., starting a timer, enabling a peripheral).
- `main_action`: Executed repeatedly while the system remains in the state. Suitable for ongoing tasks (e.g., blinking an LED, monitoring a sensor).
- `exit_action`: Executed once upon exiting a state. Perfect for cleanup (e.g., stopping a timer, disabling a peripheral).

This model cleanly separates initialization, continuous work, and cleanup logic for each state.

### 4. Non-Blocking Design via Event Queue
A critical feature of this design is its non-blocking nature, achieved with a **circular event queue** (`event_queue.c`).

- **How it works**: When an event occurs (e.g., inside an Interrupt Service Routine - ISR), it is quickly placed into a fixed-size queue. The main loop later retrieves events from this queue and processes them one by one.
- **Benefit**: This decouples event detection from event processing. The ISR or event-generating task can run very quickly without getting blocked by the potentially longer state transition logic. This ensures the system remains responsive to new events.

### 5. Resource and Memory Optimization
- **No Dynamic Allocation**: The code uses **zero dynamic memory allocation** (`malloc`/`free`). All data structures, including the event queue, are statically allocated with a fixed size. This is crucial for the reliability and predictability of embedded systems, as it eliminates the risk of memory leaks or heap fragmentation.
- **`const` State Definitions**: State definitions (`IDLE_STATE`, `ACTIVE_STATE`) are declared as `const`. This allows the compiler to place them in read-only memory (Flash/ROM) instead of RAM, saving precious RAM resources.

## Audit and Areas for Improvement

While this implementation is robust, a production-grade system should consider the following:

### 1. Interrupt Safety (Concurrency)
The `push_event()` function is not currently "interrupt-safe." If an interrupt occurs and calls `push_event()` while the main loop is in the middle of modifying the queue, the queue's state could be corrupted.

- **Solution**: The critical section within `push_event()` where the queue's `head` pointer and flags are modified must be atomic. This is typically achieved by briefly disabling interrupts before modifying the queue and re-enabling them immediately after.

```c
// Example of making push_event interrupt-safe
void push_event(event_t event) {
    // 1. Disable interrupts
    disable_interrupts();

    // Critical section: modify the queue
    int next_head = (event_queue.head + 1) % EVENT_QUEUE_SIZE;
    if (next_head == event_queue.tail && !event_queue.is_empty) {
        // Handle queue full error...
    } else {
        event_queue.queue[event_queue.head] = event;
        event_queue.head = next_head;
        event_queue.is_empty = false;
    }

    // 2. Re-enable interrupts
    enable_interrupts();
}
```

### 2. Robust Error Handling
- **Event Queue Full**: The current implementation prints a message if the queue is full. In a real system, this should be handled more gracefully. Depending on the application, you might drop the oldest event, drop the newest event, or transition to an error state.
- **Unhandled Events**: If an event occurs that the current state doesn't handle, the system currently ignores it. For safety-critical applications, it may be necessary to transition to a dedicated "Error" or "Safe" state.

### 3. Non-Blocking Logging
The `logger.c` uses `printf`, which is a blocking and resource-intensive function. A true non-blocking logger for an embedded system would write log messages to a separate circular buffer, and a low-priority task or a UART "transmit empty" interrupt would handle the actual transmission of data.

### 4. Power Management
For battery-powered devices, the main loop should put the CPU into a low-power sleep mode when there are no events to process. The CPU would then be woken up by an interrupt (which would push an event to the queue).

```c
// Conceptual low-power main loop
void main_loop(void) {
    while(1) {
        event_t event = pop_event();
        if (event != EVENT_MAX) {
            // Process event...
        } else {
            // No events to process, enter sleep mode
            enter_sleep_mode(); // Woken by next interrupt
        }
    }
}
```
