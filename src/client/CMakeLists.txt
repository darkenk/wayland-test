project(wayland_test_client)

find_package(Wayland 1.7.0 COMPONENTS Client Egl REQUIRED)
find_package(EGL REQUIRED)
#find_package(GLESv2 REQUIRED)

include_directories(
    ${Wayland_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} test2.cpp)

target_link_libraries(${PROJECT_NAME}
    ${Wayland_LIBRARIES}
    ${EGL_LIBRARIES}
    GLESv2
    glapi
)
