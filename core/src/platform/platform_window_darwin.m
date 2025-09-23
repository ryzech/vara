#include "core/platform/platform.h"
#include "core/platform/platform_window.h"

#ifdef VARA_PLATFORM_APPLE

#import <Cocoa/Cocoa.h>

typedef struct vara_window_state {
    NSWindow* window;
    id<NSWindowDelegate> delegate;
} vara_window_state;

@interface VaraWindowDelegate : NSObject <NSWindowDelegate>
@property(nonatomic, assign) vara_window* vara_window_ref;
@end

@implementation VaraWindowDelegate

- (BOOL)windowShouldClose:(NSWindow*)sender {
    return YES;
}

- (void)windowWillClose:(NSNotification*)notification {
}

- (void)windowDidResize:(NSNotification*)notification {
    if (self.vara_window_ref) {
        NSRect frame = [self.vara_window_ref->platform_state->window
            contentRectForFrameRect:[self.vara_window_ref->platform_state->window frame]];
        self.vara_window_ref->width = (u32)frame.size.width;
        self.vara_window_ref->height = (u32)frame.size.height;
    }
}

@end

b8 platform_window_create(vara_window* window, const vara_window_config* config) {
    if (!window || !config) {
        return false;
    }

    window->platform_state = (vara_window_state*)platform_allocate(sizeof(vara_window_state), false);
    if (!window->platform_state) {
        return false;
    }

    window->width = config->width;
    window->height = config->height;
    window->title = config->title;
    window->name = config->name;
    window->pixel_density = 1.0f;

    NSRect contentRect = NSMakeRect(config->position_x, config->position_y, config->width, config->height);

    NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                                  | NSWindowStyleMaskResizable;

    window->platform_state->window = [[NSWindow alloc] initWithContentRect:contentRect
                                                                 styleMask:styleMask
                                                                   backing:NSBackingStoreBuffered
                                                                     defer:NO];

    if (!window->platform_state->window) {
        platform_free(window->platform_state, false);
        return false;
    }

    [window->platform_state->window setTitle:[NSString stringWithUTF8String:config->title]];
    [window->platform_state->window makeKeyAndOrderFront:nil];
    [window->platform_state->window center];

    VaraWindowDelegate* delegate = [[VaraWindowDelegate alloc] init];
    delegate.vara_window_ref = window;
    window->platform_state->delegate = delegate;
    [window->platform_state->window setDelegate:delegate];

    return true;
}

void platform_window_destroy(vara_window* window) {
    if (!window || !window->platform_state) {
        return;
    }

    if (window->platform_state->window) {
        [window->platform_state->window setDelegate:nil];
        [window->platform_state->window close];
        window->platform_state->window = nil;
    }

    if (window->platform_state->delegate) {
        window->platform_state->delegate = nil;
    }

    platform_free(window->platform_state, false);
    window->platform_state = 0;
}

b8 platform_window_poll_events(vara_window* window) {
    if (!window || !window->platform_state || !window->platform_state->window) {
        return false;
    }

    NSEvent* event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                       untilDate:[NSDate distantPast]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES])) {
        [NSApp sendEvent:event];
        [NSApp updateWindows];
    }

    return [window->platform_state->window isVisible];
}

void platform_window_set_title(vara_window* window, const char* title) {
    if (!window || !window->platform_state || !window->platform_state->window || !title) {
        return;
    }

    window->title = title;
    [window->platform_state->window setTitle:[NSString stringWithUTF8String:title]];
}

void platform_window_set_size(vara_window* window, u32 width, u32 height) {
    if (!window || !window->platform_state || !window->platform_state->window) {
        return;
    }

    window->width = width;
    window->height = height;

    NSSize newSize = NSMakeSize(width, height);
    [window->platform_state->window setContentSize:newSize];
}

void platform_window_set_position(vara_window* window, i32 x, i32 y) {
    if (!window || !window->platform_state || !window->platform_state->window) {
        return;
    }

    NSPoint newOrigin = NSMakePoint(x, y);
    [window->platform_state->window setFrameOrigin:newOrigin];
}

void platform_window_get_size(vara_window* window, u32* width, u32* height) {
    if (!window || !window->platform_state || !window->platform_state->window || !width || !height) {
        return;
    }

    NSRect frame = [window->platform_state->window contentRectForFrameRect:[window->platform_state->window frame]];
    *width = (u32)frame.size.width;
    *height = (u32)frame.size.height;
}

void platform_window_get_position(vara_window* window, i32* x, i32* y) {
    if (!window || !window->platform_state || !window->platform_state->window || !x || !y) {
        return;
    }

    NSRect frame = [window->platform_state->window frame];
    *x = (i32)frame.origin.x;
    *y = (i32)frame.origin.y;
}

#endif
