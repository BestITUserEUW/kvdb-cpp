cmake_minimum_required(VERSION 3.11...3.31)

project(kvdbcpp VERSION 0.0.0 LANGUAGES CXX)

option(KVDB_ENABLE_TESTS "Build Tests" ON)
option(KVDB_BUILD_DEPS "Build Dependencies from source" ON)

include(FetchContent)
include(GNUInstallDirs)


if(KVDB_BUILD_DEPS)
    message(STATUS "Building deps from source")

    FetchContent_Declare(
        reflectcpp
        GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
        GIT_TAG v0.15.0
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )

    FetchContent_Declare(
        leveldb
        GIT_REPOSITORY https://github.com/google/leveldb.git
        GIT_TAG 1.23
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )

    FetchContent_MakeAvailable(reflectcpp leveldb)
endif()

find_package(reflectcpp)
find_package(leveldb)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    INTERFACE 
        reflectcpp
        leveldb
)

if(KVDB_ENABLE_TESTS)
    set(test_exe ${PROJECT_NAME}_test)
    add_executable(${test_exe} test/main.cpp test/read_write.cpp)
    target_link_libraries(${test_exe} PRIVATE ${PROJECT_NAME})
endif()


target_include_directories(${PROJECT_NAME}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>" 
)

