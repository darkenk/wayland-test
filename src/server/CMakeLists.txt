project(wayland_server_test)
aux_source_directory(wrapper SRC_WRAPPER)
add_library(wrapper STATIC ${SRC_WRAPPER})

aux_source_directory(. SRC)
list(REMOVE_ITEM SRC ./main.cpp)
add_library(dk_wayland_server STATIC ${SRC})
target_link_libraries(dk_wayland_server
    wrapper)

aux_source_directory(backend SRC_BACKEND)

set(SRC "main.cpp;${SRC_BACKEND}")

find_package(Wayland 1.7.0 COMPONENTS Server REQUIRED)
find_package(X11 REQUIRED)

include_directories(
    SYSTEM ${Wayland_INCLUDE_DIRS}
    SYSTEM ${X11_INCLUDE_DIR}
    ${DK_UTILS_INCLUDE_DIRS}
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
    dk_wayland_server
)
