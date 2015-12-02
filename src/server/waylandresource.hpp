#ifndef WAYLANDRESOURCE_HPP
#define WAYLANDRESOURCE_HPP

#include <wayland-server.h>
#include <stdexcept>

template<class T, const wl_interface& INTERFACE, typename S>
class WaylandResource
{
public:
    WaylandResource(T* _this, const S* interface, wl_client* client, uint32_t id) {
        mResource = wl_resource_create(client, &INTERFACE, INTERFACE.version, id);
        if (not mResource) {
            wl_client_post_no_memory(client);
            std::string msg("No memory, can't create ");
            msg.append(INTERFACE.name);
            throw std::logic_error(msg);
            return;
        }
        wl_resource_set_implementation(mResource, interface, _this, hookDestroy);
    }
    virtual ~WaylandResource() {}

protected:
    virtual void destroy() {}

    static T* getThis(wl_resource* resource) {
        return reinterpret_cast<T*>(wl_resource_get_user_data(resource));
    }

    wl_resource* getResource() {
        return mResource;
    }

private:
    wl_resource* mResource;

    static void hookDestroy(wl_resource* resource) {
        auto w = reinterpret_cast<WaylandResource*>(wl_resource_get_user_data(resource));
        w->destroy();
    }

};
#endif // WAYLANDRESOURCE_HPP
