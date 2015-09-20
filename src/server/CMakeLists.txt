project(wayland_server_test)
aux_source_directory(. SRC)

find_package(Wayland COMPONENTS Server Egl REQUIRED)
if (NOT X11_FOUND)
    find_package(X11 REQUIRED)
endif()

include_directories(
    ${Wayland_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    dk_utils
    ${X11_LIBRARIES}
#    EGL
#    GLESv2
)

