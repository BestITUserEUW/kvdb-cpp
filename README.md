# ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) kvdb-cpp

[![linux](https://github.com/BestITUserEUW/kvdb-cpp/actions/workflows/linux.yaml/badge.svg)](https://github.com/BestITUserEUW/kvdb-cpp/actions/workflows/linux.yaml)
[![windows](https://github.com/BestITUserEUW/kvdb-cpp/actions/workflows/windows.yaml/badge.svg)](https://github.com/BestITUserEUW/kvdb-cpp/actions/workflows/windows.yaml)

Orm Key Value Database

**kvdb-cpp** is a simple orm key value database using leveldb as db engine and reflect-cpp for providing orm support.

The following table lists the libraries in use:

| Library                                                      | Version   | License      |
|--------------------------------------------------------------|-----------|--------------|
| [reflect-cpp](https://github.com/getml/reflect-cpp.git)      | >= 0.15.0 | MIT          |
| [leveldb](https://github.com/google/leveldb.git)             | >= 1.23   | BSD-3-Clause |


## Supported for Types

kvdb-cpp natively supports the following types:

- `std::string`
- `bool`
- `integral types`
- `floating types`

Anything beyond that will be forwarded to reflect-cpp json serialization and deserialization which supports structs and whole bunch of other stuff check out their: [C++ Standart Support](https://github.com/getml/reflect-cpp?tab=readme-ov-file#support-for-containers)

## Build locally

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -Bbuild -H.
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Only needed for clangd   
```

```bash
cmake --build build -j32
```

## Ready to run Example

```cpp
#include <iostream>
#include <cassert>

#include <oryx/key_value_database.hpp>

auto main() -> int {
    oryx::KeyValueDatabase db{};

    auto status = db.Open("./database.ldb");
    if (!status.ok()) {
        std::cout << "Failed to open db with error: " << status.ToString() << "\n";
        return -1;
    }

    std::string input{"SuperImportantValue"};
    status = db.Put("my_key", input);
    if (!status.ok()) {
        std::cout << "DB put failed" << "\n";
        return -1;
    }

    std::string value;
    status = db.Get("my_key", value);
    if (!status.ok()) {
        std::cout << "DB get failed" << "\n";
        return -1;
    }

    assert(input == value && "This should not happen!");
    return 0;
}
```

## Cmake Integration

Package is being made available through the oryx namespace use this if you have it installed in your system:

```cmake
find_package(kvdb-cpp REQUIRED)

target_link_libraries(your_exe oryx::kvdb-cpp)
```

Or with FetchContent API:

```cmake
include(FetchContent)
FetchContent_Declare(
    kvdb-cpp
    GIT_REPOSITORY https://github.com/BestITUserEUW/kvdb-cpp.git
    GIT_TAG main
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(kvdb-cpp)
target_link_libraries(your_exe kvdb-cpp)
```

Alternatively if you already have leveldb and reflect-cpp linking to your project you can just drop in `include/key_value_database.hpp` into your project.

## Todo

- Replace std::to_string() for floating point types
- Add performance benchmarks