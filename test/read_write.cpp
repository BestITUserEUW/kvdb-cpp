#include "doctest.h"

#include <filesystem>

#include <kvdb/db.h>

namespace fs = std::filesystem;
using namespace st;

struct Dummy {
    std::string prop0;
    int prop1;
    bool prop2;
};

static constexpr char kDummyString[] = R"({"prop0":"hello","prop1":5,"prop2":false})";
static constexpr char kCorruptDummyString[] = R"({"prop"hello","prop5":5,"prop6":false})";

TEST_CASE("Reading string into struct") {
    Dummy value = internal::Read<Dummy>(kDummyString).value();
    CHECK(value.prop0 == "hello");
    CHECK(value.prop1 == 5);
    CHECK(value.prop2 == false);
}

TEST_CASE("Reading corrupt string into struct") {
    auto result = internal::Read<Dummy>(kCorruptDummyString);
    REQUIRE_FALSE(result);
}

TEST_CASE("Writing struct to string") {
    REQUIRE(internal::Write<Dummy>(Dummy("hello", 5, false)) == std::string(kDummyString));
}

TEST_CASE("Reading supported types") {
    CHECK_EQ(internal::Read<std::string>("hello world1232!*2`-.").value(), "hello world1232!*2`-.");
    CHECK(internal::Read<bool>("0").value() == false);
    CHECK(internal::Read<bool>("1").value() == true);
    CHECK(internal::Read<double>("1.256").value() == 1.256);
    CHECK(internal::Read<double>("-1.256").value() == -1.256);
    CHECK(internal::Read<float>("1.256").value() == 1.256f);
    CHECK(internal::Read<float>("-1.256").value() == -1.256f);
    CHECK(internal::Read<uint64_t>("5").value() == static_cast<uint64_t>(5));
    CHECK(internal::Read<int64_t>("5").value() == static_cast<int64_t>(5));
    CHECK(internal::Read<int64_t>("-5").value() == static_cast<int64_t>(-5));
    CHECK(internal::Read<int>("5").value() == 5);
    CHECK(internal::Read<int>("-5").value() == -5);
}

TEST_CASE("Writing supported types") {
    CHECK_EQ(internal::Write<std::string>("hello world1232!*2`-."), "hello world1232!*2`-.");
    CHECK_EQ(internal::Write<bool>(false), "0");
    CHECK_EQ(internal::Write<bool>(true), "1");
    CHECK_EQ(internal::Write<double>(1.256), "1.256000");  // std::to_string appends 0 at the end for some reason
    CHECK_EQ(internal::Write<float>(1.256f), "1.256000");
    CHECK_EQ(internal::Write<uint64_t>(5), "5");
    CHECK_EQ(internal::Write<int64_t>(5), "5");
    CHECK_EQ(internal::Write<int>(5), "5");
    CHECK_EQ(internal::Write<int>(-5), "-5");
}

TEST_CASE("New closed database") {
    DB db;
    CHECK_FALSE(db.IsOpen());
}

TEST_CASE("Database is open after opening") {
    auto temp_file = fs::temp_directory_path();
    temp_file.append("tmp.db");
    DB db;
    auto status = db.Open(temp_file.string());

    REQUIRE(status.ok());
    REQUIRE(db.IsOpen());

    db.Close();
    std::filesystem::remove_all(temp_file);
}

TEST_CASE("Database is not open after closing") {
    auto temp_file = fs::temp_directory_path();
    temp_file.append("tmp.db");
    DB db;
    auto status = db.Open(temp_file.string());

    REQUIRE(status.ok());
    REQUIRE(db.IsOpen());

    db.Close();

    REQUIRE_FALSE(db.IsOpen());
    std::filesystem::remove_all(temp_file);
}

TEST_CASE("Write and read on opened db") {
    auto temp_file = fs::temp_directory_path();
    temp_file.append("tmp.db");
    DB db;
    int myVal = 0;

    REQUIRE(db.Open(temp_file.string()).ok());
    REQUIRE(db.Put("myKey", 5).ok());
    REQUIRE(db.Get("myKey", myVal).ok());
    REQUIRE(myVal == 5);

    db.Close();
    std::filesystem::remove_all(temp_file);
}

TEST_CASE("Persisted write and read on db") {
    auto temp_file = fs::temp_directory_path();
    temp_file.append("tmp.db");
    DB db;
    int myVal = 0;

    REQUIRE(db.Open(temp_file.string()).ok());
    REQUIRE(db.Put("myKey", 5).ok());
    db.Close();
    REQUIRE_FALSE(db.IsOpen());
    REQUIRE(db.Open(temp_file.string()).ok());
    REQUIRE(db.Get("myKey", myVal).ok());
    REQUIRE(myVal == 5);

    db.Close();
    std::filesystem::remove_all(temp_file);
}