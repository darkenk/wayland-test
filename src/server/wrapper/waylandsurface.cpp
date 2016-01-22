#include "waylandsurface.hpp"

// clang format off
struct wl_surface_interface WaylandSurface::sInterface = {
    WaylandSurface::hookSurfaceDestroy,
    WaylandSurface::hookSurfaceAttach,
    WaylandSurface::surfaceDamage,
    WaylandSurface::hookSurfaceFrame,
    WaylandSurface::surfaceSetOpaqueRegion,
    WaylandSurface::surfaceSetInputRegion,
    WaylandSurface::hookSurfaceCommit,
    WaylandSurface::surfaceSetBufferTransform,
    WaylandSurface::surfaceSetBufferScale
};

// clang format on
