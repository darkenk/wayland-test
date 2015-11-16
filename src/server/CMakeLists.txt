project(wayland_server_test)
aux_source_directory(. SRC)

find_package(Wayland 1.7.0 COMPONENTS Server REQUIRED)
find_package(X11 REQUIRED)

include_directories(SYSTEM
    ${Wayland_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    dk_utils
    ${X11_LIBRARIES}
    xcb
    X11-xcb
    EGL
    GLESv2
    glapi
)
