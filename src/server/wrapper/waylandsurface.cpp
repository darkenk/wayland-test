#include "waylandsurface.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

struct wl_surface_interface WaylandSurface::sInterface = {
    .destroy = WaylandSurface::hookSurfaceDestroy,
    .attach = WaylandSurface::hookSurfaceAttach,
    .damage = WaylandSurface::surfaceDamage,
    .frame = WaylandSurface::hookSurfaceFrame,
    .set_opaque_region = WaylandSurface::surfaceSetOpaqueRegion,
    .set_input_region = WaylandSurface::surfaceSetInputRegion,
    .commit = WaylandSurface::hookSurfaceCommit,
    .set_buffer_transform = WaylandSurface::surfaceSetBufferTransform,
    .set_buffer_scale = WaylandSurface::surfaceSetBufferScale
};

#pragma GCC diagnostic pop
