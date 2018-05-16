/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) FIRST 2017. All Rights Reserved.               */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 2.1.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "gtest/gtest.h"

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "unit-json.h"
using wpi::json;
using wpi::JsonTest;

class JsonConstructTypeTest : public ::testing::TestWithParam<json::value_t> {};
TEST_P(JsonConstructTypeTest, Case)
{
    auto t = GetParam();
    json j(t);
    EXPECT_EQ(j.type(), t);
}

static const json::value_t construct_type_cases[] = {
    json::value_t::null,
    json::value_t::discarded,
    json::value_t::object,
    json::value_t::array,
    json::value_t::boolean,
    json::value_t::string,
    json::value_t::number_integer,
    json::value_t::number_unsigned,
    json::value_t::number_float,
};

INSTANTIATE_TEST_CASE_P(JsonConstructTypeTests, JsonConstructTypeTest,
                        ::testing::ValuesIn(construct_type_cases), );


TEST(JsonConstructNullTest, NoParameter)
{
    json j{};
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonConstructNullTest, Parameter)
{
    json j(nullptr);
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonConstructObjectExplicitTest, Empty)
{
    json::object_t o;
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConstructObjectExplicitTest, Filled)
{
    json::object_t o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
}

class JsonConstructObjectImplicitTest : public ::testing::Test {
 public:
    JsonConstructObjectImplicitTest() : j_reference(o_reference) {}

 protected:
    json::object_t o_reference {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j_reference;
};

// std::map<std::string, json>
TEST_F(JsonConstructObjectImplicitTest, StdMapStringJson)
{
    std::map<std::string, json> o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

// std::pair<CompatibleString, T>
TEST_F(JsonConstructObjectImplicitTest, StdPairStringT)
{
    std::pair<std::string, std::string> p{"first", "second"};
    json j(p);

    EXPECT_EQ(j.get<decltype(p)>(), p);

    std::pair<std::string, int> p2{"first", 1};
    // use char const*
    json j2(std::make_pair("first", 1));

    EXPECT_EQ(j2.get<decltype(p2)>(), p2);
}

// std::map<std::string, std::string>
TEST_F(JsonConstructObjectImplicitTest, StdMapStringString)
{
    std::map<std::string, std::string> m;
    m["a"] = "b";
    m["c"] = "d";
    m["e"] = "f";

    json j(m);
    EXPECT_EQ(j.get<decltype(m)>(), m);
}

// std::map<const char*, json>
TEST_F(JsonConstructObjectImplicitTest, StdMapCharPointerJson)
{
    std::map<const char*, json> o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

// std::multimap<std::string, json>
TEST_F(JsonConstructObjectImplicitTest, StdMultiMapStringJson)
{
    std::multimap<std::string, json> o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

// std::unordered_map<std::string, json>
TEST_F(JsonConstructObjectImplicitTest, StdUnorderedMapStringJson)
{
    std::unordered_map<std::string, json> o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

// std::unordered_multimap<std::string, json>
TEST_F(JsonConstructObjectImplicitTest, StdUnorderedMultiMapStringJson)
{
    std::unordered_multimap<std::string, json> o {{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}};
    json j(o);
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

// associative container literal
TEST_F(JsonConstructObjectImplicitTest, AssociativeContainerLiteral)
{
    json j({{"a", json(1)}, {"b", json(1u)}, {"c", json(2.2)}, {"d", json(false)}, {"e", json("string")}, {"f", json()}});
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j, j_reference);
}

TEST(JsonConstructArrayExplicitTest, Empty)
{
    json::array_t a;
    json j(a);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructArrayExplicitTest, Filled)
{
    json::array_t a {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j(a);
    EXPECT_EQ(j.type(), json::value_t::array);
}

template <typename T>
class JsonConstructArrayTest : public ::testing::Test {
 public:
    JsonConstructArrayTest() : j_reference(a_reference) {}

 protected:
    json::array_t a_reference {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j_reference;
};

typedef ::testing::Types<std::list<json>, std::forward_list<json>,
                         std::array<json, 6>, std::vector<json>,
                         std::deque<json>>
    JsonConstructArrayTestTypes;
TYPED_TEST_CASE(JsonConstructArrayTest, JsonConstructArrayTestTypes);

// clang warns on missing braces on the TypeParam initializer line below.
// Suppress this warning.
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif
TYPED_TEST(JsonConstructArrayTest, Implicit)
{
    TypeParam a {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j(a);
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, this->j_reference);
}
#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

// std::set<json>
TEST(JsonConstructArraySetTest, StdSet)
{
    std::set<json> a {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j(a);
    EXPECT_EQ(j.type(), json::value_t::array);
    // we cannot really check for equality here
}

// std::unordered_set<json>
TEST(JsonConstructArraySetTest, StdUnorderedSet)
{
    std::unordered_set<json> a {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j(a);
    EXPECT_EQ(j.type(), json::value_t::array);
    // we cannot really check for equality here
}

// sequence container literal
TEST(JsonConstructArrayContainerTest, Case)
{
    json::array_t a_reference {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j_reference(a_reference);

    json j({json(1), json(1u), json(2.2), json(false), json("string"), json()});
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, j_reference);
}

TEST(JsonConstructStringExplicitTest, Empty)
{
    std::string s;
    json j(s);
    EXPECT_EQ(j.type(), json::value_t::string);
}

TEST(JsonConstructStringExplicitTest, Filled)
{
    std::string s {"Hello world"};
    json j(s);
    EXPECT_EQ(j.type(), json::value_t::string);
}

class JsonConstructStringTest : public ::testing::Test {
 public:
    JsonConstructStringTest() : j_reference(s_reference) {}

 protected:
    std::string s_reference {"Hello world"};
    json j_reference;
};

// std::string
TEST_F(JsonConstructStringTest, StdString)
{
    std::string s {"Hello world"};
    json j(s);
    EXPECT_EQ(j.type(), json::value_t::string);
    EXPECT_EQ(j, j_reference);
}

// char[]
TEST_F(JsonConstructStringTest, CharArray)
{
    char s[] {"Hello world"};
    json j(s);
    EXPECT_EQ(j.type(), json::value_t::string);
    EXPECT_EQ(j, j_reference);
}

// const char*
TEST_F(JsonConstructStringTest, ConstCharPointer)
{
    const char* s {"Hello world"};
    json j(s);
    EXPECT_EQ(j.type(), json::value_t::string);
    EXPECT_EQ(j, j_reference);
}

// string literal
TEST_F(JsonConstructStringTest, StringLiteral)
{
    json j("Hello world");
    EXPECT_EQ(j.type(), json::value_t::string);
    EXPECT_EQ(j, j_reference);
}

TEST(JsonConstructBooleanExplicitTest, Empty)
{
    bool b{};
    json j(b);
    EXPECT_EQ(j.type(), json::value_t::boolean);
}

TEST(JsonConstructBooleanExplicitTest, True)
{
    json j(true);
    EXPECT_EQ(j.type(), json::value_t::boolean);
}

TEST(JsonConstructBooleanExplicitTest, False)
{
    json j(false);
    EXPECT_EQ(j.type(), json::value_t::boolean);
}

TEST(JsonConstructIntegerExplicitTest, Uninitialized)
{
    int64_t n{};
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_integer);
}

TEST(JsonConstructIntegerExplicitTest, Initialized)
{
    int64_t n(42);
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_integer);
}

template <typename T>
class JsonConstructIntegerTest : public ::testing::Test {
 public:
    JsonConstructIntegerTest()
        : j_reference(n_reference), j_unsigned_reference(n_unsigned_reference) {}

 protected:
    int64_t n_reference = 42;
    json j_reference;
    uint64_t n_unsigned_reference = 42u;
    json j_unsigned_reference;
};

typedef ::testing::Types<
      short
    , unsigned short
    , int
    , unsigned int
    , long
    , unsigned long
    , long long
    , unsigned long long
    , int8_t
    , int16_t
    , int32_t
    , int64_t
#if 0
    , int8_fast_t
    , int16_fast_t
    , int32_fast_t
    , int64_fast_t
    , int8_least_t
    , int16_least_t
    , int32_least_t
    , int64_least_t
#endif
    , uint8_t
    , uint16_t
    , uint32_t
    , uint64_t
#if 0
    , uint8_fast_t
    , uint16_fast_t
    , uint32_fast_t
    , uint64_fast_t
    , uint8_least_t
    , uint16_least_t
    , uint32_least_t
    , uint64_least_t
#endif
    > JsonConstructIntegerTestTypes;

TYPED_TEST_CASE(JsonConstructIntegerTest, JsonConstructIntegerTestTypes);

TYPED_TEST(JsonConstructIntegerTest, Implicit)
{
    TypeParam n = 42;
    json j(n);
    if (std::is_unsigned<TypeParam>::value)
    {
        EXPECT_EQ(j.type(), json::value_t::number_unsigned);
        EXPECT_EQ(j, this->j_unsigned_reference);
    }
    else
    {
        EXPECT_EQ(j.type(), json::value_t::number_integer);
        EXPECT_EQ(j, this->j_reference);
    }
}

class JsonConstructIntegerLiteralTest : public ::testing::Test {
 public:
    JsonConstructIntegerLiteralTest()
        : j_reference(n_reference), j_unsigned_reference(n_unsigned_reference) {}

 protected:
    int64_t n_reference = 42;
    json j_reference;
    uint64_t n_unsigned_reference = 42u;
    json j_unsigned_reference;
};

TEST_F(JsonConstructIntegerLiteralTest, None)
{
    json j(42);
    EXPECT_EQ(j.type(), json::value_t::number_integer);
    EXPECT_EQ(j, j_reference);
}

TEST_F(JsonConstructIntegerLiteralTest, U)
{
    json j(42u);
    EXPECT_EQ(j.type(), json::value_t::number_unsigned);
    EXPECT_EQ(j, j_unsigned_reference);
}

TEST_F(JsonConstructIntegerLiteralTest, L)
{
    json j(42l);
    EXPECT_EQ(j.type(), json::value_t::number_integer);
    EXPECT_EQ(j, j_reference);
}

TEST_F(JsonConstructIntegerLiteralTest, UL)
{
    json j(42ul);
    EXPECT_EQ(j.type(), json::value_t::number_unsigned);
    EXPECT_EQ(j, j_unsigned_reference);
}

TEST_F(JsonConstructIntegerLiteralTest, LL)
{
    json j(42ll);
    EXPECT_EQ(j.type(), json::value_t::number_integer);
    EXPECT_EQ(j, j_reference);
}

TEST_F(JsonConstructIntegerLiteralTest, ULL)
{
    json j(42ull);
    EXPECT_EQ(j.type(), json::value_t::number_unsigned);
    EXPECT_EQ(j, j_unsigned_reference);
}

TEST(JsonConstructFloatExplicitTest, Uninitialized)
{
    double n{};
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_float);
}

TEST(JsonConstructFloatExplicitTest, Initialized)
{
    double n(42.23);
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_float);
}

TEST(JsonConstructFloatExplicitTest, Infinity)
{
    // infinity is stored properly, but serialized to null
    double n(std::numeric_limits<double>::infinity());
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_float);

    // check round trip of infinity
    double d = j;
    EXPECT_EQ(d, n);

    // check that inf is serialized to null
    EXPECT_EQ(j.dump(), "null");
}

template <typename T>
class JsonConstructFloatTest : public ::testing::Test {
 public:
    JsonConstructFloatTest() : j_reference(n_reference) {}

 protected:
    double n_reference {42.23};
    json j_reference;
};

typedef ::testing::Types<float, double
#if 0
                         , long double
#endif
                         >
    JsonConstructFloatTestTypes;

TYPED_TEST_CASE(JsonConstructFloatTest, JsonConstructFloatTestTypes);

TYPED_TEST(JsonConstructFloatTest, Implicit)
{
    TypeParam n = 42.23f;
    json j(n);
    EXPECT_EQ(j.type(), json::value_t::number_float);
    EXPECT_LT(std::fabs(JsonTest::GetValue(j).number_float -
                        JsonTest::GetValue(this->j_reference).number_float),
              0.001);
}

class JsonConstructFloatLiteralTest : public ::testing::Test {
 public:
    JsonConstructFloatLiteralTest() : j_reference(n_reference) {}

 protected:
    double n_reference {42.23};
    json j_reference;
};

TEST_F(JsonConstructFloatLiteralTest, None)
{
    json j(42.23);
    EXPECT_EQ(j.type(), json::value_t::number_float);
    EXPECT_LT(std::fabs(JsonTest::GetValue(j).number_float -
                        JsonTest::GetValue(this->j_reference).number_float),
              0.001);
}

TEST_F(JsonConstructFloatLiteralTest, F)
{
    json j(42.23f);
    EXPECT_EQ(j.type(), json::value_t::number_float);
    EXPECT_LT(std::fabs(JsonTest::GetValue(j).number_float -
                        JsonTest::GetValue(this->j_reference).number_float),
              0.001);
}

#if 0
TEST_F(JsonConstructFloatLiteralTest, L)
{
    json j(42.23l);
    EXPECT_EQ(j.type(), json::value_t::number_float);
    EXPECT_LT(std::fabs(JsonTest::GetValue(j).number_float -
                        JsonTest::GetValue(this->j_reference).number_float),
              0.001);
}
#endif

TEST(JsonConstructInitializerEmptyTest, Explicit)
{
    json j(json::initializer_list_t{});
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConstructInitializerEmptyTest, Implicit)
{
    json j {};
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonConstructInitializerOneTest, ExplicitArray)
{
    std::initializer_list<json> l = {json(json::array_t())};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitArray)
{
    json j {json::array_t()};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitObject)
{
    std::initializer_list<json> l = {json(json::object_t())};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitObject)
{
    json j {json::object_t()};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitString)
{
    std::initializer_list<json> l = {json("Hello world")};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitString)
{
    json j {"Hello world"};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitBoolean)
{
    std::initializer_list<json> l = {json(true)};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitBoolean)
{
    json j {true};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitInteger)
{
    std::initializer_list<json> l = {json(1)};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitInteger)
{
    json j {1};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitUnsigned)
{
    std::initializer_list<json> l = {json(1u)};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitUnsigned)
{
    json j {1u};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ExplicitFloat)
{
    std::initializer_list<json> l = {json(42.23)};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerOneTest, ImplicitFloat)
{
    json j {42.23};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerManyTest, Explicit)
{
    std::initializer_list<json> l = {1, 1u, 42.23, true, nullptr, json::object_t(), json::array_t()};
    json j(l);
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerManyTest, Implicit)
{
    json j {1, 1u, 42.23, true, nullptr, json::object_t(), json::array_t()};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerImplicitTest, Object)
{
    json j { {"one", 1}, {"two", 1u}, {"three", 2.2}, {"four", false} };
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConstructInitializerImplicitTest, Array)
{
    json j { {"one", 1}, {"two", 1u}, {"three", 2.2}, {"four", false}, 13 };
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerExplicitTest, EmptyObject)
{
    json j = json::object();
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConstructInitializerExplicitTest, Object)
{
    json j = json::object({ {"one", 1}, {"two", 1u}, {"three", 2.2}, {"four", false} });
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConstructInitializerExplicitTest, ObjectError)
{
    EXPECT_THROW_MSG(json::object({ {"one", 1}, {"two", 1u}, {"three", 2.2}, {"four", false}, 13 }),
    json::type_error,
    "[json.exception.type_error.301] cannot create object from initializer list");
}

// std::pair<CompatibleString, T> with error
TEST(JsonConstructInitializerPairErrorTest, WrongFieldNumber)
{
    json j{{"too", "much"}, {"string", "fields"}};
    EXPECT_THROW_MSG((j.get<std::pair<std::string, std::string>>()), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with object");
}

TEST(JsonConstructInitializerPairErrorTest, WrongJsonType)
{
    json j(42);
    EXPECT_THROW_MSG((j.get<std::pair<std::string, std::string>>()), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST(JsonConstructInitializerTest, EmptyArray)
{
    json j = json::array();
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConstructInitializerTest, Array)
{
    json j = json::array({ {"one", 1}, {"two", 1u}, {"three", 2.2}, {"four", false} });
    EXPECT_EQ(j.type(), json::value_t::array);
}

// create an array of n copies of a given value
TEST(JsonConstructArrayCopyTest, Case)
{
    json v = {1, "foo", 34.23, {1, 2, 3}, {{"A", 1}, {"B", 2u}}};
    json arr(3, v);
    EXPECT_EQ(arr.size(), 3u);
    for (auto& x : arr)
    {
        EXPECT_EQ(x, v);
    }
}

// create a JSON container from an iterator range
TEST(JsonConstructIteratorTest, ObjectBeginEnd)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json j_new(jobject.begin(), jobject.end());
    EXPECT_EQ(j_new, jobject);
#else
    EXPECT_THROW(json(jobject.begin(), jobject.end()), json::invalid_iterator);
#endif
}

TEST(JsonConstructIteratorTest, ObjectBeginEndConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json j_new(jobject.cbegin(), jobject.cend());
    EXPECT_EQ(j_new, jobject);
#else
    EXPECT_THROW(json(jobject.cbegin(), jobject.cend()), json::invalid_iterator);
#endif
}

TEST(JsonConstructIteratorTest, ObjectBeginBegin)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json j_new(jobject.begin(), jobject.begin());
    EXPECT_EQ(j_new, json::object());
#else
    EXPECT_THROW(json(jobject.begin(), jobject.end()), json::invalid_iterator);
#endif
}

TEST(JsonConstructIteratorTest, ObjectBeginBeginConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json j_new(jobject.cbegin(), jobject.cbegin());
    EXPECT_EQ(j_new, json::object());
#else
    EXPECT_THROW(json(jobject.cbegin(), jobject.cend()), json::invalid_iterator);
#endif
}
#if 0
TEST(JsonConstructIteratorTest, ObjectSubrange)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
    json j_new(jobject.find("b"), jobject.find("e"));
    EXPECT_EQ(j_new, json({{"b", 1}, {"c", 17u}, {"d", false}}));
}
#endif
TEST(JsonConstructIteratorTest, ObjectIncompatibleIterators)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
    json jobject2 = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    EXPECT_THROW_MSG(json(jobject.begin(), jobject2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
    EXPECT_THROW_MSG(json(jobject2.begin(), jobject.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
}

TEST(JsonConstructIteratorTest, ObjectIncompatibleIteratorsConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
    json jobject2 = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    EXPECT_THROW_MSG(json(jobject.cbegin(), jobject2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
    EXPECT_THROW_MSG(json(jobject2.cbegin(), jobject.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
}

TEST(JsonConstructIteratorTest, ArrayBeginEnd)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.begin(), jarray.end());
    EXPECT_EQ(j_new, jarray);
}

TEST(JsonConstructIteratorTest, ArrayBeginEndConst)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.cbegin(), jarray.cend());
    EXPECT_EQ(j_new, jarray);
}

TEST(JsonConstructIteratorTest, ArrayBeginBegin)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.begin(), jarray.begin());
    EXPECT_EQ(j_new, json::array());
}

TEST(JsonConstructIteratorTest, ArrayBeginBeginConst)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.cbegin(), jarray.cbegin());
    EXPECT_EQ(j_new, json::array());
}

TEST(JsonConstructIteratorTest, ArraySubrange)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.begin() + 1, jarray.begin() + 3);
    EXPECT_EQ(j_new, json({2, 3}));
}

TEST(JsonConstructIteratorTest, ArraySubrangeConst)
{
    json jarray = {1, 2, 3, 4, 5};
    json j_new(jarray.cbegin() + 1, jarray.cbegin() + 3);
    EXPECT_EQ(j_new, json({2, 3}));
}

TEST(JsonConstructIteratorTest, ArrayIncompatibleIterators)
{
    json jarray = {1, 2, 3, 4};
    json jarray2 = {2, 3, 4, 5};
    EXPECT_THROW_MSG(json(jarray.begin(), jarray2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
    EXPECT_THROW_MSG(json(jarray2.begin(), jarray.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
}

TEST(JsonConstructIteratorTest, ArrayIncompatibleIteratorsConst)
{
    json jarray = {1, 2, 3, 4};
    json jarray2 = {2, 3, 4, 5};
    EXPECT_THROW_MSG(json(jarray.cbegin(), jarray2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
    EXPECT_THROW_MSG(json(jarray2.cbegin(), jarray.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.201] iterators are not compatible");
}

TEST(JsonConstructTwoValidIteratorTest, Null)
{
    json j;
    EXPECT_THROW_MSG(json(j.begin(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.206] cannot construct with iterators from null");
}

TEST(JsonConstructTwoValidIteratorTest, NullConst)
{
    json j;
    EXPECT_THROW_MSG(json(j.cbegin(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.206] cannot construct with iterators from null");
}

TEST(JsonConstructTwoValidIteratorTest, String)
{
    json j = "foo";
    json j_new(j.begin(), j.end());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, StringConst)
{
    json j = "bar";
    json j_new(j.cbegin(), j.cend());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, Boolean)
{
    json j = false;
    json j_new(j.begin(), j.end());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, BooleanConst)
{
    json j = true;
    json j_new(j.cbegin(), j.cend());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, Integer)
{
    json j = 17;
    json j_new(j.begin(), j.end());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, IntegerConst)
{
    json j = 17;
    json j_new(j.cbegin(), j.cend());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, Unsigned)
{
    json j = 17u;
    json j_new(j.begin(), j.end());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, UnsignedConst)
{
    json j = 17u;
    json j_new(j.cbegin(), j.cend());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, Float)
{
    json j = 23.42;
    json j_new(j.begin(), j.end());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoValidIteratorTest, FloatConst)
{
    json j = 23.42;
    json j_new(j.cbegin(), j.cend());
    EXPECT_EQ(j, j_new);
}

TEST(JsonConstructTwoInvalidIteratorTest, String)
{
    json j = "foo";
    EXPECT_THROW_MSG(json(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, StringConst)
{
    json j = "bar";
    EXPECT_THROW_MSG(json(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, Boolean)
{
    json j = false;
    EXPECT_THROW_MSG(json(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, BooleanConst)
{
    json j = true;
    EXPECT_THROW_MSG(json(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, Integer)
{
    json j = 17;
    EXPECT_THROW_MSG(json(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, IntegerConst)
{
    json j = 17;
    EXPECT_THROW_MSG(json(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, Unsigned)
{
    json j = 17u;
    EXPECT_THROW_MSG(json(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, UnsignedConst)
{
    json j = 17u;
    EXPECT_THROW_MSG(json(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, Float)
{
    json j = 23.42;
    EXPECT_THROW_MSG(json(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonConstructTwoInvalidIteratorTest, FloatConst)
{
    json j = 23.42;
    EXPECT_THROW_MSG(json(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(json(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}
