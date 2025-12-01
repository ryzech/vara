#pragma once

#include "types.h"

static b8 point_in_rect(Vector2 point, Rect rect) {
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x
           && point.y >= rect.position.y && point.y <= rect.position.y + rect.size.y;
}