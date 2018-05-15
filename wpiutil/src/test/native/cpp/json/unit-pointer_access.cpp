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

TEST(JsonPointerTest, TypesCreate)
{
    // create a JSON value with different types
    json json_types =
    {
        {"boolean", true},
        {
            "number", {
                {"integer", 42},
                {"unsigned", 42u},
                {"floating-point", 17.23}
            }
        },
        {"string", "Hello, world!"},
        {"array", {1, 2, 3, 4, 5}},
        {"null", nullptr}
    };
}

// pointer access to object_t
TEST(JsonPointerTest, ObjectT)
{
    using test_type = json::object_t;
    json value = {{"one", 1}, {"two", 2}};

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_NE(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const object_t
TEST(JsonPointerTest, ConstObjectT)
{
    using test_type = const json::object_t;
    const json value = {{"one", 1}, {"two", 2}};

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_NE(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to array_t
TEST(JsonPointerTest, ArrayT)
{
    using test_type = json::array_t;
    json value = {1, 2, 3, 4};

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const array_t
TEST(JsonPointerTest, ConstArrayT)
{
    using test_type = const json::array_t;
    const json value = {1, 2, 3, 4};

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to string_t
TEST(JsonPointerTest, StringT)
{
    using test_type = std::string;
    json value = "hello";

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const string_t
TEST(JsonPointerTest, ConstStringT)
{
    using test_type = const std::string;
    const json value = "hello";

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to boolean_t
TEST(JsonPointerTest, BooleanT)
{
    using test_type = bool;
    json value = false;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_NE(value.get_ptr<bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const boolean_t
TEST(JsonPointerTest, ConstBooleanT)
{
    using test_type = const bool;
    const json value = false;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    //EXPECT_EQ(*p1, value.get<test_type>());

    //const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    //EXPECT_EQ(*p2, value.get<test_type>());

    //const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    //EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_NE(value.get_ptr<const bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to number_integer_t
TEST(JsonPointerTest, IntegerT)
{
    using test_type = int64_t;
    json value = 23;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_NE(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const number_integer_t
TEST(JsonPointerTest, ConstIntegerT)
{
    using test_type = const int64_t;
    const json value = 23;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_NE(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to number_unsigned_t
TEST(JsonPointerTest, UnsignedT)
{
    using test_type = uint64_t;
    json value = 23u;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_NE(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<double*>(), nullptr);
}

// pointer access to const number_unsigned_t
TEST(JsonPointerTest, ConstUnsignedT)
{
    using test_type = const uint64_t;
    const json value = 23u;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_EQ(*p1, value.get<test_type>());

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_EQ(*p2, value.get<test_type>());

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_EQ(*p3, value.get<test_type>());

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_NE(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const double*>(), nullptr);
}

// pointer access to number_float_t
TEST(JsonPointerTest, FloatT)
{
    using test_type = double;
    json value = 42.23;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_LT(std::fabs(*p1 - value.get<test_type>()), 0.001);

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_LT(std::fabs(*p2 - value.get<test_type>()), 0.001);

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_LT(std::fabs(*p3 - value.get<test_type>()), 0.001);

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<uint64_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<double*>(), nullptr);
}

// pointer access to const number_float_t
TEST(JsonPointerTest, ConstFloatT)
{
    using test_type = const double;
    const json value = 42.23;

    // check if pointers are returned correctly
    test_type* p1 = value.get_ptr<test_type*>();
    EXPECT_EQ(p1, value.get_ptr<test_type*>());
    EXPECT_LT(std::fabs(*p1 - value.get<test_type>()), 0.001);

    const test_type* p2 = value.get_ptr<const test_type*>();
    EXPECT_EQ(p1, value.get_ptr<const test_type*>());
    EXPECT_LT(std::fabs(*p2 - value.get<test_type>()), 0.001);

    const test_type* const p3 = value.get_ptr<const test_type* const>();
    EXPECT_EQ(p1, value.get_ptr<const test_type* const>());
    EXPECT_LT(std::fabs(*p3 - value.get<test_type>()), 0.001);

    // check if null pointers are returned correctly
    EXPECT_EQ(value.get_ptr<const json::object_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const json::array_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const std::string*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const bool*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const int64_t*>(), nullptr);
    EXPECT_EQ(value.get_ptr<const uint64_t*>(), nullptr);
    EXPECT_NE(value.get_ptr<const double*>(), nullptr);
}
