if (NOT GTEST_FOUND)
    find_package(GTest REQUIRED)
endif()

if (NOT GMOCK_FOUND)
    find_package(GMock REQUIRED)
endif()

find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src/
    SYSTEM ${GTEST_INCLUDE_DIRS}
    SYSTEM ${GMOCK_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")

function(add_gtest name sources)
    add_executable(${name} ${sources})
    target_link_libraries(${name}
        ${GMOCK_MAIN_LIBRARY}
        ${GMOCK_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    add_test(AllTests ${name})
endfunction()

add_gtest(test1 test1.cpp)
