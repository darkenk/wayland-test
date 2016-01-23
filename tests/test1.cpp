#include <gtest/gtest.h>
#include <wayland-client.h>
#include "server/waylandserver.hpp"
#include "nullbackend.hpp"
#include "dk_utils/make_unique.hpp"

using namespace std;

TEST(WaylandServer, terminate_server)
{
    WaylandServer wl(make_unique<NullBackend>(320, 480, 1));
    wl_display* mDisplay = wl_display_connect(nullptr);
    EXPECT_NE(nullptr, mDisplay);
}
