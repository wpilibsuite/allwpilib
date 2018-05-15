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

TEST(JsonIteratorBasicTest, Uninitialized)
{
    json::iterator it;
    EXPECT_EQ(JsonTest::GetObject(it), nullptr);

    json::const_iterator cit;
    EXPECT_EQ(JsonTest::GetObject(cit), nullptr);
}

class JsonIteratorBooleanTest : public ::testing::Test {
 public:
    JsonIteratorBooleanTest() : j_const(j) {}

 protected:
    json j = true;
    json j_const;
};

TEST_F(JsonIteratorBooleanTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    it--;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    --it;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorBooleanTest, ConstBeginEnd)
{
    json::const_iterator it = j_const.begin();
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    it--;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    --it;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);
}

TEST_F(JsonIteratorBooleanTest, CBeginEnd)
{
    json::const_iterator it = j.cbegin();
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    it--;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    --it;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorBooleanTest, ConstCBeginEnd)
{
    json::const_iterator it = j_const.cbegin();
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    it--;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    --it;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);
}
#if 0
TEST_F(JsonIteratorBooleanTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    it--;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    --it;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorBooleanTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    it--;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    --it;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorBooleanTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    it--;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    --it;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);
}
#endif
TEST_F(JsonIteratorBooleanTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json(true));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json(true));
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

class JsonIteratorStringTest : public ::testing::Test {
 public:
    JsonIteratorStringTest() : j_const(j) {}

 protected:
    json j = "hello world";
    json j_const;
};

TEST_F(JsonIteratorStringTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    it--;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    --it;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorStringTest, ConstBeginEnd)
{
    json::const_iterator it = j_const.begin();
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    it--;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    --it;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);
}

TEST_F(JsonIteratorStringTest, CBeginEnd)
{
    json::const_iterator it = j.cbegin();
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    it--;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    --it;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorStringTest, ConstCBeginEnd)
{
    json::const_iterator it = j_const.cbegin();
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    it--;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    --it;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);
}
#if 0
TEST_F(JsonIteratorStringTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    it--;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    --it;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorStringTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    it--;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    --it;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorStringTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    it--;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    --it;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);
}
#endif
TEST_F(JsonIteratorStringTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json("hello world"));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json("hello world"));
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

class JsonIteratorArrayTest : public ::testing::Test {
 public:
    JsonIteratorArrayTest() : j_const(j) {}

 protected:
    json j = {1, 2, 3};
    json j_const;
};

TEST_F(JsonIteratorArrayTest, BeginEnd)
{
    json::iterator it_begin = j.begin();
    json::iterator it_end = j.end();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorArrayTest, ConstBeginEnd)
{
    json::const_iterator it_begin = j_const.begin();
    json::const_iterator it_end = j_const.end();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const[0]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const[2]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorArrayTest, CBeginEnd)
{
    json::const_iterator it_begin = j.cbegin();
    json::const_iterator it_end = j.cend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorArrayTest, ConstCBeginEnd)
{
    json::const_iterator it_begin = j_const.cbegin();
    json::const_iterator it_end = j_const.cend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}
#if 0
TEST_F(JsonIteratorArrayTest, RBeginEnd)
{
    json::reverse_iterator it_begin = j.rbegin();
    json::reverse_iterator it_end = j.rend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorArrayTest, CRBeginEnd)
{
    json::const_reverse_iterator it_begin = j.crbegin();
    json::const_reverse_iterator it_end = j.crend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorArrayTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it_begin = j_const.crbegin();
    json::const_reverse_iterator it_end = j_const.crend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[2]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[1]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j[0]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}
#endif
TEST_F(JsonIteratorArrayTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json(1));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json(1));
}

class JsonIteratorObjectTest : public ::testing::Test {
 public:
    JsonIteratorObjectTest() : j_const(j) {}

 protected:
    json j = {{"A", 1}, {"B", 2}, {"C", 3}};
    json j_const;
};

TEST_F(JsonIteratorObjectTest, BeginEnd)
{
    json::iterator it_begin = j.begin();
    json::iterator it_end = j.end();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["A"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["C"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorObjectTest, ConstBeginEnd)
{
    json::const_iterator it_begin = j_const.begin();
    json::const_iterator it_end = j_const.end();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["A"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["C"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorObjectTest, CBeginEnd)
{
    json::const_iterator it_begin = j.cbegin();
    json::const_iterator it_end = j.cend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["A"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["C"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorObjectTest, ConstCBeginEnd)
{
    json::const_iterator it_begin = j_const.cbegin();
    json::const_iterator it_end = j_const.cend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["A"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j_const["C"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}
#if 0
TEST_F(JsonIteratorObjectTest, RBeginEnd)
{
    json::reverse_iterator it_begin = j.rbegin();
    json::reverse_iterator it_end = j.rend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["C"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["A"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorObjectTest, CRBeginEnd)
{
    json::const_reverse_iterator it_begin = j.crbegin();
    json::const_reverse_iterator it_end = j.crend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["C"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["A"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}

TEST_F(JsonIteratorObjectTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it_begin = j_const.crbegin();
    json::const_reverse_iterator it_end = j_const.crend();

    auto it = it_begin;
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["C"]);

    it++;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["B"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_NE(it, it_end);
    EXPECT_EQ(*it, j["A"]);

    ++it;
    EXPECT_NE(it, it_begin);
    EXPECT_EQ(it, it_end);
}
#endif

TEST_F(JsonIteratorObjectTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_EQ(it.key(), "A");
    EXPECT_EQ(it.value(), json(1));
    EXPECT_EQ(cit.key(), "A");
    EXPECT_EQ(cit.value(), json(1));
}

class JsonIteratorIntegerTest : public ::testing::Test {
 public:
    JsonIteratorIntegerTest() : j_const(j) {}

 protected:
    json j = 23;
    json j_const;
};

TEST_F(JsonIteratorIntegerTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    it--;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    --it;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorIntegerTest, ConstBeginEnd)
{
    json::const_iterator it = j_const.begin();
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    it--;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    --it;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);
}

TEST_F(JsonIteratorIntegerTest, CBeginEnd)
{
    json::const_iterator it = j.cbegin();
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    it--;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    --it;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorIntegerTest, ConstCBeginEnd)
{
    json::const_iterator it = j_const.cbegin();
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    it--;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    --it;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);
}
#if 0
TEST_F(JsonIteratorIntegerTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    it--;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    --it;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorIntegerTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    it--;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    --it;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorIntegerTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    it--;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    --it;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);
}
#endif
TEST_F(JsonIteratorIntegerTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json(23));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json(23));
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

class JsonIteratorUnsignedTest : public ::testing::Test {
 public:
    JsonIteratorUnsignedTest() : j_const(j) {}

 protected:
    json j = 23u;
    json j_const;
};

TEST_F(JsonIteratorUnsignedTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    it--;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    --it;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorUnsignedTest, ConstBeginEnd)
{
    json::const_iterator it = j_const.begin();
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    it--;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    --it;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);
}

TEST_F(JsonIteratorUnsignedTest, CBeginEnd)
{
    json::const_iterator it = j.cbegin();
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    it--;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    --it;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorUnsignedTest, ConstCBeginEnd)
{
    json::const_iterator it = j_const.cbegin();
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    it--;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    --it;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);
}
#if 0
TEST_F(JsonIteratorUnsignedTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    it--;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    --it;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorUnsignedTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    it--;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    --it;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorUnsignedTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    it--;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    --it;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);
}
#endif
TEST_F(JsonIteratorUnsignedTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json(23));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json(23));
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

class JsonIteratorFloatTest : public ::testing::Test {
 public:
    JsonIteratorFloatTest() : j_const(j) {}

 protected:
    json j = 23.42;
    json j_const;
};

TEST_F(JsonIteratorFloatTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    it--;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.begin());
    EXPECT_EQ(it, j.end());

    --it;
    EXPECT_EQ(it, j.begin());
    EXPECT_NE(it, j.end());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorFloatTest, ConstBeginEnd)
{
    json::const_iterator it = j_const.begin();
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    it--;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.begin());
    EXPECT_EQ(it, j_const.end());

    --it;
    EXPECT_EQ(it, j_const.begin());
    EXPECT_NE(it, j_const.end());
    EXPECT_EQ(*it, j_const);
}

TEST_F(JsonIteratorFloatTest, CBeginEnd)
{
    json::const_iterator it = j.cbegin();
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    it--;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.cbegin());
    EXPECT_EQ(it, j.cend());

    --it;
    EXPECT_EQ(it, j.cbegin());
    EXPECT_NE(it, j.cend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorFloatTest, ConstCBeginEnd)
{
    json::const_iterator it = j_const.cbegin();
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    it--;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.cbegin());
    EXPECT_EQ(it, j_const.cend());

    --it;
    EXPECT_EQ(it, j_const.cbegin());
    EXPECT_NE(it, j_const.cend());
    EXPECT_EQ(*it, j_const);
}
#if 0
TEST_F(JsonIteratorFloatTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    it--;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.rbegin());
    EXPECT_EQ(it, j.rend());

    --it;
    EXPECT_EQ(it, j.rbegin());
    EXPECT_NE(it, j.rend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorFloatTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    it++;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    it--;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);

    ++it;
    EXPECT_NE(it, j.crbegin());
    EXPECT_EQ(it, j.crend());

    --it;
    EXPECT_EQ(it, j.crbegin());
    EXPECT_NE(it, j.crend());
    EXPECT_EQ(*it, j);
}

TEST_F(JsonIteratorFloatTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    it++;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    it--;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);

    ++it;
    EXPECT_NE(it, j_const.crbegin());
    EXPECT_EQ(it, j_const.crend());

    --it;
    EXPECT_EQ(it, j_const.crbegin());
    EXPECT_NE(it, j_const.crend());
    EXPECT_EQ(*it, j_const);
}
#endif
TEST_F(JsonIteratorFloatTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(it.value(), json(23.42));
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_EQ(cit.value(), json(23.42));
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

class JsonIteratorNullTest : public ::testing::Test {
 public:
    JsonIteratorNullTest() : j_const(j) {}

 protected:
    json j = nullptr;
    json j_const;
};

TEST_F(JsonIteratorNullTest, BeginEnd)
{
    json::iterator it = j.begin();
    EXPECT_EQ(it, j.end());
}

TEST_F(JsonIteratorNullTest, ConstBeginEnd)
{
    json::const_iterator it_begin = j_const.begin();
    json::const_iterator it_end = j_const.end();
    EXPECT_EQ(it_begin, it_end);
}

TEST_F(JsonIteratorNullTest, CBeginEnd)
{
    json::const_iterator it_begin = j.cbegin();
    json::const_iterator it_end = j.cend();
    EXPECT_EQ(it_begin, it_end);
}

TEST_F(JsonIteratorNullTest, ConstCBeginEnd)
{
    json::const_iterator it_begin = j_const.cbegin();
    json::const_iterator it_end = j_const.cend();
    EXPECT_EQ(it_begin, it_end);
}
#if 0
TEST_F(JsonIteratorNullTest, RBeginEnd)
{
    json::reverse_iterator it = j.rbegin();
    EXPECT_EQ(it, j.rend());
}

TEST_F(JsonIteratorNullTest, CRBeginEnd)
{
    json::const_reverse_iterator it = j.crbegin();
    EXPECT_EQ(it, j.crend());
}

TEST_F(JsonIteratorNullTest, ConstCRBeginEnd)
{
    json::const_reverse_iterator it = j_const.crbegin();
    EXPECT_EQ(it, j_const.crend());
}
#endif
TEST_F(JsonIteratorNullTest, KeyValue)
{
    auto it = j.begin();
    auto cit = j_const.cbegin();
    EXPECT_THROW_MSG(it.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(it.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(cit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(cit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#if 0
    auto rit = j.rend();
    auto crit = j.crend();
    EXPECT_THROW_MSG(rit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(rit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(crit.key(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.207] cannot use key() for non-object iterators");
    EXPECT_THROW_MSG(crit.value(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
#endif
}

TEST(JsonIteratorConstConversionTest, Boolean)
{
    json j = true;
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, String)
{
    json j = "hello world";
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Array)
{
    json j = {1, 2, 3};
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Object)
{
    json j = {{"A", 1}, {"B", 2}, {"C", 3}};
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Integer)
{
    json j = 23;
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Unsigned)
{
    json j = 23u;
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Float)
{
    json j = 23.42;
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}

TEST(JsonIteratorConstConversionTest, Null)
{
    json j = nullptr;
    json::const_iterator it = j.begin();
    EXPECT_EQ(it, j.cbegin());
    it = j.begin();
    EXPECT_EQ(it, j.cbegin());
}
