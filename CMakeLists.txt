cmake_minimum_required(VERSION 3.24)

project(kvdb-cpp VERSION 0.1.0 LANGUAGES CXX)

option(ORYX_KVDB_ENABLE_TESTS "Build Tests" ON)
option(ORYX_KVDB_BUILD_DEPS "Build Dependencies from source" ON)
option(ORYX_KVDB_INSTALL "Install the project" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

include(FetchContent)

if(ORYX_KVDB_BUILD_DEPS)
    message(STATUS "Building deps from source")

    FetchContent_Declare(
        reflectcpp
        GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
        GIT_TAG 2a0fdeabe35bdab0e07e05d4f869ca88bd6d2b8c
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        leveldb
        GIT_REPOSITORY https://github.com/google/leveldb.git
        GIT_TAG ac691084fdc5546421a55b25e7653d450e5a25fb
        OVERRIDE_FIND_PACKAGE
    )

    option(LEVELDB_BUILD_BENCHMARKS "Build LevelDB benchmarks" OFF)
    option(LEVELDB_BUILD_TESTS "Build LevelDB tests" OFF)
    option(LEVELDB_INSTALL "Install LevelDB" ${ORYX_KVDB_INSTALL})
    option(REFLECTCPP_INSTALL "Install ReflectCpp" ${ORYX_KVDB_INSTALL})
    FetchContent_MakeAvailable(reflectcpp leveldb)
    
    add_library(reflectcpp::reflectcpp ALIAS reflectcpp)
    add_library(leveldb::leveldb ALIAS leveldb)
else()
    find_package(reflectcpp CONFIG REQUIRED)
    find_package(leveldb CONFIG REQUIRED)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    INTERFACE 
        reflectcpp::reflectcpp
        leveldb::leveldb
)

if(ORYX_KVDB_ENABLE_TESTS)
    set(test_exe ${PROJECT_NAME}_tests)
    add_executable(${test_exe} 
        tests/main.cpp 
        tests/read_write.cpp
    )
    target_link_libraries(${test_exe} PRIVATE ${PROJECT_NAME})
endif()

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} INTERFACE $<$<CONFIG:Debug>:/W4>)
else()
    target_compile_options(${PROJECT_NAME} INTERFACE $<$<CONFIG:Debug>:-Wall -Wextra>)
endif()


if(ORYX_KVDB_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        cmake/${PROJECT_NAME}-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    file(GLOB_RECURSE KVDB_CPP_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/include/*")

    target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET kvdb_cpp_headers
        TYPE HEADERS
        BASE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        FILES ${KVDB_CPP_HEADERS}
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-exports
        FILE_SET kvdb_cpp_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT ${PROJECT_NAME}-exports
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        NAMESPACE oryx::
    )
endif()