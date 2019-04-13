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

#include "wpi/Format.h"
#include "wpi/StringExtras.h"
#include "wpi/raw_ostream.h"

#include <fstream>

// create and check a JSON string with up to four UTF-8 bytes
::testing::AssertionResult check_utf8string(bool success_expected, int byte1, int byte2 = -1, int byte3 = -1, int byte4 = -1)
{
    std::string json_string = "\"";

    json_string += std::string(1, static_cast<char>(byte1));

    if (byte2 != -1)
    {
        json_string += std::string(1, static_cast<char>(byte2));
    }

    if (byte3 != -1)
    {
        json_string += std::string(1, static_cast<char>(byte3));
    }

    if (byte4 != -1)
    {
        json_string += std::string(1, static_cast<char>(byte4));
    }

    json_string += "\"";

    const char* basemsg = "";

    try {
        json::parse(json_string);
    } catch (json::parse_error&) {
        if (success_expected)
        {
            basemsg = "parse_error";
            goto error;
        }
        return ::testing::AssertionSuccess();
    } catch (...) {
        basemsg = "other exception";
        goto error;
    }

    if (success_expected)
    {
        return ::testing::AssertionSuccess();
    }
    basemsg = "expected failure";

error:
    auto result = ::testing::AssertionFailure();
    result << basemsg << " with {" << wpi::utohexstr(byte1);
    if (byte2 != -1)
    {
        result << ',' << wpi::utohexstr(byte2);
    }
    if (byte3 != -1)
    {
        result << ',' << wpi::utohexstr(byte3);
    }
    if (byte4 != -1)
    {
        result << ',' << wpi::utohexstr(byte4);
    }
    result << '}';
    return result;
}

/*
RFC 3629 describes in Sect. 4 the syntax of UTF-8 byte sequences as
follows:

    A UTF-8 string is a sequence of octets representing a sequence of UCS
    characters.  An octet sequence is valid UTF-8 only if it matches the
    following syntax, which is derived from the rules for encoding UTF-8
    and is expressed in the ABNF of [RFC2234].

    UTF8-octets = *( UTF8-char )
    UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
    UTF8-1      = %x00-7F
    UTF8-2      = %xC2-DF UTF8-tail
    UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) /
                  %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
    UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) /
                  %xF4 %x80-8F 2( UTF8-tail )
    UTF8-tail   = %x80-BF
*/

// ill-formed first byte
TEST(JsonUnicodeRfc3629Test, IllFormedFirstByte)
{
    for (int byte1 = 0x80; byte1 <= 0xC1; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }

    for (int byte1 = 0xF5; byte1 <= 0xFF; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// UTF8-1 (x00-x7F), well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_1WellFormed)
{
    for (int byte1 = 0x00; byte1 <= 0x7F; ++byte1)
    {
        // unescaped control characters are parse errors in JSON
        if (0x00 <= byte1 && byte1 <= 0x1F)
        {
            EXPECT_TRUE(check_utf8string(false, byte1));
            continue;
        }

        // a single quote is a parse error in JSON
        if (byte1 == 0x22)
        {
            EXPECT_TRUE(check_utf8string(false, byte1));
            continue;
        }

        // a single backslash is a parse error in JSON
        if (byte1 == 0x5C)
        {
            EXPECT_TRUE(check_utf8string(false, byte1));
            continue;
        }

        // all other characters are OK
        EXPECT_TRUE(check_utf8string(true, byte1));
    }
}

// UTF8-2 (xC2-xDF UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_2WellFormed)
{
    for (int byte1 = 0xC2; byte1 <= 0xDF; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(true, byte1, byte2));
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_2Missing2)
{
    for (int byte1 = 0xC2; byte1 <= 0xDF; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, Utf8_2Wrong2)
{
    for (int byte1 = 0xC2; byte1 <= 0xDF; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// UTF8-3 (xE0 xA0-BF UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_3AWellFormed)
{
    for (int byte1 = 0xE0; byte1 <= 0xE0; ++byte1)
    {
        for (int byte2 = 0xA0; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3AMissing2)
{
    for (int byte1 = 0xE0; byte1 <= 0xE0; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3AMissing3)
{
    for (int byte1 = 0xE0; byte1 <= 0xE0; ++byte1)
    {
        for (int byte2 = 0xA0; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3AWrong2)
{
    for (int byte1 = 0xE0; byte1 <= 0xE0; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0xA0 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3AWrong3)
{
    for (int byte1 = 0xE0; byte1 <= 0xE0; ++byte1)
    {
        for (int byte2 = 0xA0; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// UTF8-3 (xE1-xEC UTF8-tail UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_3BWellFormed)
{
    for (int byte1 = 0xE1; byte1 <= 0xEC; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3BMissing2)
{
    for (int byte1 = 0xE1; byte1 <= 0xEC; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3BMissing3)
{
    for (int byte1 = 0xE1; byte1 <= 0xEC; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_3BWrong2)
{
    for (int byte1 = 0xE1; byte1 <= 0xEC; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_3BWrong3)
{
    for (int byte1 = 0xE1; byte1 <= 0xEC; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// UTF8-3 (xED x80-9F UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_3CWellFormed)
{
    for (int byte1 = 0xED; byte1 <= 0xED; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x9F; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3CMissing2)
{
    for (int byte1 = 0xED; byte1 <= 0xED; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3CMissing3)
{
    for (int byte1 = 0xED; byte1 <= 0xED; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x9F; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3CWrong2)
{
    for (int byte1 = 0xED; byte1 <= 0xED; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0x9F)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3CWrong3)
{
    for (int byte1 = 0xED; byte1 <= 0xED; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x9F; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// UTF8-3 (xEE-xEF UTF8-tail UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_3DWellFormed)
{
    for (int byte1 = 0xEE; byte1 <= 0xEF; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3DMissing2)
{
    for (int byte1 = 0xEE; byte1 <= 0xEF; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3DMissing3)
{
    for (int byte1 = 0xEE; byte1 <= 0xEF; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, Utf8_3DWrong2)
{
    for (int byte1 = 0xEE; byte1 <= 0xEF; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, Utf8_3DWrong3)
{
    for (int byte1 = 0xEE; byte1 <= 0xEF; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// UTF8-4 (xF0 x90-BF UTF8-tail UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_4AWellFormed)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x90; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_4AMissing2)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_4AMissing3)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x90; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: missing fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4AMissing4)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x90; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4AWrong2)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x90 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4AWrong3)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x90; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4AWrong4)
{
    for (int byte1 = 0xF0; byte1 <= 0xF0; ++byte1)
    {
        for (int byte2 = 0x90; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x00; byte4 <= 0xFF; ++byte4)
                {
                    // skip fourth second byte
                    if (0x80 <= byte3 && byte3 <= 0xBF)
                    {
                        continue;
                    }

                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// UTF8-4 (xF1-F3 UTF8-tail UTF8-tail UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_4BWellFormed)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_4BMissing2)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_4BMissing3)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: missing fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4BMissing4)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4BWrong2)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0xBF)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4BWrong3)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4BWrong4)
{
    for (int byte1 = 0xF1; byte1 <= 0xF3; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0xBF; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x00; byte4 <= 0xFF; ++byte4)
                {
                    // skip correct fourth byte
                    if (0x80 <= byte3 && byte3 <= 0xBF)
                    {
                        continue;
                    }

                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// UTF8-4 (xF4 x80-8F UTF8-tail UTF8-tail)
// well-formed
TEST(JsonUnicodeRfc3629Test, Utf8_4CWellFormed)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x8F; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(true, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: missing second byte
TEST(JsonUnicodeRfc3629Test, Utf8_4CMissing2)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        EXPECT_TRUE(check_utf8string(false, byte1));
    }
}

// ill-formed: missing third byte
TEST(JsonUnicodeRfc3629Test, Utf8_4CMissing3)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x8F; ++byte2)
        {
            EXPECT_TRUE(check_utf8string(false, byte1, byte2));
        }
    }
}

// ill-formed: missing fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4CMissing4)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x8F; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3));
            }
        }
    }
}

// ill-formed: wrong second byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4CWrong2)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x00; byte2 <= 0xFF; ++byte2)
        {
            // skip correct second byte
            if (0x80 <= byte2 && byte2 <= 0x8F)
            {
                continue;
            }

            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong third byte
TEST(JsonUnicodeRfc3629Test, DISABLED_Utf8_4CWrong3)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x8F; ++byte2)
        {
            for (int byte3 = 0x00; byte3 <= 0xFF; ++byte3)
            {
                // skip correct third byte
                if (0x80 <= byte3 && byte3 <= 0xBF)
                {
                    continue;
                }

                for (int byte4 = 0x80; byte4 <= 0xBF; ++byte4)
                {
                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// ill-formed: wrong fourth byte
TEST(JsonUnicodeRfc3629Test, Utf8_4CWrong4)
{
    for (int byte1 = 0xF4; byte1 <= 0xF4; ++byte1)
    {
        for (int byte2 = 0x80; byte2 <= 0x8F; ++byte2)
        {
            for (int byte3 = 0x80; byte3 <= 0xBF; ++byte3)
            {
                for (int byte4 = 0x00; byte4 <= 0xFF; ++byte4)
                {
                    // skip correct fourth byte
                    if (0x80 <= byte3 && byte3 <= 0xBF)
                    {
                        continue;
                    }

                    EXPECT_TRUE(check_utf8string(false, byte1, byte2, byte3, byte4));
                }
            }
        }
    }
}

// \\uxxxx sequences

// create an escaped string from a code point
static std::string codepoint_to_unicode(std::size_t cp)
{
    // code points are represented as a six-character sequence: a
    // reverse solidus, followed by the lowercase letter u, followed
    // by four hexadecimal digits that encode the character's code
    // point
    std::string s;
    wpi::raw_string_ostream ss(s);
    ss << "\\u" << wpi::format_hex_no_prefix(cp, 4);
    ss.flush();
    return s;
}

// correct sequences
TEST(JsonUnicodeCodepointTest, DISABLED_Correct)
{
    // generate all UTF-8 code points; in total, 1112064 code points are
    // generated: 0x1FFFFF code points - 2048 invalid values between
    // 0xD800 and 0xDFFF.
    for (std::size_t cp = 0; cp <= 0x10FFFFu; ++cp)
    {
        // string to store the code point as in \uxxxx format
        std::string json_text = "\"";

        // decide whether to use one or two \uxxxx sequences
        if (cp < 0x10000u)
        {
            // The Unicode standard permanently reserves these code point
            // values for UTF-16 encoding of the high and low surrogates, and
            // they will never be assigned a character, so there should be no
            // reason to encode them. The official Unicode standard says that
            // no UTF forms, including UTF-16, can encode these code points.
            if (cp >= 0xD800u && cp <= 0xDFFFu)
            {
                // if we would not skip these code points, we would get a
                // "missing low surrogate" exception
                continue;
            }

            // code points in the Basic Multilingual Plane can be
            // represented with one \uxxxx sequence
            json_text += codepoint_to_unicode(cp);
        }
        else
        {
            // To escape an extended character that is not in the Basic
            // Multilingual Plane, the character is represented as a
            // 12-character sequence, encoding the UTF-16 surrogate pair
            const auto codepoint1 = 0xd800u + (((cp - 0x10000u) >> 10) & 0x3ffu);
            const auto codepoint2 = 0xdc00u + ((cp - 0x10000u) & 0x3ffu);
            json_text += codepoint_to_unicode(codepoint1) + codepoint_to_unicode(codepoint2);
        }

        json_text += "\"";
        SCOPED_TRACE(json_text);
        EXPECT_NO_THROW(json::parse(json_text));
    }
}

#if 0
// incorrect sequences
// high surrogate without low surrogate
TEST(JsonUnicodeCodepointTest, IncorrectHighMissingLow)
{
    // D800..DBFF are high surrogates and must be followed by low
    // surrogates DC00..DFFF; here, nothing follows
    for (std::size_t cp = 0xD800u; cp <= 0xDBFFu; ++cp)
    {
        std::string json_text = "\"" + codepoint_to_unicode(cp) + "\"";
        SCOPED_TRACE(json_text);
        EXPECT_THROW(json::parse(json_text), json::parse_error);
    }
}

// high surrogate with wrong low surrogate
TEST(JsonUnicodeCodepointTest, IncorrectHighWrongLow)
{
    // D800..DBFF are high surrogates and must be followed by low
    // surrogates DC00..DFFF; here a different sequence follows
    for (std::size_t cp1 = 0xD800u; cp1 <= 0xDBFFu; ++cp1)
    {
        for (std::size_t cp2 = 0x0000u; cp2 <= 0xFFFFu; ++cp2)
        {
            if (0xDC00u <= cp2 && cp2 <= 0xDFFFu)
            {
                continue;
            }

            std::string json_text = "\"" + codepoint_to_unicode(cp1) + codepoint_to_unicode(cp2) + "\"";
            SCOPED_TRACE(json_text);
            EXPECT_THROW(json::parse(json_text), json::parse_error);
        }
    }
}

// low surrogate without high surrogate
TEST(JsonUnicodeCodepointTest, IncorrectLowMissingHigh)
{
    // low surrogates DC00..DFFF must follow high surrogates; here,
    // they occur alone
    for (std::size_t cp = 0xDC00u; cp <= 0xDFFFu; ++cp)
    {
        std::string json_text = "\"" + codepoint_to_unicode(cp) + "\"";
        SCOPED_TRACE(json_text);
        EXPECT_THROW(json::parse(json_text), json::parse_error);
    }
}
#endif

#if 0
// read all unicode characters
TEST(JsonUnicodeTest, ReadAllUnicode)
{
    // read a file with all unicode characters stored as single-character
    // strings in a JSON array
    std::ifstream f("test/data/json_nlohmann_tests/all_unicode.json");
    json j;
    CHECK_NOTHROW(f >> j);

    // the array has 1112064 + 1 elemnts (a terminating "null" value)
    // Note: 1112064 = 0x1FFFFF code points - 2048 invalid values between
    // 0xD800 and 0xDFFF.
    CHECK(j.size() == 1112065);

    SECTION("check JSON Pointers")
    {
        for (auto s : j)
        {
            // skip non-string JSON values
            if (not s.is_string())
            {
                continue;
            }

            std::string ptr = s;

            // tilde must be followed by 0 or 1
            if (ptr == "~")
            {
                ptr += "0";
            }

            // JSON Pointers must begin with "/"
            ptr = "/" + ptr;

            CHECK_NOTHROW(json::json_pointer("/" + ptr));

            // check escape/unescape roundtrip
            auto escaped = json::json_pointer::escape(ptr);
            json::json_pointer::unescape(escaped);
            CHECK(escaped == ptr);
        }
    }
}

// ignore byte-order-mark
// in a stream
TEST(JsonUnicodeTest, IgnoreBOMStream)
{
    // read a file with a UTF-8 BOM
    std::ifstream f("test/data/json_nlohmann_tests/bom.json");
    json j;
    EXPECT_NO_THROW(f >> j);
}

// with an iterator
TEST(JsonUnicodeTest, IgnoreBOMIterator)
{
    std::string i = "\xef\xbb\xbf{\n   \"foo\": true\n}";
    EXPECT_NO_THROW(json::parse(i.begin(), i.end()));
}
#endif
// error for incomplete/wrong BOM
TEST(JsonUnicodeTest, WrongBOM)
{
    EXPECT_THROW(json::parse("\xef\xbb"), json::parse_error);
    EXPECT_THROW(json::parse("\xef\xbb\xbb"), json::parse_error);
}
