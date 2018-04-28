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

TEST(JsonCopyConstructorTest, Object)
{
    json j {{"foo", 1}, {"bar", false}};
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Array)
{
    json j {"foo", 1, 42.23, false};
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Null)
{
    json j(nullptr);
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Boolean)
{
    json j(true);
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, String)
{
    json j("Hello world");
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Integer)
{
    json j(42);
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Unsigned)
{
    json j(42u);
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonCopyConstructorTest, Float)
{
    json j(42.23);
    json k(j);
    EXPECT_EQ(j, k);
}

TEST(JsonMoveConstructorTest, Case)
{
    json j {{"foo", "bar"}, {"baz", {1, 2, 3, 4}}, {"a", 42u}, {"b", 42.23}, {"c", nullptr}};
    EXPECT_EQ(j.type(), json::value_t::object);
    json k(std::move(j));
    EXPECT_EQ(k.type(), json::value_t::object);
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonCopyAssignmentTest, Object)
{
    json j {{"foo", 1}, {"bar", false}};
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Array)
{
    json j {"foo", 1, 42.23, false};
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Null)
{
    json j(nullptr);
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Boolean)
{
    json j(true);
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, String)
{
    json j("Hello world");
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Integer)
{
    json j(42);
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Unsigned)
{
    json j(42u);
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonCopyAssignmentTest, Float)
{
    json j(42.23);
    json k;
    k = j;
    EXPECT_EQ(j, k);
}

TEST(JsonDestructorTest, Object)
{
    auto j = new json {{"foo", 1}, {"bar", false}};
    delete j;
}

TEST(JsonDestructorTest, Array)
{
    auto j = new json {"foo", 1, 1u, false, 23.42};
    delete j;
}

TEST(JsonDestructorTest, String)
{
    auto j = new json("Hello world");
    delete j;
}
