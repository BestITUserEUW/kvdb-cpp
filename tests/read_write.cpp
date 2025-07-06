#include "doctest.hpp"

#include <filesystem>

#include <oryx/key_value_database.hpp>

namespace fs = std::filesystem;
using namespace oryx;

struct Dummy {
    std::string prop0;
    int prop1;
    bool prop2;
};

struct TempDbFile {
    TempDbFile()
        : file(fs::temp_directory_path()) {
        file.append("tmp.db");
    }

    ~TempDbFile() { std::filesystem::remove_all(file); }

    auto ToString() { return file.string(); }

    fs::path file;
};

static constexpr char kDummyString[] = R"({"prop0":"hello","prop1":5,"prop2":false})";
static constexpr char kCorruptDummyString[] = R"({"prop"hello","prop5":5,"prop6":false})";

TEST_CASE("Reading string into struct") {
    Dummy value = detail::Read<Dummy>(kDummyString).value();
    CHECK(value.prop0 == "hello");
    CHECK(value.prop1 == 5);
    CHECK(value.prop2 == false);
}

TEST_CASE("Reading corrupt string into struct") {
    auto result = detail::Read<Dummy>(kCorruptDummyString);
    REQUIRE_FALSE(result);
}

TEST_CASE("Writing struct to string") {
    REQUIRE(detail::Write<Dummy>(Dummy("hello", 5, false)) == std::string(kDummyString));
}

TEST_CASE("Reading supported types") {
    CHECK_EQ(detail::Read<std::string>("hello world1232!*2`-.").value(), "hello world1232!*2`-.");
    CHECK(detail::Read<bool>("0").value() == false);
    CHECK(detail::Read<bool>("1").value() == true);
    CHECK(detail::Read<double>("1.256").value() == 1.256);
    CHECK(detail::Read<double>("-1.256").value() == -1.256);
    CHECK(detail::Read<float>("1.256").value() == 1.256f);
    CHECK(detail::Read<float>("-1.256").value() == -1.256f);
    CHECK(detail::Read<uint64_t>("5").value() == static_cast<uint64_t>(5));
    CHECK(detail::Read<int64_t>("5").value() == static_cast<int64_t>(5));
    CHECK(detail::Read<int64_t>("-5").value() == static_cast<int64_t>(-5));
    CHECK(detail::Read<int>("5").value() == 5);
    CHECK(detail::Read<int>("-5").value() == -5);
}

TEST_CASE("Writing supported types") {
    CHECK_EQ(detail::Write<std::string>("hello world1232!*2`-."), "hello world1232!*2`-.");
    CHECK_EQ(detail::Write<std::string_view>("hello world1232!*2`-."), "hello world1232!*2`-.");
    CHECK_EQ(detail::Write<bool>(false), "0");
    CHECK_EQ(detail::Write<bool>(true), "1");
    CHECK_EQ(detail::Write<double>(1.256), "1.256000");  // std::to_string appends 0 at the end for some reason
    CHECK_EQ(detail::Write<float>(1.256f), "1.256000");
    CHECK_EQ(detail::Write<uint64_t>(5), "5");
    CHECK_EQ(detail::Write<int64_t>(5), "5");
    CHECK_EQ(detail::Write<int>(5), "5");
    CHECK_EQ(detail::Write<int>(-5), "-5");
}

TEST_CASE("New closed database") {
    KeyValueDatabase db{};
    CHECK_FALSE(db.IsOpen());
}

TEST_CASE("Opening and Reopening") {
    TempDbFile file{};
    KeyValueDatabase db{};

    auto status = db.Open(file.ToString());
    REQUIRE(status.ok());
    status = db.Open(file.ToString());
    REQUIRE(status.ok());
}

TEST_CASE("Database is open after opening") {
    TempDbFile file{};
    KeyValueDatabase db{};
    auto status = db.Open(file.ToString());

    REQUIRE(status.ok());
    REQUIRE(db.IsOpen());
    db.Close();
}

TEST_CASE("Database is not open after closing") {
    TempDbFile file{};
    KeyValueDatabase db{};
    auto status = db.Open(file.ToString());

    REQUIRE(status.ok());
    REQUIRE(db.IsOpen());

    db.Close();
    REQUIRE_FALSE(db.IsOpen());
}

TEST_CASE("Write and read on opened db") {
    TempDbFile file{};
    KeyValueDatabase db{};
    int myVal = 0;

    REQUIRE(db.Open(file.ToString()).ok());
    REQUIRE(db.Put("myKey", 5).ok());
    REQUIRE(db.Get("myKey", myVal).ok());
    REQUIRE(myVal == 5);
}

TEST_CASE("Persisted write and read on db") {
    TempDbFile file{};
    KeyValueDatabase db{};
    int myVal = 0;

    REQUIRE(db.Open(file.ToString()).ok());
    REQUIRE(db.Put("myKey", 5).ok());
    db.Close();
    REQUIRE_FALSE(db.IsOpen());
    REQUIRE(db.Open(file.ToString()).ok());
    REQUIRE(db.Get("myKey", myVal).ok());
    REQUIRE(myVal == 5);
}