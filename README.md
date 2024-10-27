# ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) kvdb-cpp

Orm Key Value Database

**kvdb-cpp** is a simple orm key value database using leveldb as db engine and reflect-cpp for providing orm support.

The following table lists the libraries in use:

| Library                                                          | Version   | License      |
|------------------------------------------------------------------|-----------|--------------|
| [reflect-cpp](https://github.com/getml/reflect-cpp/tree/main)    | >= 0.14.1 | MIT          |
| [leveldb](https://github.com/google/leveldb.git)                 | >= 1.23   | BSD-3-Clause |


## Supported for Types

kvdb-cpp supports the following types:

- `std::string`
- `bool`
- `int`
- `double`

1. structs / classes serialized with [Reflect Cpp](https://github.com/getml/reflect-cpp.git)
2. strings
3. bool
4. int

## Roadmap

- Write Tests with [Doctest](https://github.com/doctest/doctest.git)
- Examples
- Add cmake support
- Docs