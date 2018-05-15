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

#include "unit-json.h"
using wpi::json;
using wpi::JsonTest;

#include <deque>
//#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>

template <typename T>
class JsonGetObjectTest : public ::testing::Test {
 public:
    JsonGetObjectTest() : j(o_reference) {}

 protected:
    json::object_t o_reference = {{"object", json::object()}, {"array", {1, 2, 3, 4}}, {"number", 42}, {"boolean", false}, {"null", nullptr}, {"string", "Hello world"} };
    json j;
};

typedef ::testing::Types<
      json::object_t
    , std::map<std::string, json>
    , std::multimap<std::string, json>
    , std::unordered_map<std::string, json>
    , std::unordered_multimap<std::string, json>
    > JsonGetObjectTestTypes;
TYPED_TEST_CASE(JsonGetObjectTest, JsonGetObjectTestTypes);

TYPED_TEST(JsonGetObjectTest, Explicit)
{
    TypeParam o = (this->j).template get<TypeParam>();
    EXPECT_EQ(json(o), this->j);
}

TYPED_TEST(JsonGetObjectTest, Implicit)
{
    TypeParam o = this->j;
    EXPECT_EQ(json(o), this->j);
}

// exception in case of a non-object type
TEST(JsonGetObjectExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is null");
    EXPECT_THROW_MSG(json(json::value_t::array).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is array");
    EXPECT_THROW_MSG(json(json::value_t::string).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is string");
    EXPECT_THROW_MSG(json(json::value_t::boolean).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is boolean");
    EXPECT_THROW_MSG(json(json::value_t::number_integer).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_unsigned).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_float).get<json::object_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is number");
}

template <typename T>
class JsonGetArrayTest : public ::testing::Test {
 public:
    JsonGetArrayTest() : j(a_reference) {}

 protected:
    json::array_t a_reference {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j;
};

typedef ::testing::Types<json::array_t, std::list<json>,
                         /*std::forward_list<json>,*/ std::vector<json>,
                         std::deque<json>>
    JsonGetArrayTestTypes;
TYPED_TEST_CASE(JsonGetArrayTest, JsonGetArrayTestTypes);

TYPED_TEST(JsonGetArrayTest, Explicit)
{
    TypeParam a = (this->j).template get<TypeParam>();
    EXPECT_EQ(json(a), this->j);
}

TYPED_TEST(JsonGetArrayTest, Implicit)
{
    TypeParam a = this->j;
    EXPECT_EQ(json(a), this->j);
}

#if !defined(JSON_NOEXCEPTION)
// reserve is called on containers that supports it
TEST(JsonGetArrayAdditionalTest, ExplicitStdVectorReserve)
{
    json::array_t a_reference {json(1), json(1u), json(2.2), json(false), json("string"), json()};
    json j(a_reference);

    // making the call to from_json throw in order to check capacity
    std::vector<float> v;
    EXPECT_THROW(wpi::from_json(j, v), json::type_error);
    EXPECT_EQ(v.capacity(), j.size());

    // make sure all values are properly copied
    std::vector<int> v2 = json({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    EXPECT_EQ(v2.size(), 10u);
}
#endif

// built-in arrays
TEST(JsonGetArrayAdditionalTest, ExplicitBuiltinArray)
{
    const char str[] = "a string";
    const int nbs[] = {0, 1, 2};

    json j2 = nbs;
    json j3 = str;

    auto v = j2.get<std::vector<int>>();
    auto s = j3.get<std::string>();
    EXPECT_TRUE(std::equal(v.begin(), v.end(), std::begin(nbs)));
    EXPECT_EQ(s, str);
}
#if 0
TEST(JsonGetArrayExceptionTest, ForwardList)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<std::forward_list<json>>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
}
#endif
TEST(JsonGetArrayExceptionTest, StdVector)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<std::vector<json>>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
}

// exception in case of a non-array type
TEST(JsonGetArrayExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::object).get<std::vector<int>>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is object");
    EXPECT_THROW_MSG(json(json::value_t::null).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
    EXPECT_THROW_MSG(json(json::value_t::object).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is object");
    EXPECT_THROW_MSG(json(json::value_t::string).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is string");
    EXPECT_THROW_MSG(json(json::value_t::boolean).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is boolean");
    EXPECT_THROW_MSG(json(json::value_t::number_integer).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_unsigned).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_float).get<json::array_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is number");
}

template <typename T>
class JsonGetStringTest : public ::testing::Test {
 public:
    JsonGetStringTest() : j(s_reference) {}

 protected:
    std::string s_reference {"Hello world"};
    json j;
};

typedef ::testing::Types<std::string, std::string> JsonGetStringTestTypes;
TYPED_TEST_CASE(JsonGetStringTest, JsonGetStringTestTypes);

TYPED_TEST(JsonGetStringTest, Explicit)
{
    TypeParam s = (this->j).template get<TypeParam>();
    EXPECT_EQ(json(s), this->j);
}

TYPED_TEST(JsonGetStringTest, Implicit)
{
    TypeParam s = this->j;
    EXPECT_EQ(json(s), this->j);
}

// exception in case of a non-string type
TEST(JsonGetStringExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is null");
    EXPECT_THROW_MSG(json(json::value_t::object).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is object");
    EXPECT_THROW_MSG(json(json::value_t::array).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is array");
    EXPECT_THROW_MSG(json(json::value_t::boolean).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is boolean");
    EXPECT_THROW_MSG(json(json::value_t::number_integer).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_unsigned).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_float).get<std::string>(), json::type_error,
                     "[json.exception.type_error.302] type must be string, but is number");
}

template <typename T>
class JsonGetBooleanTest : public ::testing::Test {
 public:
    JsonGetBooleanTest() : j(b_reference) {}

 protected:
    bool b_reference {true};
    json j;
};

typedef ::testing::Types<bool, bool> JsonGetBooleanTestTypes;
TYPED_TEST_CASE(JsonGetBooleanTest, JsonGetBooleanTestTypes);

TYPED_TEST(JsonGetBooleanTest, Explicit)
{
    TypeParam b = (this->j).template get<TypeParam>();
    EXPECT_EQ(json(b), this->j);
}

TYPED_TEST(JsonGetBooleanTest, Implicit)
{
    TypeParam b = this->j;
    EXPECT_EQ(json(b), this->j);
}

// exception in case of a non-string type
TEST(JsonGetBooleanExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is null");
    EXPECT_THROW_MSG(json(json::value_t::object).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is object");
    EXPECT_THROW_MSG(json(json::value_t::array).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is array");
    EXPECT_THROW_MSG(json(json::value_t::string).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is string");
    EXPECT_THROW_MSG(json(json::value_t::number_integer).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_unsigned).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is number");
    EXPECT_THROW_MSG(json(json::value_t::number_float).get<bool>(), json::type_error,
                     "[json.exception.type_error.302] type must be boolean, but is number");
}

template <typename T>
class JsonGetIntegerTest : public ::testing::Test {
 public:
    JsonGetIntegerTest() : j(n_reference), j_unsigned(n_unsigned_reference) {}

 protected:
    int64_t n_reference {42};
    json j;
    uint64_t n_unsigned_reference {42u};
    json j_unsigned;
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
    > JsonGetIntegerTestTypes;

TYPED_TEST_CASE(JsonGetIntegerTest, JsonGetIntegerTestTypes);

TYPED_TEST(JsonGetIntegerTest, Explicit)
{
    TypeParam n = (this->j).template get<TypeParam>();
    EXPECT_EQ(json(n), this->j);
}

TYPED_TEST(JsonGetIntegerTest, Implicit)
{
    if (std::is_unsigned<TypeParam>::value)
    {
        TypeParam n = this->j_unsigned;
        EXPECT_EQ(json(n), this->j_unsigned);
    }
    else
    {
        TypeParam n = this->j;
        EXPECT_EQ(json(n), this->j);
    }
}

// exception in case of a non-number type
TEST(JsonGetIntegerExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<int64_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is null");
    EXPECT_THROW_MSG(json(json::value_t::object).get<int64_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is object");
    EXPECT_THROW_MSG(json(json::value_t::array).get<int64_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is array");
    EXPECT_THROW_MSG(json(json::value_t::string).get<int64_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is string");
    EXPECT_THROW_MSG(json(json::value_t::boolean).get<int64_t>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is boolean");

    EXPECT_NO_THROW(json(json::value_t::number_float).get<int64_t>());
    EXPECT_NO_THROW(json(json::value_t::number_float).get<uint64_t>());
}

template <typename T>
class JsonGetFloatTest : public ::testing::Test {
 public:
    JsonGetFloatTest() : j(n_reference) {}

 protected:
    double n_reference {42.23};
    json j;
};

typedef ::testing::Types<double, float, double>
    JsonGetFloatTestTypes;

TYPED_TEST_CASE(JsonGetFloatTest, JsonGetFloatTestTypes);

TYPED_TEST(JsonGetFloatTest, Explicit)
{
    TypeParam n = (this->j).template get<TypeParam>();
    EXPECT_LT(std::fabs(JsonTest::GetValue(json(n)).number_float -
                        JsonTest::GetValue(this->j).number_float), 0.001);
}

TYPED_TEST(JsonGetFloatTest, Implicit)
{
    TypeParam n = this->j;
    EXPECT_LT(std::fabs(JsonTest::GetValue(json(n)).number_float -
                        JsonTest::GetValue(this->j).number_float), 0.001);
}

// exception in case of a non-string type
TEST(JsonGetFloatExceptionTest, TypeError)
{
    EXPECT_THROW_MSG(json(json::value_t::null).get<double>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is null");
    EXPECT_THROW_MSG(json(json::value_t::object).get<double>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is object");
    EXPECT_THROW_MSG(json(json::value_t::array).get<double>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is array");
    EXPECT_THROW_MSG(json(json::value_t::string).get<double>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is string");
    EXPECT_THROW_MSG(json(json::value_t::boolean).get<double>(), json::type_error,
                     "[json.exception.type_error.302] type must be number, but is boolean");

    EXPECT_NO_THROW(json(json::value_t::number_integer).get<double>());
    EXPECT_NO_THROW(json(json::value_t::number_unsigned).get<double>());
}

TEST(JsonGetEnumTest, Case)
{
    enum c_enum { value_1, value_2 };
    enum class cpp_enum { value_1, value_2 };

    EXPECT_EQ(json(value_1).get<c_enum>(), value_1);
    EXPECT_EQ(json(cpp_enum::value_1).get<cpp_enum>(), cpp_enum::value_1);
}

class JsonObjectConversionTest : public ::testing::Test {
 protected:
    json j1 = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j2 = {{"one", 1u}, {"two", 2u}, {"three", 3u}};
    json j3 = {{"one", 1.1}, {"two", 2.2}, {"three", 3.3}};
    json j4 = {{"one", true}, {"two", false}, {"three", true}};
    json j5 = {{"one", "eins"}, {"two", "zwei"}, {"three", "drei"}};
};

TEST_F(JsonObjectConversionTest, StdMap)
{
    auto m1 = j1.get<std::map<std::string, int>>();
    auto m2 = j2.get<std::map<std::string, unsigned int>>();
    auto m3 = j3.get<std::map<std::string, double>>();
    auto m4 = j4.get<std::map<std::string, bool>>();
    //auto m5 = j5.get<std::map<std::string, std::string>>();
}

TEST_F(JsonObjectConversionTest, StdUnorderedMap)
{
    auto m1 = j1.get<std::unordered_map<std::string, int>>();
    auto m2 = j2.get<std::unordered_map<std::string, unsigned int>>();
    auto m3 = j3.get<std::unordered_map<std::string, double>>();
    auto m4 = j4.get<std::unordered_map<std::string, bool>>();
    //auto m5 = j5.get<std::unordered_map<std::string, std::string>>();
    //CHECK(m5["one"] == "eins");
}

TEST_F(JsonObjectConversionTest, StdMultiMap)
{
    auto m1 = j1.get<std::multimap<std::string, int>>();
    auto m2 = j2.get<std::multimap<std::string, unsigned int>>();
    auto m3 = j3.get<std::multimap<std::string, double>>();
    auto m4 = j4.get<std::multimap<std::string, bool>>();
    //auto m5 = j5.get<std::multimap<std::string, std::string>>();
    //CHECK(m5["one"] == "eins");
}

TEST_F(JsonObjectConversionTest, StdUnorderedMultiMap)
{
    auto m1 = j1.get<std::unordered_multimap<std::string, int>>();
    auto m2 = j2.get<std::unordered_multimap<std::string, unsigned int>>();
    auto m3 = j3.get<std::unordered_multimap<std::string, double>>();
    auto m4 = j4.get<std::unordered_multimap<std::string, bool>>();
    //auto m5 = j5.get<std::unordered_multimap<std::string, std::string>>();
    //CHECK(m5["one"] == "eins");
}

// exception in case of a non-object type
TEST_F(JsonObjectConversionTest, Exception)
{
    EXPECT_THROW_MSG((json().get<std::map<std::string, int>>()), json::type_error,
                     "[json.exception.type_error.302] type must be object, but is null");
}

class JsonArrayConversionTest : public ::testing::Test {
 protected:
    json j1 = {1, 2, 3, 4};
    json j2 = {1u, 2u, 3u, 4u};
    json j3 = {1.2, 2.3, 3.4, 4.5};
    json j4 = {true, false, true};
    json j5 = {"one", "two", "three"};
};

TEST_F(JsonArrayConversionTest, StdList)
{
    auto m1 = j1.get<std::list<int>>();
    auto m2 = j2.get<std::list<unsigned int>>();
    auto m3 = j3.get<std::list<double>>();
    auto m4 = j4.get<std::list<bool>>();
    auto m5 = j5.get<std::list<std::string>>();
}

#if 0
TEST_F(JsonArrayConversionTest, StdForwardList)
{
    auto m1 = j1.get<std::forward_list<int>>();
    auto m2 = j2.get<std::forward_list<unsigned int>>();
    auto m3 = j3.get<std::forward_list<double>>();
    auto m4 = j4.get<std::forward_list<bool>>();
    auto m5 = j5.get<std::forward_list<std::string>>();
}
#endif

TEST_F(JsonArrayConversionTest, StdVector)
{
    auto m1 = j1.get<std::vector<int>>();
    auto m2 = j2.get<std::vector<unsigned int>>();
    auto m3 = j3.get<std::vector<double>>();
    auto m4 = j4.get<std::vector<bool>>();
    auto m5 = j5.get<std::vector<std::string>>();
}

TEST_F(JsonArrayConversionTest, StdDeque)
{
    auto m1 = j1.get<std::deque<int>>();
    auto m2 = j2.get<std::deque<unsigned int>>();
    auto m3 = j2.get<std::deque<double>>();
    auto m4 = j4.get<std::deque<bool>>();
    auto m5 = j5.get<std::deque<std::string>>();
}

TEST_F(JsonArrayConversionTest, StdSet)
{
    auto m1 = j1.get<std::set<int>>();
    auto m2 = j2.get<std::set<unsigned int>>();
    auto m3 = j3.get<std::set<double>>();
    auto m4 = j4.get<std::set<bool>>();
    auto m5 = j5.get<std::set<std::string>>();
}

TEST_F(JsonArrayConversionTest, StdUnorderedSet)
{
    auto m1 = j1.get<std::unordered_set<int>>();
    auto m2 = j2.get<std::unordered_set<unsigned int>>();
    auto m3 = j3.get<std::unordered_set<double>>();
    auto m4 = j4.get<std::unordered_set<bool>>();
    auto m5 = j5.get<std::unordered_set<std::string>>();
}

// exception in case of a non-object type
TEST_F(JsonArrayConversionTest, Exception)
{
    EXPECT_THROW_MSG((json().get<std::list<int>>()), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
    EXPECT_THROW_MSG((json().get<std::vector<int>>()), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
    EXPECT_THROW_MSG((json().get<std::vector<json>>()), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
    EXPECT_THROW_MSG((json().get<std::list<json>>()), json::type_error,
                     "[json.exception.type_error.302] type must be array, but is null");
    // does type really must be an array? or it rather must not be null?
    // that's what I thought when other test like this one broke
}
