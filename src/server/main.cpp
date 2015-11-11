#include "waylandserver.hpp"
#include "x11backend.hpp"

using namespace std;

int main() {
    WaylandServer(std::make_unique<X11Backend>(320, 480)).run();
    return 0;
}
