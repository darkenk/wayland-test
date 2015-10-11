set(CMAKE_CXX_FLAGS "")

if (ENABLE_TESTS)
    # GTest
    #add_subdirectory(${CMAKE_SOURCE_DIR}/external/gtest)
    # gtest is built in gmock

    set(GTEST_FOUND 1 PARENT_SCOPE)
    set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gtest/include/ PARENT_SCOPE)
    set(GTEST_LIBRARIES "gtest" PARENT_SCOPE)
    set(GTEST_MAIN_LIBRARIES "gtest_main" PARENT_SCOPE)
    set(GTEST_BOTH_LIBRARIES "gtest;gtest_main" PARENT_SCOPE)
    link_directories(${CMAKE_BINARY_DIR}/external/gmock/gtest/)

    # GMock
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gmock)
    set(GMOCK_FOUND 1 PARENT_SCOPE)
    set(GMOCK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gmock/include/ PARENT_SCOPE)
    set(GMOCK_LIBRARIES "gmock" PARENT_SCOPE)
    set(GMOCK_MAIN_LIBRARIES "gmock_main" PARENT_SCOPE)
    set(GMOCK_BOTH_LIBRARIES "gmock;gmock_main" PARENT_SCOPE)
    link_directories(${CMAKE_BINARY_DIR}/external/gmock/)

endif()
