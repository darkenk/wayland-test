#include "waylandseat.hpp"

// clang-format off
const struct wl_seat_interface WaylandSeat::sInterface = {
    hookGetPointer,
    hookGetKeyboard,
    hookGetTouch
};
// clang-format on
