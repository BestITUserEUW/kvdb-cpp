#pragma once

#include <string>
#include <optional>
#include <type_traits>
#include <charconv>
#include <memory>

#include <leveldb/db.h>
#include <rfl/Result.hpp>
#include <rfl/json/write.hpp>
#include <rfl/json/read.hpp>

namespace oryx {
namespace detail {

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
    if (auto val = FromChars<uint8_t>(s); val.has_value())
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

}  // namespace detail

class KeyValueDatabase {
public:
    KeyValueDatabase() = default;

    auto Open(const std::string& name, const leveldb::Options& opts = DefaultOptions()) -> leveldb::Status {
        Close();

        leveldb::DB* db;
        const auto status = leveldb::DB::Open(opts, name, &db);
        if (status.ok()) {
            handle_ = std::unique_ptr<leveldb::DB>(db);
        }
        return status;
    }

    void Close() { handle_.reset(); }

    template <typename T>
    auto Get(const leveldb::Slice& key, T& val, const leveldb::ReadOptions& opts = DefaultReadOptions())
        -> leveldb::Status {
        std::string result;
        leveldb::Status status = handle_->Get(opts, key, &result);
        if (!status.ok()) {
            return status;
        }

        std::optional<T> parsed = detail::Read<T>(result);
        if (!parsed) {
            return leveldb::Status::IOError("Parse failed");
        }

        val = std::move(parsed.value());
        return status;
    }

    template <typename T>
    auto Put(const leveldb::Slice& key, const T& obj, const leveldb::WriteOptions& opts = DefaultWriteOptions())
        -> leveldb::Status {
        return handle_->Put(opts, key, detail::Write(obj));
    }

    auto Delete(const leveldb::Slice& key, const leveldb::WriteOptions& opts = DefaultWriteOptions())
        -> leveldb::Status {
        return handle_->Delete(opts, key);
    }

    [[nodiscard]] auto IsOpen() const -> bool { return static_cast<bool>(handle_); }
    [[nodiscard]] auto handle() const -> leveldb::DB& { return *handle_; }

    static auto DefaultOptions() -> leveldb::Options {
        leveldb::Options opts{};
        opts.create_if_missing = true;
        opts.reuse_logs = true;
        return opts;
    }

    static auto DefaultWriteOptions() -> leveldb::WriteOptions {
        leveldb::WriteOptions opts{};
        opts.sync = true;
        return opts;
    }

    static auto DefaultReadOptions() -> leveldb::ReadOptions { return {}; }

private:
    std::unique_ptr<leveldb::DB> handle_{};
};

}  // namespace oryx