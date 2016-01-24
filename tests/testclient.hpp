#ifndef TESTCLIENT_HPP
#define TESTCLIENT_HPP

#include "client/waylandclient.hpp"

namespace client {

class TestClient : public WaylandClient
{
public:
    TestClient(const std::string& socketName = ""): WaylandClient(socketName) {}
    wl_display* getWlDisplay() {
        return getDisplay();
    }
};
}

#endif  // TESTCLIENT_HPP
