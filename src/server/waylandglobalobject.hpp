#ifndef WAYLANDGLOBALOBJECT_HPP
#define WAYLANDGLOBALOBJECT_HPP

#include <wayland-server.h>
#include <stdexcept>

template<class T, const wl_interface& INTERFACE, typename S>
class WaylandGlobalObject
{
public:
    WaylandGlobalObject(wl_display* display) {
        if (not wl_global_create(display, &INTERFACE, INTERFACE.version, this, hookBind)) {
            std::string msg("We are doomed, cannot create ");
            msg.append(INTERFACE.name);
            throw std::logic_error(msg);
        }
    }

    virtual ~WaylandGlobalObject() {}

protected:
    virtual void clientDisconnects(wl_client* /*client*/) {}

    static T* getThis(wl_resource* resource) {
        return reinterpret_cast<T*>(wl_resource_get_user_data(resource));
    }

private:
    friend T;

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        wl_resource* resource = wl_resource_create(client, &INTERFACE, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, T::getInterface(), this, hookClientDisconects);
    }

    static void hookBind(wl_client* client, void* data, uint32_t version, uint32_t id) {
        T* object = reinterpret_cast<T*>(data);
        object->bind(client, version, id);
    }

    static void hookClientDisconects(wl_resource* resource) {
        getThis(resource)->clientDisconnects(wl_resource_get_client(resource));
    }

};

#endif // WAYLANDGLOBALOBJECT_HPP
