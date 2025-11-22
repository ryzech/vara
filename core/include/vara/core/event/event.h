#pragma once

#include "vara/core/defines.h"

typedef enum EventCode EventCode;
typedef struct EventData EventData;
enum EventCode {
    EVENT_INVALID = -1,

    EVENT_QUIT = 0,
    EVENT_KEY_PRESSED = 1,
    EVENT_KEY_RELEASED = 2,
    EVENT_WINDOW_RESIZE = 3,

    EVENT_INTERNAL_MAX = 255,
};

struct EventData {
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];
        i32 i32[4];
        u32 u32[4];
        f32 f32[4];
        i16 i16[8];
        u16 u16[8];
        i8 i8[16];
        u8 u8[16];
        const char* string;
    };
};

typedef b8 (*EventCallback)(i16 event_code, void* sender, EventData* event);

b8 event_system_create(void);
void event_system_destroy(void);

b8 event_register(i16 event_code, EventCallback listener);
void event_unregister(i16 event_code, EventCallback listener);
b8 event_fire(i16 event_code, void* sender, EventData* data);