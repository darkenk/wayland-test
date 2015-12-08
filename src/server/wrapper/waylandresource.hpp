#ifndef WAYLANDRESOURCE_HPP
#define WAYLANDRESOURCE_HPP

#include <stdexcept>
#include <wayland-server.h>

class WaylandResourceFactory
{
public:
    template <class T, typename... Args>
    static T* create(wl_client* client, uint32_t id, Args&&... args) {
        auto p = new T(std::forward<Args>(args)...);
        if (auto r = wl_resource_create(client, &T::interface, T::interface.version, id)) {
            p->setResource(r);
            wl_resource_set_implementation(p->getResource(), T::sImplInterface, p, T::hookDestroy);
            return p;
        }
        std::string msg("We are doomed, cannot create ");
        msg.append(T::interface.name);
        throw std::logic_error(msg);
    }
};

template <class T, const wl_interface& INTERFACE, typename S> class WaylandResource
{
public:
    WaylandResource(const S* implementedInterface) : mResource(nullptr) {
        sImplInterface = implementedInterface;
    }
    virtual ~WaylandResource() {
        // TODO: There should better way to fix sigsegv in hookDestroy
        wl_resource_set_destructor(mResource, nullptr);
    }

protected:
    virtual void destroy() {}

    static T* getThis(wl_resource* resource) {
        return reinterpret_cast<T*>(wl_resource_get_user_data(resource));
    }

    wl_resource* getResource() {
        return mResource;
    }

private:
    friend WaylandResourceFactory;
    static constexpr const wl_interface& interface = INTERFACE;
    static const S* sImplInterface;
    wl_resource* mResource;

    void setResource(wl_resource* resource) {
        mResource = resource;
    }

    static void hookDestroy(wl_resource* resource) {
        auto w = reinterpret_cast<WaylandResource*>(wl_resource_get_user_data(resource));
        w->destroy();
    }
};

template <class T, const wl_interface& INTERFACE, typename S>
const S* WaylandResource<T, INTERFACE, S>::sImplInterface = nullptr;

#endif  // WAYLANDRESOURCE_HPP
