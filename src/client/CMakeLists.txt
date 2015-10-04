project(shm_example)

find_package(Wayland 1.7.0 COMPONENTS Client Egl REQUIRED)
find_package(EGL REQUIRED)

include_directories(
    ${Wayland_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    dk_utils
)

set(PROJECT_NAME opengl_example)

add_executable(${PROJECT_NAME} test2.cpp)

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    dk_utils
    ${EGL_LIBRARIES}
    GLESv2
    glapi
)
