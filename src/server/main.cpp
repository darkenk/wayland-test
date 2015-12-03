#include "waylandserver.hpp"
#include "backend/x11backend.hpp"

using namespace std;

int main() {
    WaylandServer(make_unique<X11Backend>(320, 480)).run();
    return 0;
}
