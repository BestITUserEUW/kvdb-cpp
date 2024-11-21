# ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) kvdb-cpp

Orm Key Value Database

**kvdb-cpp** is a simple orm key value database using leveldb as db engine and reflect-cpp for providing orm support.

The following table lists the libraries in use:

| Library                                                      | Version   | License      |
|--------------------------------------------------------------|-----------|--------------|
| [reflect-cpp](https://github.com/getml/reflect-cpp.git)      | >= 0.15.0 | MIT          |
| [leveldb](https://github.com/google/leveldb.git)             | >= 1.23   | BSD-3-Clause |


## Supported for Types

kvdb-cpp supports the following types:

- `std::string`
- `bool`
- `int`
- `double`
- `structs / classes`

## Roadmap

- Examples
- Add cmake support
- Docs