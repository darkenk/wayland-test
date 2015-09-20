project(wayland_test_client)

find_package(Wayland COMPONENTS Client Egl REQUIRED)

include_directories(
    ${Wayland_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} test2.cpp)

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    EGL
    GLESv2
    glapi
)
