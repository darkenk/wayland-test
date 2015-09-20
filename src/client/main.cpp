#include <wayland-client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

wl_compositor* compositor = nullptr;
wl_shell* shell = nullptr;

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
    const char *interface, uint32_t version)
{
    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, wl_shell_interface.name) == 0) {
        shell = (wl_shell*)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    printf("Got a registry losing event for %d\n", id);
}

static const wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};

int main(int argc, char *argv[])
{
    wl_display *display = wl_display_connect(nullptr);
    if (not display) {
        printf("Error opening display");
        exit(1);
    }

    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, nullptr);

    wl_display_dispatch(display);
    wl_display_roundtrip(display); // wait synchronously for event

    if (not compositor) {
        printf("Can't find compositor\n");
        exit(1);
    } else {
        printf("Found compositor\n");
    }

    wl_surface* surface = wl_compositor_create_surface(compositor);
    if (not surface) {
        printf("Can't create surface\n");
        exit(1);
    } else {
        printf("Created surface\n");
    }

    wl_shell_surface* shell_surface = wl_shell_get_shell_surface(shell, surface);
    if (not shell_surface) {
        printf("Can't create shell surface\n");
        exit(1);
    } else {
        printf("Created shell surface\n");
    }
    wl_shell_surface_set_toplevel(shell_surface);

    wl_display_disconnect(display);

    return 0;
}
