#ifndef TESTCLIENT_HPP
#define TESTCLIENT_HPP

#include "client/waylandclient.hpp"

namespace client {

class TestClient : public WaylandClient
{
public:
    TestClient(const std::string& socketName = ""): WaylandClient(socketName) {}
    virtual ~TestClient();
    wl_display* getWlDisplay();
    wl_surface* createSurface();
    void commitChangesToServer();
};
}

#endif  // TESTCLIENT_HPP
