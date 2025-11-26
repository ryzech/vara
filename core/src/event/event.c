#include "vara/core/event/event.h"
#include "vara/core/platform/platform.h"

#define MAX_LISTENERS 32

typedef struct EventState EventState;

// We should implement some kind of ArrayList/Dynamic Array instead.
struct EventState {
    EventCallback listeners[EVENT_INTERNAL_MAX][MAX_LISTENERS];
    u32 listener_count[EVENT_INTERNAL_MAX];
};

static EventState* event_state;

b8 event_system_create(void) {
    event_state = platform_allocate(sizeof(EventState));
    platform_zero_memory(event_state, sizeof(EventState));

    return true;
}

void event_system_destroy(void) {
    if (event_state) {
        platform_free(event_state);
    }
}

b8 event_register(i16 event_code, EventCallback listener) {
    if (!event_state || event_code <= EVENT_INVALID) {
        return false;
    }

    u32* index = &event_state->listener_count[event_code];
    if (*index >= MAX_LISTENERS) {
        return false;
    }

    event_state->listeners[event_code][(*index)++] = listener;
    return true;
}

void event_unregister(i16 event_code, EventCallback listener) {
    if (!event_state || event_code <= EVENT_INVALID) {
        return;
    }

    u32* index = &event_state->listener_count[event_code];
    EventCallback* listeners = event_state->listeners[event_code];

    for (u32 i = 0; i < *index; i++) {
        if (listeners[i] == listener) {
            listeners[i] = listeners[*index - 1];
            (*index)--;
            return;
        }
    }
}

b8 event_fire(i16 event_code, void* sender, EventData* data) {
    if (!event_state || event_code <= EVENT_INVALID) {
        return false;
    }

    u32 count = event_state->listener_count[event_code];
    EventCallback* listeners = event_state->listeners[event_code];

    for (u32 i = 0; i < count; i++) {
        if (listeners[i](event_code, sender, data)) {
            // The event was handled, further events should not receive.
            return true;
        }
    }

    // Event was not handled, return once all listeners were iterated.
    return false;
}
