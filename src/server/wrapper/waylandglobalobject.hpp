#ifndef WAYLANDGLOBALOBJECT_HPP
#define WAYLANDGLOBALOBJECT_HPP

#include "dk_utils/make_unique.hpp"
#include <memory>
#include <stdexcept>
#include <wayland-server.h>

class FactoryWaylandGlobalObject
{
public:
    FactoryWaylandGlobalObject(wl_display* display) : mDisplay(display) {}

    template <class T, typename... Args> std::unique_ptr<T> create(Args&&... args) {
        auto p = std::make_unique<T>(std::forward<Args>(args)...);
        if (wl_global_create(mDisplay, &T::interface, T::interface.version, p.get(), T::hookBind)) {
            return p;
        }
        std::string msg("We are doomed, cannot create ");
        msg.append(T::interface.name);
        throw std::logic_error(msg);
    }

private:
    wl_display* mDisplay;
};

template <class T, const wl_interface& INTERFACE, typename S> class WaylandGlobalObject
{
public:
    WaylandGlobalObject(const S* implementedInterface) {
        sInterface = implementedInterface;
    }
    virtual ~WaylandGlobalObject() {}

protected:
    virtual void clientDisconnects(wl_client* /*client*/) {}
    virtual void bind(wl_client* /*client*/, wl_resource* /*resource*/) {}

    static T* getThis(wl_resource* resource) {
        return reinterpret_cast<T*>(wl_resource_get_user_data(resource));
    }

private:
    friend FactoryWaylandGlobalObject;
    static constexpr const wl_interface& interface = INTERFACE;
    static const S* sInterface;

    static void hookBind(wl_client* client, void* data, uint32_t version, uint32_t id) {
        wl_resource* resource = wl_resource_create(client, &INTERFACE, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        WaylandGlobalObject* object = reinterpret_cast<WaylandGlobalObject*>(data);
        wl_resource_set_implementation(resource, object->sInterface, data, hookClientDisconects);
        object->bind(client, resource);
    }

    static void hookClientDisconects(wl_resource* resource) {
        WaylandGlobalObject* o =
            reinterpret_cast<WaylandGlobalObject*>(wl_resource_get_user_data(resource));
        o->clientDisconnects(wl_resource_get_client(resource));
    }
};

template <class T, const wl_interface& INTERFACE, typename S>
const S* WaylandGlobalObject<T, INTERFACE, S>::sInterface = nullptr;

#endif  // WAYLANDGLOBALOBJECT_HPP
