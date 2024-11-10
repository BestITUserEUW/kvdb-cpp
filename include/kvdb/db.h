#pragma once

#include <string>
#include <optional>
#include <type_traits>
#include <charconv>

#include <leveldb/db.h>
#include <rfl/Result.hpp>
#include <rfl/json/write.hpp>
#include <rfl/json/read.hpp>

namespace st {
namespace internal {

template <typename T>
constexpr auto FromChars(std::string_view s) -> std::optional<T> {
    T val;
    if (std::from_chars(s.data(), s.data() + s.size(), val).ec == std::errc{}) {
        return val;
    } else {
        return std::nullopt;
    }
}

template <>
constexpr auto FromChars<bool>(std::string_view s) -> std::optional<bool> {
    if(auto val = FromChars<uint8_t>(s); val.has_value())
        return static_cast<bool>(val.value());
    else
        return std::nullopt;
}

template <typename T>
auto Read(const std::string& val) -> std::optional<T> {
    using _T = std::remove_cvref_t<T>;

    if constexpr (std::is_same<_T, std::string>()) {
        return val;
    } else if constexpr (std::is_same<_T, bool>()) {
        return FromChars<bool>(val);
    } else if constexpr (std::is_floating_point<_T>()) {
        return FromChars<T>(val);
    } else if constexpr (std::is_integral<_T>()) {
        return FromChars<T>(val);
    } else {
        if (auto result = rfl::json::read<T>(val); result) {
            return result.value();
        } else {
            return std::nullopt;
        }
    }
}

template <typename T>
auto Write(const T& obj) -> std::string {
    using _T = std::remove_cvref_t<T>;

    if constexpr (std::is_same<_T, std::string>()) {
        return obj;
    } else if constexpr (std::is_same<_T, bool>()) {
        return std::to_string(static_cast<uint8_t>(obj));
    } else if constexpr (std::is_floating_point<_T>()) {
        return std::to_string(obj);
    } else if constexpr (std::is_integral<_T>()) {
        return std::to_string(obj);
    } else {
        return rfl::json::write(obj);
    }
}

}  // namespace internal

class DB {
public:
    explicit DB()
        : db_(nullptr) {}

    leveldb::Status Open(const std::string& name, const leveldb::Options& opts = DefaultOptions()) {
        if (db_ != nullptr) {
            Close();
        }
        return leveldb::DB::Open(opts, name, &db_);
    }

    void Close() {
        if (db_) {
            delete db_;
            db_ = nullptr;
        }
    }

    template <typename T>
    leveldb::Status Get(const leveldb::Slice& key, T& val, const leveldb::ReadOptions& opts = DefaultReadOptions()) {
        std::string result;
        leveldb::Status status = db_->Get(opts, key, &result);
        if (!status.ok()) {
            return status;
        }

        std::optional<T> parsed = internal::Read<T>(result);
        if (!parsed) {
            return leveldb::Status::IOError("Parse failed");
        }

        val = std::move(parsed.value());
        return status;
    }

    template <typename T>
    leveldb::Status Put(const leveldb::Slice& key,
                        const T& obj,
                        const leveldb::WriteOptions& opts = DefaultWriteOptions()) {
        return db_->Put(opts, key, internal::Write(obj));
    }

    leveldb::Status Delete(const leveldb::Slice& key, const leveldb::WriteOptions& opts = DefaultWriteOptions()) {
        return db_->Delete(opts, key);
    }

    static auto DefaultOptions() -> leveldb::Options {
        leveldb::Options opts;
        opts.create_if_missing = true;
        opts.reuse_logs = true;
        return opts;
    }
    static auto DefaultWriteOptions() -> leveldb::WriteOptions {
        leveldb::WriteOptions opts;
        opts.sync = true;
        return opts;
    }
    static auto DefaultReadOptions() -> leveldb::ReadOptions { return leveldb::ReadOptions(); }

    ~DB() { Close(); }

private:
    leveldb::DB* db_;
};
}  // namespace st