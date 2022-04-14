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

// reference access to object_t
TEST(JsonReferenceTest, ObjectT)
{
    using test_type = json::object_t;
    json value = {{"one", 1}, {"two", 2}};

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_NO_THROW(value.get_ref<json::object_t&>());
    EXPECT_ANY_THROW(value.get_ref<json::array_t&>());
    EXPECT_ANY_THROW(value.get_ref<std::string&>());
    EXPECT_ANY_THROW(value.get_ref<bool&>());
    EXPECT_ANY_THROW(value.get_ref<int64_t&>());
    EXPECT_ANY_THROW(value.get_ref<double&>());
}

// const reference access to const object_t
TEST(JsonReferenceTest, ConstObjectT)
{
    using test_type = json::object_t;
    const json value = {{"one", 1}, {"two", 2}};

    // this should not compile
    // test_type& p1 = value.get_ref<test_type&>();

    // check if references are returned correctly
    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());
}

// reference access to array_t
TEST(JsonReferenceTest, ArrayT)
{
    using test_type = json::array_t;
    json value = {1, 2, 3, 4};

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_ANY_THROW(value.get_ref<json::object_t&>());
    EXPECT_NO_THROW(value.get_ref<json::array_t&>());
    EXPECT_ANY_THROW(value.get_ref<std::string&>());
    EXPECT_ANY_THROW(value.get_ref<bool&>());
    EXPECT_ANY_THROW(value.get_ref<int64_t&>());
    EXPECT_ANY_THROW(value.get_ref<double&>());
}

// reference access to string_t
TEST(JsonReferenceTest, StringT)
{
    using test_type = std::string;
    json value = "hello";

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_ANY_THROW(value.get_ref<json::object_t&>());
    EXPECT_ANY_THROW(value.get_ref<json::array_t&>());
    EXPECT_NO_THROW(value.get_ref<std::string&>());
    EXPECT_ANY_THROW(value.get_ref<bool&>());
    EXPECT_ANY_THROW(value.get_ref<int64_t&>());
    EXPECT_ANY_THROW(value.get_ref<double&>());
}

// reference access to boolean_t
TEST(JsonReferenceTest, BooleanT)
{
    using test_type = bool;
    json value = false;

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_ANY_THROW(value.get_ref<json::object_t&>());
    EXPECT_ANY_THROW(value.get_ref<json::array_t&>());
    EXPECT_ANY_THROW(value.get_ref<std::string&>());
    EXPECT_NO_THROW(value.get_ref<bool&>());
    EXPECT_ANY_THROW(value.get_ref<int64_t&>());
    EXPECT_ANY_THROW(value.get_ref<double&>());
}

// reference access to number_integer_t
TEST(JsonReferenceTest, IntegerT)
{
    using test_type = int64_t;
    json value = 23;

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_ANY_THROW(value.get_ref<json::object_t&>());
    EXPECT_ANY_THROW(value.get_ref<json::array_t&>());
    EXPECT_ANY_THROW(value.get_ref<std::string&>());
    EXPECT_ANY_THROW(value.get_ref<bool&>());
    EXPECT_NO_THROW(value.get_ref<int64_t&>());
    EXPECT_ANY_THROW(value.get_ref<double&>());
}

// reference access to number_float_t
TEST(JsonReferenceTest, FloatT)
{
    using test_type = double;
    json value = 42.23;

    // check if references are returned correctly
    test_type& p1 = value.get_ref<test_type&>();
    EXPECT_EQ(&p1, value.get_ptr<test_type*>());
    EXPECT_EQ(p1, value.get<test_type>());

    const test_type& p2 = value.get_ref<const test_type&>();
    EXPECT_EQ(&p2, value.get_ptr<const test_type*>());
    EXPECT_EQ(p2, value.get<test_type>());

    // check if mismatching references throw correctly
    EXPECT_ANY_THROW(value.get_ref<json::object_t&>());
    EXPECT_ANY_THROW(value.get_ref<json::array_t&>());
    EXPECT_ANY_THROW(value.get_ref<std::string&>());
    EXPECT_ANY_THROW(value.get_ref<bool&>());
    EXPECT_ANY_THROW(value.get_ref<int64_t&>());
    EXPECT_NO_THROW(value.get_ref<double&>());
}
