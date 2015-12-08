#ifndef SHELL_HPP
#define SHELL_HPP

#include "shellsurface.hpp"
#include "surface.hpp"
#include "wrapper/waylandshell.hpp"

class Shell : public WaylandShell
{
public:
    Shell() {}

protected:
    virtual void getShellSurface(wl_client* client, wl_resource* /*resource*/, uint32_t id,
                                 wl_resource* surface) {
        auto p = reinterpret_cast<Surface*>(wl_resource_get_user_data(surface));
        p->setShellSurface(std::unique_ptr<ShellSurface>(
            WaylandResourceFactory::create<ShellSurface>(client, id)));
    }
};

#endif  // SHELL_HPP
