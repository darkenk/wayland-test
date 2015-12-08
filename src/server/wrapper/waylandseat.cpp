#include "waylandseat.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

const struct wl_seat_interface WaylandSeat::sInterface = {
    .get_pointer = hookGetPointer, .get_keyboard = hookGetKeyboard, .get_touch = hookGetTouch};

#pragma GCC diagnostic pop
