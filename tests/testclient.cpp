#include "testclient.hpp"

using namespace client;

TestClient::~TestClient()
{
}

wl_display* TestClient::getWlDisplay() {
    return getDisplay();
}

wl_surface* TestClient::createSurface()
{
    return wl_compositor_create_surface(getCompositor());
}

void TestClient::commitChangesToServer()
{
    wl_display_flush(getDisplay());
}
