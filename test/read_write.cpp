#include "doctest.h"

#include <kvdb/db.h>

using namespace st;

struct Dummy{
    std::string prop1;
    int prop2;
    bool prop3;
};

static constexpr char kDummyString[] = R"({"prop1":"hello","prop2":5,"prop3":false})";

TEST_CASE("Test Struct Read"){
    Dummy value = internal::Read<Dummy>(kDummyString).value();
    CHECK_EQ(value.prop1, "hello");
    CHECK_EQ(value.prop2, 5);
    CHECK_EQ(value.prop3, false);
}

TEST_CASE("Test Struct Write"){
    CHECK_EQ(internal::Write<Dummy>(Dummy("hello", 5, false)), kDummyString);
}

TEST_CASE("Test Read"){
    CHECK_EQ(internal::Read<std::string>("hello world").value(),"hello world");
    CHECK_EQ(internal::Read<bool>("0").value(), false);
    CHECK_EQ(internal::Read<bool>("1").value(), true);
    CHECK_EQ(internal::Read<double>("1.256").value(), 1.256);
    CHECK_EQ(internal::Read<float>("1.256").value(), 1.256f);
    CHECK_EQ(internal::Read<uint64_t>("2354543435454").value(), static_cast<uint64_t>(2354543435454));
    CHECK_EQ(internal::Read<int64_t>("2354543435454").value(), static_cast<int64_t>(2354543435454));
    CHECK_EQ(internal::Read<int>("1256").value(), 1256);
}

TEST_CASE("Test Write"){
    CHECK_EQ(internal::Write<std::string>("hello world"), "hello world");
    CHECK_EQ(internal::Write<bool>(false), "0");
    CHECK_EQ(internal::Write<bool>(true), "1");
    CHECK_EQ(internal::Write<double>(1.256), "1.256000"); // For some reason to string appends three zeros
    CHECK_EQ(internal::Write<float>(1.256f), "1.256000");
    CHECK_EQ(internal::Write<uint64_t>(2354543435454), "2354543435454");
    CHECK_EQ(internal::Write<int64_t>(2354543435454), "2354543435454");
    CHECK_EQ(internal::Write<int>(1256), "1256");
}