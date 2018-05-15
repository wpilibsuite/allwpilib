/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
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

#include <fstream>

TEST(MessagePackDiscardedTest, Case)
{
    // discarded values are not serialized
    json j = json::value_t::discarded;
    const auto result = json::to_msgpack(j);
    EXPECT_TRUE(result.empty());
}

TEST(MessagePackNullTest, Case)
{
    json j = nullptr;
    std::vector<uint8_t> expected = {0xc0};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

TEST(MessagePackBooleanTest, True)
{
    json j = true;
    std::vector<uint8_t> expected = {0xc3};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

TEST(MessagePackBooleanTest, False)
{
    json j = false;
    std::vector<uint8_t> expected = {0xc2};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// -32..-1 (negative fixnum)
TEST(MessagePackSignedTest, Neg0)
{
    for (auto i = -32; i <= -1; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(static_cast<int8_t>(result[0]), i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 0..127 (positive fixnum)
TEST(MessagePackSignedTest, Pos0)
{
    for (size_t i = 0; i <= 127; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(i));

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 128..255 (int 8)
TEST(MessagePackSignedTest, Pos1)
{
    for (size_t i = 128; i <= 255; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xcc));
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xcc));
        uint8_t restored = static_cast<uint8_t>(result[1]);
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 256..65535 (int 16)
TEST(MessagePackSignedTest, Pos2)
{
    for (size_t i = 256; i <= 65535; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xcd));
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xcd));
        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 65536..4294967295 (int 32)
class MessagePackSignedPos4Test : public ::testing::TestWithParam<uint32_t> {};
TEST_P(MessagePackSignedPos4Test, Case)
{
    // create JSON value with integer number
    json j = -1;
    j.get_ref<int64_t&>() = static_cast<int64_t>(GetParam());

    // check type
    EXPECT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xce));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xce));
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static const uint32_t pos4_numbers[] = {
    65536u,
    77777u,
    1048576u,
    4294967295u,
};

INSTANTIATE_TEST_CASE_P(MessagePackSignedPos4Tests, MessagePackSignedPos4Test,
                        ::testing::ValuesIn(pos4_numbers), );

// 4294967296..9223372036854775807 (int 64)
class MessagePackSignedPos8Test : public ::testing::TestWithParam<uint64_t> {};
TEST_P(MessagePackSignedPos8Test, Case)
{
    // create JSON value with integer number
    json j = -1;
    j.get_ref<int64_t&>() =
        static_cast<int64_t>(GetParam());

    // check type
    EXPECT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xcf));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 070) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 060) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 050) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 040) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 030) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 020) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 010) & 0xff));
    expected.push_back(static_cast<char>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xcf));
    uint64_t restored = (static_cast<uint64_t>(static_cast<uint8_t>(result[1])) << 070) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[2])) << 060) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[3])) << 050) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[4])) << 040) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[5])) << 030) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[6])) << 020) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[7])) << 010) +
                        static_cast<uint64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static const uint64_t pos8_numbers[] = {
    4294967296lu,
    9223372036854775807lu,
};

INSTANTIATE_TEST_CASE_P(MessagePackSignedPos8Tests, MessagePackSignedPos8Test,
                        ::testing::ValuesIn(pos8_numbers), );

// -128..-33 (int 8)
TEST(MessagePackSignedTest, Neg1)
{
    for (auto i = -128; i <= -33; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xd0));
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xd0));
        EXPECT_EQ(static_cast<int8_t>(result[1]), i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// -32768..-129 (int 16)
TEST(MessagePackSignedTest, Neg2)
{
    for (int16_t i = -32768; i <= -129; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xd1));
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xd1));
        int16_t restored = static_cast<int16_t>((static_cast<uint8_t>(result[1]) << 8) +
                                                static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// -32769..-2147483648
class MessagePackSignedNeg4Test : public ::testing::TestWithParam<int32_t> {};
TEST_P(MessagePackSignedNeg4Test, Case)
{
    // create JSON value with integer number
    json j = GetParam();

    // check type
    EXPECT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xd2));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xd2));
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(static_cast<int32_t>(restored), GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static const int32_t neg4_numbers[] = {
    -32769,
    -65536,
    -77777,
    -1048576,
    -2147483648ll,
};

INSTANTIATE_TEST_CASE_P(MessagePackSignedNeg4Tests, MessagePackSignedNeg4Test,
                        ::testing::ValuesIn(neg4_numbers), );

// -9223372036854775808..-2147483649 (int 64)
class MessagePackSignedNeg8Test : public ::testing::TestWithParam<int64_t> {};
TEST_P(MessagePackSignedNeg8Test, Case)
{
    // create JSON value with unsigned integer number
    json j = GetParam();

    // check type
    EXPECT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xd3));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 070) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 060) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 050) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 040) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 030) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 020) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 010) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xd3));
    int64_t restored = (static_cast<int64_t>(static_cast<uint8_t>(result[1])) << 070) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[2])) << 060) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[3])) << 050) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[4])) << 040) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[5])) << 030) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[6])) << 020) +
                       (static_cast<int64_t>(static_cast<uint8_t>(result[7])) << 010) +
                       static_cast<int64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static const int64_t neg8_numbers[] = {
    INT64_MIN,
    -2147483649ll,
};

INSTANTIATE_TEST_CASE_P(MessagePackSignedNeg8Tests, MessagePackSignedNeg8Test,
                        ::testing::ValuesIn(neg8_numbers), );

// 0..127 (positive fixnum)
TEST(MessagePackUnsignedTest, Pos0)
{
    for (size_t i = 0; i <= 127; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(i));

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 128..255 (uint 8)
TEST(MessagePackUnsignedTest, Pos1)
{
    for (size_t i = 128; i <= 255; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xcc));
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xcc));
        uint8_t restored = static_cast<uint8_t>(result[1]);
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 256..65535 (uint 16)
TEST(MessagePackUnsignedTest, Pos2)
{
    for (size_t i = 256; i <= 65535; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        EXPECT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xcd));
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(0xcd));
        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// 65536..4294967295 (uint 32)
class MessagePackUnsignedPos4Test : public ::testing::TestWithParam<uint32_t> {};
TEST_P(MessagePackUnsignedPos4Test, Case)
{
    // create JSON value with unsigned integer number
    json j = GetParam();

    // check type
    EXPECT_TRUE(j.is_number_unsigned());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xce));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xce));
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

INSTANTIATE_TEST_CASE_P(MessagePackUnsignedPos4Tests,
                        MessagePackUnsignedPos4Test,
                        ::testing::ValuesIn(pos4_numbers), );

// 4294967296..18446744073709551615 (uint 64)
class MessagePackUnsignedPos8Test : public ::testing::TestWithParam<uint64_t> {};
TEST_P(MessagePackUnsignedPos8Test, Case)
{
    // create JSON value with unsigned integer number
    json j = GetParam();

    // check type
    EXPECT_TRUE(j.is_number_unsigned());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xcf));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 070) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 060) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 050) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 040) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 030) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 020) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 010) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xcf));
    uint64_t restored = (static_cast<uint64_t>(static_cast<uint8_t>(result[1])) << 070) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[2])) << 060) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[3])) << 050) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[4])) << 040) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[5])) << 030) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[6])) << 020) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[7])) << 010) +
                        static_cast<uint64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

INSTANTIATE_TEST_CASE_P(MessagePackUnsignedPos8Tests,
                        MessagePackUnsignedPos8Test,
                        ::testing::ValuesIn(pos8_numbers), );

// 3.1415925
TEST(MessagePackFloatTest, Number)
{
    double v = 3.1415925;
    json j = v;
    std::vector<uint8_t> expected = {0xcb,0x40,0x09,0x21,0xfb,0x3f,0xa6,0xde,0xfc};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
    EXPECT_EQ(json::from_msgpack(result), v);
}

// N = 0..31
TEST(MessagePackStringTest, String1)
{
    // explicitly enumerate the first byte for all 32 strings
    const std::vector<uint8_t> first_bytes =
    {
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
        0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1,
        0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
        0xbb, 0xbc, 0xbd, 0xbe, 0xbf
    };

    for (size_t N = 0; N < first_bytes.size(); ++N)
    {
        SCOPED_TRACE(N);

        // create JSON value with string containing of N * 'x'
        const auto s = std::string(N, 'x');
        json j = s;

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(first_bytes[N]));
        for (size_t i = 0; i < N; ++i)
        {
            expected.push_back('x');
        }

        // check first byte
        EXPECT_EQ((first_bytes[N] & 0x1f), static_cast<uint8_t>(N));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), N + 1);
        // check that no null byte is appended
        if (N > 0)
        {
            EXPECT_NE(result.back(), '\x00');
        }

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// N = 32..255
TEST(MessagePackStringTest, String2)
{
    for (size_t N = 32; N <= 255; ++N)
    {
        SCOPED_TRACE(N);

        // create JSON value with string containing of N * 'x'
        const auto s = std::string(N, 'x');
        json j = s;

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0xd9));
        expected.push_back(static_cast<uint8_t>(N));
        for (size_t i = 0; i < N; ++i)
        {
            expected.push_back('x');
        }

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result, expected);
        EXPECT_EQ(result.size(), N + 2);
        // check that no null byte is appended
        EXPECT_NE(result.back(), '\x00');

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}

// N = 256..65535
class MessagePackString3Test : public ::testing::TestWithParam<size_t> {};
TEST_P(MessagePackString3Test, Case)
{
    // create JSON value with string containing of N * 'x'
    const auto s = std::string(GetParam(), 'x');
    json j = s;

    // create expected byte vector (hack: create string first)
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xda));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));
    for (size_t i = 0; i < GetParam(); ++i)
    {
        expected.push_back('x');
    }

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), GetParam() + 3);
    // check that no null byte is appended
    EXPECT_NE(result.back(), '\x00');

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static size_t string3_lens[] = {
    256u,
    999u,
    1025u,
    3333u,
    2048u,
    65535u
};

INSTANTIATE_TEST_CASE_P(MessagePackString3Tests, MessagePackString3Test,
                        ::testing::ValuesIn(string3_lens), );


// N = 65536..4294967295
class MessagePackString5Test : public ::testing::TestWithParam<size_t> {};
TEST_P(MessagePackString5Test, Case)
{
    // create JSON value with string containing of N * 'x'
    const auto s = std::string(GetParam(), 'x');
    json j = s;

    // create expected byte vector (hack: create string first)
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0xdb));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));
    for (size_t i = 0; i < GetParam(); ++i)
    {
        expected.push_back('x');
    }

    // compare result + size
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);
    EXPECT_EQ(result.size(), GetParam() + 5);
    // check that no null byte is appended
    EXPECT_NE(result.back(), '\x00');

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

static size_t string5_lens[] = {
    65536u,
    77777u,
    1048576u
};

INSTANTIATE_TEST_CASE_P(MessagePackString5Tests, MessagePackString5Test,
                        ::testing::ValuesIn(string5_lens), );

TEST(MessagePackArrayTest, Empty)
{
    json j = json::array();
    std::vector<uint8_t> expected = {0x90};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// [null]
TEST(MessagePackArrayTest, Null)
{
    json j = {nullptr};
    std::vector<uint8_t> expected = {0x91,0xc0};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// [1,2,3,4,5]
TEST(MessagePackArrayTest, Simple)
{
    json j = json::parse("[1,2,3,4,5]");
    std::vector<uint8_t> expected = {0x95,0x01,0x02,0x03,0x04,0x05};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// [[[[]]]]
TEST(MessagePackArrayTest, NestEmpty)
{
    json j = json::parse("[[[[]]]]");
    std::vector<uint8_t> expected = {0x91,0x91,0x91,0x90};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// array 16
TEST(MessagePackArrayTest, UInt16)
{
    json j(16, nullptr);
    std::vector<uint8_t> expected(j.size() + 3, static_cast<uint8_t>(0xc0)); // all null
    expected[0] = static_cast<uint8_t>(0xdc); // array 16
    expected[1] = 0x00; // size (0x0010), byte 0
    expected[2] = 0x10; // size (0x0010), byte 1
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// array 32
TEST(MessagePackArrayTest, UInt32)
{
    json j(65536, nullptr);
    std::vector<uint8_t> expected(j.size() + 5, static_cast<uint8_t>(0xc0)); // all null
    expected[0] = static_cast<uint8_t>(0xdd); // array 32
    expected[1] = 0x00; // size (0x00100000), byte 0
    expected[2] = 0x01; // size (0x00100000), byte 1
    expected[3] = 0x00; // size (0x00100000), byte 2
    expected[4] = 0x00; // size (0x00100000), byte 3
    const auto result = json::to_msgpack(j);
    //EXPECT_EQ(result, expected);

    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        SCOPED_TRACE(i);
        EXPECT_EQ(result[i], expected[i]);
    }

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

TEST(MessagePackObjectTest, Empty)
{
    json j = json::object();
    std::vector<uint8_t> expected = {0x80};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// {"":null}
TEST(MessagePackObjectTest, EmptyKey)
{
    json j = {{"", nullptr}};
    std::vector<uint8_t> expected = {0x81,0xa0,0xc0};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// {"a": {"b": {"c": {}}}}
TEST(MessagePackObjectTest, NestedEmpty)
{
    json j = json::parse("{\"a\": {\"b\": {\"c\": {}}}}");
    std::vector<uint8_t> expected = {0x81,0xa1,0x61,0x81,0xa1,0x62,0x81,0xa1,0x63,0x80};
    const auto result = json::to_msgpack(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// map 16
TEST(MessagePackObjectTest, UInt16)
{
    json j = R"({"00": null, "01": null, "02": null, "03": null,
                 "04": null, "05": null, "06": null, "07": null,
                 "08": null, "09": null, "10": null, "11": null,
                 "12": null, "13": null, "14": null, "15": null})"_json;

    const auto result = json::to_msgpack(j);

    // Checking against an expected vector byte by byte is
    // difficult, because no assumption on the order of key/value
    // pairs are made. We therefore only check the prefix (type and
    // size and the overall size. The rest is then handled in the
    // roundtrip check.
    EXPECT_EQ(result.size(), 67u); // 1 type, 2 size, 16*4 content
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xde)); // map 16
    EXPECT_EQ(result[1], 0x00); // byte 0 of size (0x0010)
    EXPECT_EQ(result[2], 0x10); // byte 1 of size (0x0010)

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// map 32
TEST(MessagePackObjectTest, UInt32)
{
    json j;
    for (auto i = 0; i < 65536; ++i)
    {
        // format i to a fixed width of 5
        // each entry will need 7 bytes: 6 for fixstr, 1 for null
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << i;
        j.emplace(ss.str(), nullptr);
    }

    const auto result = json::to_msgpack(j);

    // Checking against an expected vector byte by byte is
    // difficult, because no assumption on the order of key/value
    // pairs are made. We therefore only check the prefix (type and
    // size and the overall size. The rest is then handled in the
    // roundtrip check.
    EXPECT_EQ(result.size(), 458757u); // 1 type, 4 size, 65536*7 content
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xdf)); // map 32
    EXPECT_EQ(result[1], 0x00); // byte 0 of size (0x00010000)
    EXPECT_EQ(result[2], 0x01); // byte 1 of size (0x00010000)
    EXPECT_EQ(result[3], 0x00); // byte 2 of size (0x00010000)
    EXPECT_EQ(result[4], 0x00); // byte 3 of size (0x00010000)

    // roundtrip
    EXPECT_EQ(json::from_msgpack(result), j);
}

// from float32
TEST(MessagePackFloat32Test, Case)
{
    auto given = std::vector<uint8_t>({0xca,0x41,0xc8,0x00,0x01});
    json j = json::from_msgpack(given);
    EXPECT_LT(std::fabs(j.get<double>() - 25), 0.001);
}

TEST(MessagePackErrorTest, TooShortByteVector)
{
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcc})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcd})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcd,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xce})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xce,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xce,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 4: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xce,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 5: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 4: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 5: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 6: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 7: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 8: unexpected end of input");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 9: unexpected end of input");
}

TEST(MessagePackErrorTest, UnsupportedBytesConcrete)
{
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xc1})), json::parse_error,
                     "[json.exception.parse_error.112] parse error at 1: error reading MessagePack; last byte: 0xc1");
    EXPECT_THROW_MSG(json::from_msgpack(std::vector<uint8_t>({0xc6})), json::parse_error,
                     "[json.exception.parse_error.112] parse error at 1: error reading MessagePack; last byte: 0xc6");
}

TEST(MessagePackErrorTest, UnsupportedBytesAll)
{
    for (auto byte :
            {
                // never used
                0xc1,
                // bin
                0xc4, 0xc5, 0xc6,
                // ext
                0xc7, 0xc8, 0xc9,
                // fixext
                0xd4, 0xd5, 0xd6, 0xd7, 0xd8
            })
    {
        EXPECT_THROW(json::from_msgpack(std::vector<uint8_t>({static_cast<uint8_t>(byte)})), json::parse_error);
    }
}
#if 0
// use this testcase outside [hide] to run it with Valgrind
TEST(MessagePackRoundtripTest, Sample)
{
    std::string filename = "test/data/json_testsuite/sample.json";

    // parse JSON file
    std::ifstream f_json(filename);
    json j1 = json::parse(f_json);

    // parse MessagePack file
    std::ifstream f_msgpack(filename + ".msgpack", std::ios::binary);
    std::vector<uint8_t> packed((std::istreambuf_iterator<char>(f_msgpack)),
                                std::istreambuf_iterator<char>());
    json j2;
    EXPECT_NO_THROW(j2 = json::from_msgpack(packed));

    // compare parsed JSON values
    EXPECT_EQ(j1, j2);

    // check with different start index
    packed.insert(packed.begin(), 5, 0xff);
    EXPECT_EQ(j1, json::from_msgpack(packed, 5));
}

class MessagePackRegressionTest : public ::testing::TestWithParam<const char*> {};
TEST_P(MessagePackRegressionTest, Case)
{
    // parse JSON file
    std::ifstream f_json(GetParam());
    json j1 = json::parse(f_json);

    // parse MessagePack file
    std::ifstream f_msgpack(filename + ".msgpack", std::ios::binary);
    std::vector<uint8_t> packed((std::istreambuf_iterator<char>(f_msgpack)),
                                std::istreambuf_iterator<char>());
    json j2;
    EXPECT_NO_THROW(j2 = json::from_msgpack(packed));

    // compare parsed JSON values
    EXPECT_EQ(j1, j2);
}

static const char* regression_test_cases[] = {
    "test/data/json_nlohmann_tests/all_unicode.json",
    "test/data/json.org/1.json",
    "test/data/json.org/2.json",
    "test/data/json.org/3.json",
    "test/data/json.org/4.json",
    "test/data/json.org/5.json",
    "test/data/json_roundtrip/roundtrip01.json",
    "test/data/json_roundtrip/roundtrip02.json",
    "test/data/json_roundtrip/roundtrip03.json",
    "test/data/json_roundtrip/roundtrip04.json",
    "test/data/json_roundtrip/roundtrip05.json",
    "test/data/json_roundtrip/roundtrip06.json",
    "test/data/json_roundtrip/roundtrip07.json",
    "test/data/json_roundtrip/roundtrip08.json",
    "test/data/json_roundtrip/roundtrip09.json",
    "test/data/json_roundtrip/roundtrip10.json",
    "test/data/json_roundtrip/roundtrip11.json",
    "test/data/json_roundtrip/roundtrip12.json",
    "test/data/json_roundtrip/roundtrip13.json",
    "test/data/json_roundtrip/roundtrip14.json",
    "test/data/json_roundtrip/roundtrip15.json",
    "test/data/json_roundtrip/roundtrip16.json",
    "test/data/json_roundtrip/roundtrip17.json",
    "test/data/json_roundtrip/roundtrip18.json",
    "test/data/json_roundtrip/roundtrip19.json",
    "test/data/json_roundtrip/roundtrip20.json",
    "test/data/json_roundtrip/roundtrip21.json",
    "test/data/json_roundtrip/roundtrip22.json",
    "test/data/json_roundtrip/roundtrip23.json",
    "test/data/json_roundtrip/roundtrip24.json",
    "test/data/json_roundtrip/roundtrip25.json",
    "test/data/json_roundtrip/roundtrip26.json",
    "test/data/json_roundtrip/roundtrip27.json",
    "test/data/json_roundtrip/roundtrip28.json",
    "test/data/json_roundtrip/roundtrip29.json",
    "test/data/json_roundtrip/roundtrip30.json",
    "test/data/json_roundtrip/roundtrip31.json",
    "test/data/json_roundtrip/roundtrip32.json",
    "test/data/json_testsuite/sample.json", // kills AppVeyor
    "test/data/json_tests/pass1.json",
    "test/data/json_tests/pass2.json",
    "test/data/json_tests/pass3.json",
    "test/data/regression/floats.json",
    "test/data/regression/signed_ints.json",
    "test/data/regression/unsigned_ints.json",
    "test/data/regression/working_file.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_arraysWithSpaces.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_empty-string.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_ending_with_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_false.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_heterogeneous.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_1_and_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_leading_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_several_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_trailing_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_number.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_0e+1.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_0e1.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_after_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_double_close_to_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_double_huge_neg_exp.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_huge_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_int_with_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_minus_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_one.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_neg_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_pos_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_exponent.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_fraction_exponent.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_neg_exp.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_real_neg_overflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_pos_exponent.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_real_pos_overflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_underflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_simple_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_simple_real.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_neg_int.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_pos_int.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_very_big_negative_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_object.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_basic.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key_and_value.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_empty_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_escaped_null_in_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_extreme_numbers.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_long_strings.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_simple.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_string_unicode.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_with_newlines.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_UTF-16_Surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pair.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pairs.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_allowed_escapes.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_and_u_escaped_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_doublequotes.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_comments.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_a.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_n.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_control_character.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_noncharacter.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_in_array.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_in_array_with_leading_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_last_surrogates_1_and_2.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_newline_uescaped.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+1FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_null_escape.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_one-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_pi.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_simple_ascii.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_three-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_two-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_u+2028_line_sep.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_u+2029_par_sep.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_uEscape.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unescaped_char_delete.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicodeEscapedBackslash.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_2.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+2064_invisible_plus.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_escaped_double_quote.json",
    // "test/data/nst_json_testsuite/test_parsing/y_string_utf16.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_utf8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_with_del_character.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_false.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_negative_real.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_string.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_true.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_string_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_trailing_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_true_in_array.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_whitespace_array.json",
};

INSTANTIATE_TEST_CASE_P(MessagePackRegressionTests, MessagePackRegressionTest,
                        ::testing::ValuesIn(regression_test_cases));
#endif
