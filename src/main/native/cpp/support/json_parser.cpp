/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) FIRST 2017. All Rights Reserved.               */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
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
#define WPI_JSON_IMPLEMENTATION
#include "support/json.h"

#include <array>
#include <clocale> // lconv, localeconv
#include <locale> // locale

#include "llvm/Format.h"
#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"
#include "support/raw_istream.h"

using namespace wpi;

namespace {

//////////////////////
// lexer and parser //
//////////////////////

/*!
@brief lexical analysis

This class organizes the lexical analysis during JSON deserialization.
*/
class lexer
{
  public:
    /// token types for the parser
    enum class token_type
    {
        uninitialized,   ///< indicating the scanner is uninitialized
        literal_true,    ///< the `true` literal
        literal_false,   ///< the `false` literal
        literal_null,    ///< the `null` literal
        value_string,    ///< a string -- use get_string() for actual value
        value_unsigned,  ///< an unsigned integer -- use get_number_unsigned() for actual value
        value_integer,   ///< a signed integer -- use get_number_integer() for actual value
        value_float,     ///< an floating point number -- use get_number_float() for actual value
        begin_array,     ///< the character for array begin `[`
        begin_object,    ///< the character for object begin `{`
        end_array,       ///< the character for array end `]`
        end_object,      ///< the character for object end `}`
        name_separator,  ///< the name separator `:`
        value_separator, ///< the value separator `,`
        parse_error,     ///< indicating a parse error
        end_of_input     ///< indicating the end of the input buffer
    };

    /// return name of values of type token_type (only used for errors)
    static const char* token_type_name(const token_type t) noexcept;

    explicit lexer(wpi::raw_istream& s)
        : is(s), decimal_point_char(get_decimal_point())
    {}

  private:
    /////////////////////
    // locales
    /////////////////////

    /// return the locale-dependent decimal point
    static char get_decimal_point() noexcept
    {
        const auto loc = localeconv();
        assert(loc != nullptr);
        return (loc->decimal_point == nullptr) ? '.' : loc->decimal_point[0];
    }

    /////////////////////
    // scan functions
    /////////////////////

    /*!
    @brief get codepoint from 4 hex characters following `\u`

    @return codepoint or -1 in case of an error (e.g. EOF or non-hex
            character)
    */
    int get_codepoint();

    /*!
    @brief create diagnostic representation of a codepoint
    @return string "U+XXXX" for codepoint XXXX
    */
    static std::string codepoint_to_string(int codepoint);

    /*!
    @brief scan a string literal

    This function scans a string according to Sect. 7 of RFC 7159. While
    scanning, bytes are escaped and copied into buffer yytext. Then the
    function returns successfully.

    @return token_type::value_string if string could be successfully
            scanned, token_type::parse_error otherwise

    @note In case of errors, variable error_message contains a textual
          description.
    */
    token_type scan_string();

    static void strtof(float& f, const char* str, char** endptr) noexcept
    {
        f = std::strtof(str, endptr);
    }

    static void strtof(double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtod(str, endptr);
    }

    static void strtof(long double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtold(str, endptr);
    }

    /*!
    @brief scan a number literal

    This function scans a string according to Sect. 6 of RFC 7159.

    The function is realized with a deterministic finite state machine
    derived from the grammar described in RFC 7159. Starting in state
    "init", the input is read and used to determined the next state. Only
    state "done" accepts the number. State "error" is a trap state to model
    errors. In the table below, "anything" means any character but the ones
    listed before.

    state    | 0        | 1-9      | e E      | +       | -       | .        | anything
    ---------|----------|----------|----------|---------|---------|----------|-----------
    init     | zero     | any1     | [error]  | [error] | minus   | [error]  | [error]
    minus    | zero     | any1     | [error]  | [error] | [error] | [error]  | [error]
    zero     | done     | done     | exponent | done    | done    | decimal1 | done
    any1     | any1     | any1     | exponent | done    | done    | decimal1 | done
    decimal1 | decimal2 | [error]  | [error]  | [error] | [error] | [error]  | [error]
    decimal2 | decimal2 | decimal2 | exponent | done    | done    | done     | done
    exponent | any2     | any2     | [error]  | sign    | sign    | [error]  | [error]
    sign     | any2     | any2     | [error]  | [error] | [error] | [error]  | [error]
    any2     | any2     | any2     | done     | done    | done    | done     | done

    The state machine is realized with one label per state (prefixed with
    "scan_number_") and `goto` statements between them. The state machine
    contains cycles, but any cycle can be left when EOF is read. Therefore,
    the function is guaranteed to terminate.

    During scanning, the read bytes are stored in yytext. This string is
    then converted to a signed integer, an unsigned integer, or a
    floating-point number.

    @return token_type::value_unsigned, token_type::value_integer, or
            token_type::value_float if number could be successfully scanned,
            token_type::parse_error otherwise

    @note The scanner is independent of the current locale. Internally, the
          locale's decimal point is used instead of `.` to work with the
          locale-dependent converters.
    */
    token_type scan_number();

    token_type scan_true();
    token_type scan_false();
    token_type scan_null();

    /////////////////////
    // input management
    /////////////////////

    /// reset yytext
    void reset() noexcept
    {
        token_string.resize(0);
        yytext.resize(0);
    }

    /// get a character from the input
    int get()
    {
        ++chars_read;
        if (next_unget)
        {
            next_unget = false;
            return current;
        }
        char c;
        is.read(c);
        if (is.has_error())
        {
            current = std::char_traits<char>::eof();
        }
        else
        {
            current = static_cast<uint8_t>(c);
            token_string.push_back(c);
        }
        return current;
    }

    /// add a character to yytext
    void add(int c)
    {
        yytext.push_back(static_cast<char>(c));
    }

  public:
    /////////////////////
    // value getters
    /////////////////////

    /// return integer value
    std::int64_t get_number_integer() const noexcept
    {
        return value_integer;
    }

    /// return unsigned integer value
    std::uint64_t get_number_unsigned() const noexcept
    {
        return value_unsigned;
    }

    /// return floating-point value
    double get_number_float() const noexcept
    {
        return value_float;
    }

    /// return string value
    llvm::StringRef get_string()
    {
        return yytext.str();
    }

    /////////////////////
    // diagnostics
    /////////////////////

    /// return position of last read token
    size_t get_position() const noexcept
    {
        return chars_read;
    }

    /// return the last read token (for errors only)
    std::string get_token_string() const;

    /// return syntax error message
    const std::string& get_error_message() const noexcept
    {
        return error_message;
    }

    /////////////////////
    // actual scanner
    /////////////////////

    token_type scan();

  private:
    /// input adapter
    wpi::raw_istream& is;

    /// the current character
    int current = std::char_traits<char>::eof();

    /// whether get() should return the last character again
    bool next_unget = false;

    /// the number of characters read
    size_t chars_read = 0;

    /// buffer for raw byte sequence of the current token
    llvm::SmallString<128> token_string;

    /// buffer for variable-length tokens (numbers, strings)
    llvm::SmallString<128> yytext;

    /// a description of occurred lexer errors
    std::string error_message = "";

    // number values
    std::int64_t value_integer = 0;
    std::uint64_t value_unsigned = 0;
    double value_float = 0;

    /// the decimal point
    const char decimal_point_char = '.';
};

}  // anonymous namespace

const char* lexer::token_type_name(const token_type t) noexcept
{
    switch (t)
    {
        case token_type::uninitialized:
            return "<uninitialized>";
        case token_type::literal_true:
            return "true literal";
        case token_type::literal_false:
            return "false literal";
        case token_type::literal_null:
            return "null literal";
        case token_type::value_string:
            return "string literal";
        case lexer::token_type::value_unsigned:
        case lexer::token_type::value_integer:
        case lexer::token_type::value_float:
            return "number literal";
        case token_type::begin_array:
            return "'['";
        case token_type::begin_object:
            return "'{'";
        case token_type::end_array:
            return "']'";
        case token_type::end_object:
            return "'}'";
        case token_type::name_separator:
            return "':'";
        case token_type::value_separator:
            return "','";
        case token_type::parse_error:
            return "<parse error>";
        case token_type::end_of_input:
            return "end of input";
        default:
        {
            // catch non-enum values
            return "unknown token"; // LCOV_EXCL_LINE
        }
    }
}

int lexer::get_codepoint()
{
    // this function only makes sense after reading `\u`
    assert(current == 'u');
    int codepoint = 0;

    // byte 1: \uXxxx
    switch (get())
    {
        case '0':
            break;
        case '1':
            codepoint += 0x1000;
            break;
        case '2':
            codepoint += 0x2000;
            break;
        case '3':
            codepoint += 0x3000;
            break;
        case '4':
            codepoint += 0x4000;
            break;
        case '5':
            codepoint += 0x5000;
            break;
        case '6':
            codepoint += 0x6000;
            break;
        case '7':
            codepoint += 0x7000;
            break;
        case '8':
            codepoint += 0x8000;
            break;
        case '9':
            codepoint += 0x9000;
            break;
        case 'A':
        case 'a':
            codepoint += 0xa000;
            break;
        case 'B':
        case 'b':
            codepoint += 0xb000;
            break;
        case 'C':
        case 'c':
            codepoint += 0xc000;
            break;
        case 'D':
        case 'd':
            codepoint += 0xd000;
            break;
        case 'E':
        case 'e':
            codepoint += 0xe000;
            break;
        case 'F':
        case 'f':
            codepoint += 0xf000;
            break;
        default:
            return -1;
    }

    // byte 2: \uxXxx
    switch (get())
    {
        case '0':
            break;
        case '1':
            codepoint += 0x0100;
            break;
        case '2':
            codepoint += 0x0200;
            break;
        case '3':
            codepoint += 0x0300;
            break;
        case '4':
            codepoint += 0x0400;
            break;
        case '5':
            codepoint += 0x0500;
            break;
        case '6':
            codepoint += 0x0600;
            break;
        case '7':
            codepoint += 0x0700;
            break;
        case '8':
            codepoint += 0x0800;
            break;
        case '9':
            codepoint += 0x0900;
            break;
        case 'A':
        case 'a':
            codepoint += 0x0a00;
            break;
        case 'B':
        case 'b':
            codepoint += 0x0b00;
            break;
        case 'C':
        case 'c':
            codepoint += 0x0c00;
            break;
        case 'D':
        case 'd':
            codepoint += 0x0d00;
            break;
        case 'E':
        case 'e':
            codepoint += 0x0e00;
            break;
        case 'F':
        case 'f':
            codepoint += 0x0f00;
            break;
        default:
            return -1;
    }

    // byte 3: \uxxXx
    switch (get())
    {
        case '0':
            break;
        case '1':
            codepoint += 0x0010;
            break;
        case '2':
            codepoint += 0x0020;
            break;
        case '3':
            codepoint += 0x0030;
            break;
        case '4':
            codepoint += 0x0040;
            break;
        case '5':
            codepoint += 0x0050;
            break;
        case '6':
            codepoint += 0x0060;
            break;
        case '7':
            codepoint += 0x0070;
            break;
        case '8':
            codepoint += 0x0080;
            break;
        case '9':
            codepoint += 0x0090;
            break;
        case 'A':
        case 'a':
            codepoint += 0x00a0;
            break;
        case 'B':
        case 'b':
            codepoint += 0x00b0;
            break;
        case 'C':
        case 'c':
            codepoint += 0x00c0;
            break;
        case 'D':
        case 'd':
            codepoint += 0x00d0;
            break;
        case 'E':
        case 'e':
            codepoint += 0x00e0;
            break;
        case 'F':
        case 'f':
            codepoint += 0x00f0;
            break;
        default:
            return -1;
    }

    // byte 4: \uxxxX
    switch (get())
    {
        case '0':
            break;
        case '1':
            codepoint += 0x0001;
            break;
        case '2':
            codepoint += 0x0002;
            break;
        case '3':
            codepoint += 0x0003;
            break;
        case '4':
            codepoint += 0x0004;
            break;
        case '5':
            codepoint += 0x0005;
            break;
        case '6':
            codepoint += 0x0006;
            break;
        case '7':
            codepoint += 0x0007;
            break;
        case '8':
            codepoint += 0x0008;
            break;
        case '9':
            codepoint += 0x0009;
            break;
        case 'A':
        case 'a':
            codepoint += 0x000a;
            break;
        case 'B':
        case 'b':
            codepoint += 0x000b;
            break;
        case 'C':
        case 'c':
            codepoint += 0x000c;
            break;
        case 'D':
        case 'd':
            codepoint += 0x000d;
            break;
        case 'E':
        case 'e':
            codepoint += 0x000e;
            break;
        case 'F':
        case 'f':
            codepoint += 0x000f;
            break;
        default:
            return -1;
    }

    return codepoint;
}

std::string lexer::codepoint_to_string(int codepoint)
{
    std::string s;
    llvm::raw_string_ostream ss(s);
    ss << "U+" << llvm::format_hex_no_prefix(codepoint, 4, true);
    return ss.str();
}

lexer::token_type lexer::scan_string()
{
    // reset yytext (ignore opening quote)
    reset();

    // we entered the function by reading an open quote
    assert(current == '\"');

    while (true)
    {
        // get next character
        get();

        switch (current)
        {
            // end of file while parsing string
            case std::char_traits<char>::eof():
            {
                error_message = "invalid string: missing closing quote";
                return token_type::parse_error;
            }

            // closing quote
            case '\"':
            {
                // terminate yytext
                return token_type::value_string;
            }

            // escapes
            case '\\':
            {
                switch (get())
                {
                    // quotation mark
                    case '\"':
                        add('\"');
                        break;
                    // reverse solidus
                    case '\\':
                        add('\\');
                        break;
                    // solidus
                    case '/':
                        add('/');
                        break;
                    // backspace
                    case 'b':
                        add('\b');
                        break;
                    // form feed
                    case 'f':
                        add('\f');
                        break;
                    // line feed
                    case 'n':
                        add('\n');
                        break;
                    // carriage return
                    case 'r':
                        add('\r');
                        break;
                    // tab
                    case 't':
                        add('\t');
                        break;

                    // unicode escapes
                    case 'u':
                    {
                        int codepoint;
                        int codepoint1 = get_codepoint();

                        if (JSON_UNLIKELY(codepoint1 == -1))
                        {
                            error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                            return token_type::parse_error;
                        }

                        // check if code point is a high surrogate
                        if (0xD800 <= codepoint1 && codepoint1 <= 0xDBFF)
                        {
                            // expect next \uxxxx entry
                            if (JSON_LIKELY(get() == '\\' && get() == 'u'))
                            {
                                const int codepoint2 = get_codepoint();

                                if (JSON_UNLIKELY(codepoint2 == -1))
                                {
                                    error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                    return token_type::parse_error;
                                }

                                // check if codepoint2 is a low surrogate
                                if (JSON_LIKELY(0xDC00 <= codepoint2 && codepoint2 <= 0xDFFF))
                                {
                                    codepoint =
                                        // high surrogate occupies the most significant 22 bits
                                        (codepoint1 << 10)
                                        // low surrogate occupies the least significant 15 bits
                                        + codepoint2
                                        // there is still the 0xD800, 0xDC00 and 0x10000 noise
                                        // in the result so we have to subtract with:
                                        // (0xD800 << 10) + DC00 - 0x10000 = 0x35FDC00
                                        - 0x35FDC00;
                                }
                                else
                                {
                                    error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must be followed by U+DC00..U+DFFF instead of " + codepoint_to_string(codepoint2);
                                    return token_type::parse_error;
                                }
                            }
                            else
                            {
                                error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must be followed by U+DC00..U+DFFF";
                                return token_type::parse_error;
                            }
                        }
                        else
                        {
                            if (JSON_UNLIKELY(0xDC00 <= codepoint1 && codepoint1 <= 0xDFFF))
                            {
                                error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must follow U+D800..U+DBFF";
                                return token_type::parse_error;
                            }

                            // only work with first code point
                            codepoint = codepoint1;
                        }

                        // result of the above calculation yields a proper codepoint
                        assert(0x00 <= codepoint && codepoint <= 0x10FFFF);

                        // translate code point to bytes
                        if (codepoint < 0x80)
                        {
                            // 1-byte characters: 0xxxxxxx (ASCII)
                            add(codepoint);
                        }
                        else if (codepoint <= 0x7ff)
                        {
                            // 2-byte characters: 110xxxxx 10xxxxxx
                            add(0xC0 | (codepoint >> 6));
                            add(0x80 | (codepoint & 0x3F));
                        }
                        else if (codepoint <= 0xffff)
                        {
                            // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
                            add(0xE0 | (codepoint >> 12));
                            add(0x80 | ((codepoint >> 6) & 0x3F));
                            add(0x80 | (codepoint & 0x3F));
                        }
                        else
                        {
                            // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                            add(0xF0 | (codepoint >> 18));
                            add(0x80 | ((codepoint >> 12) & 0x3F));
                            add(0x80 | ((codepoint >> 6) & 0x3F));
                            add(0x80 | (codepoint & 0x3F));
                        }

                        break;
                    }

                    // other characters after escape
                    default:
                        error_message = "invalid string: forbidden character after backslash";
                        return token_type::parse_error;
                }

                break;
            }

            // invalid control characters
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0c:
            case 0x0d:
            case 0x0e:
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f:
            {
                error_message = "invalid string: control character " + codepoint_to_string(current) + " must be escaped";
                return token_type::parse_error;
            }

            // U+0020..U+007F (except U+0022 (quote) and U+005C (backspace))
            case 0x20:
            case 0x21:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3a:
            case 0x3b:
            case 0x3c:
            case 0x3d:
            case 0x3e:
            case 0x3f:
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4a:
            case 0x4b:
            case 0x4c:
            case 0x4d:
            case 0x4e:
            case 0x4f:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5d:
            case 0x5e:
            case 0x5f:
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6a:
            case 0x6b:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x6f:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7a:
            case 0x7b:
            case 0x7c:
            case 0x7d:
            case 0x7e:
            case 0x7f:
            {
                add(current);
                break;
            }

            // U+0080..U+07FF: bytes C2..DF 80..BF
            case 0xc2:
            case 0xc3:
            case 0xc4:
            case 0xc5:
            case 0xc6:
            case 0xc7:
            case 0xc8:
            case 0xc9:
            case 0xca:
            case 0xcb:
            case 0xcc:
            case 0xcd:
            case 0xce:
            case 0xcf:
            case 0xd0:
            case 0xd1:
            case 0xd2:
            case 0xd3:
            case 0xd4:
            case 0xd5:
            case 0xd6:
            case 0xd7:
            case 0xd8:
            case 0xd9:
            case 0xda:
            case 0xdb:
            case 0xdc:
            case 0xdd:
            case 0xde:
            case 0xdf:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                {
                    add(current);
                    continue;
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+0800..U+0FFF: bytes E0 A0..BF 80..BF
            case 0xe0:
            {
                add(current);
                get();
                if (JSON_LIKELY(0xa0 <= current && current <= 0xbf))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        continue;
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+1000..U+CFFF: bytes E1..EC 80..BF 80..BF
            // U+E000..U+FFFF: bytes EE..EF 80..BF 80..BF
            case 0xe1:
            case 0xe2:
            case 0xe3:
            case 0xe4:
            case 0xe5:
            case 0xe6:
            case 0xe7:
            case 0xe8:
            case 0xe9:
            case 0xea:
            case 0xeb:
            case 0xec:
            case 0xee:
            case 0xef:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        continue;
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+D000..U+D7FF: bytes ED 80..9F 80..BF
            case 0xed:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x80 <= current && current <= 0x9f))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        continue;
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+10000..U+3FFFF F0 90..BF 80..BF 80..BF
            case 0xf0:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x90 <= current && current <= 0xbf))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        get();
                        if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                        {
                            add(current);
                            continue;
                        }
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+40000..U+FFFFF F1..F3 80..BF 80..BF 80..BF
            case 0xf1:
            case 0xf2:
            case 0xf3:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        get();
                        if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                        {
                            add(current);
                            continue;
                        }
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // U+100000..U+10FFFF F4 80..8F 80..BF 80..BF
            case 0xf4:
            {
                add(current);
                get();
                if (JSON_LIKELY(0x80 <= current && current <= 0x8f))
                {
                    add(current);
                    get();
                    if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                    {
                        add(current);
                        get();
                        if (JSON_LIKELY(0x80 <= current && current <= 0xbf))
                        {
                            add(current);
                            continue;
                        }
                    }
                }

                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }

            // remaining bytes (80..C1 and F5..FF) are ill-formed
            default:
            {
                error_message = "invalid string: ill-formed UTF-8 byte";
                return token_type::parse_error;
            }
        }
    }
}

lexer::token_type lexer::scan_number()
{
    // reset yytext to store the number's bytes
    reset();

    // the type of the parsed number; initially set to unsigned; will be
    // changed if minus sign, decimal point or exponent is read
    token_type number_type = token_type::value_unsigned;

    // state (init): we just found out we need to scan a number
    switch (current)
    {
        case '-':
        {
            add(current);
            goto scan_number_minus;
        }

        case '0':
        {
            add(current);
            goto scan_number_zero;
        }

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any1;
        }

        default:
        {
            // all other characters are rejected outside scan_number()
            assert(false);  // LCOV_EXCL_LINE
        }
    }

scan_number_minus:
    // state: we just parsed a leading minus sign
    number_type = token_type::value_integer;
    switch (get())
    {
        case '0':
        {
            add(current);
            goto scan_number_zero;
        }

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any1;
        }

        default:
        {
            error_message = "invalid number; expected digit after '-'";
            return token_type::parse_error;
        }
    }

scan_number_zero:
    // state: we just parse a zero (maybe with a leading minus sign)
    switch (get())
    {
        case '.':
        {
            add(decimal_point_char);
            goto scan_number_decimal1;
        }

        case 'e':
        case 'E':
        {
            add(current);
            goto scan_number_exponent;
        }

        default:
        {
            goto scan_number_done;
        }
    }

scan_number_any1:
    // state: we just parsed a number 0-9 (maybe with a leading minus sign)
    switch (get())
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any1;
        }

        case '.':
        {
            add(decimal_point_char);
            goto scan_number_decimal1;
        }

        case 'e':
        case 'E':
        {
            add(current);
            goto scan_number_exponent;
        }

        default:
        {
            goto scan_number_done;
        }
    }

scan_number_decimal1:
    // state: we just parsed a decimal point
    number_type = token_type::value_float;
    switch (get())
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_decimal2;
        }

        default:
        {
            error_message = "invalid number; expected digit after '.'";
            return token_type::parse_error;
        }
    }

scan_number_decimal2:
    // we just parsed at least one number after a decimal point
    switch (get())
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_decimal2;
        }

        case 'e':
        case 'E':
        {
            add(current);
            goto scan_number_exponent;
        }

        default:
        {
            goto scan_number_done;
        }
    }

scan_number_exponent:
    // we just parsed an exponent
    number_type = token_type::value_float;
    switch (get())
    {
        case '+':
        case '-':
        {
            add(current);
            goto scan_number_sign;
        }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any2;
        }

        default:
        {
            error_message = "invalid number; expected '+', '-', or digit after exponent";
            return token_type::parse_error;
        }
    }

scan_number_sign:
    // we just parsed an exponent sign
    switch (get())
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any2;
        }

        default:
        {
            error_message = "invalid number; expected digit after exponent sign";
            return token_type::parse_error;
        }
    }

scan_number_any2:
    // we just parsed a number after the exponent or exponent sign
    switch (get())
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            add(current);
            goto scan_number_any2;
        }

        default:
        {
            goto scan_number_done;
        }
    }

scan_number_done:
    // unget the character after the number (we only read it to know
    // that we are done scanning a number)
    --chars_read;
    next_unget = true;

    // try to parse integers first and fall back to floats
    if (number_type == token_type::value_unsigned)
    {
        char* endptr = nullptr;
        errno = 0;
        const auto x = std::strtoull(yytext.c_str(), &endptr, 10);

        // we checked the number format before
        assert(endptr == yytext.data() + yytext.size());

        if (errno == 0)
        {
            value_unsigned = static_cast<std::uint64_t>(x);
            if (value_unsigned == x)
            {
                return token_type::value_unsigned;
            }
        }
    }
    else if (number_type == token_type::value_integer)
    {
        char* endptr = nullptr;
        errno = 0;
        const auto x = std::strtoll(yytext.c_str(), &endptr, 10);

        // we checked the number format before
        assert(endptr == yytext.data() + yytext.size());

        if (errno == 0)
        {
            value_integer = static_cast<std::int64_t>(x);
            if (value_integer == x)
            {
                return token_type::value_integer;
            }
        }
    }

    // this code is reached if we parse a floating-point number or if
    // an integer conversion above failed
    strtof(value_float, yytext.c_str(), nullptr);
    return token_type::value_float;
}

lexer::token_type lexer::scan_true()
{
    assert(current == 't');
    if (JSON_LIKELY((get() == 'r' && get() == 'u' && get() == 'e')))
    {
        return token_type::literal_true;
    }

    error_message = "invalid literal; expected 'true'";
    return token_type::parse_error;
}

lexer::token_type lexer::scan_false()
{
    assert(current == 'f');
    if (JSON_LIKELY((get() == 'a' && get() == 'l' && get() == 's' && get() == 'e')))
    {
        return token_type::literal_false;
    }

    error_message = "invalid literal; expected 'false'";
    return token_type::parse_error;
}

lexer::token_type lexer::scan_null()
{
    assert(current == 'n');
    if (JSON_LIKELY((get() == 'u' && get() == 'l' && get() == 'l')))
    {
        return token_type::literal_null;
    }

    error_message = "invalid literal; expected 'null'";
    return token_type::parse_error;
}

std::string lexer::get_token_string() const
{
    // escape control characters
    std::string result;
    for (auto c : token_string)
    {
        if (c == '\0' || c == std::char_traits<char>::eof())
        {
            // ignore EOF
            continue;
        }
        else if ('\x00' <= c && c <= '\x1f')
        {
            // escape control characters
            result += "<" + codepoint_to_string(c) + ">";
        }
        else
        {
            // add character as is
            result.append(1, c);
        }
    }

    return result;
}

lexer::token_type lexer::scan()
{
    // read next character and ignore whitespace
    do
    {
        get();
    }
    while (current == ' ' || current == '\t' || current == '\n' || current == '\r');

    switch (current)
    {
        // structural characters
        case '[':
            return token_type::begin_array;
        case ']':
            return token_type::end_array;
        case '{':
            return token_type::begin_object;
        case '}':
            return token_type::end_object;
        case ':':
            return token_type::name_separator;
        case ',':
            return token_type::value_separator;

        // literals
        case 't':
            return scan_true();
        case 'f':
            return scan_false();
        case 'n':
            return scan_null();

        // string
        case '\"':
            return scan_string();

        // number
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return scan_number();

        // end of input (the null byte is needed when parsing from
        // string literals)
        case '\0':
        case std::char_traits<char>::eof():
            return token_type::end_of_input;

        // error
        default:
            error_message = "invalid literal";
            return token_type::parse_error;
    }
}

/*!
@brief syntax analysis

This class implements a recursive decent parser.
*/
class json::parser
{
    using value_t = json::value_t;

  public:
    /// a parser reading from an input adapter
    explicit parser(wpi::raw_istream& s,
                    const parser_callback_t cb = nullptr)
        : callback(cb), m_lexer(s)
    {}

    /*!
    @brief public parser interface

    @param[in] strict  whether to expect the last token to be EOF
    @return parsed JSON value

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    */
    json parse(bool strict = true);

    /*!
    @brief public accept interface

    @param[in] strict  whether to expect the last token to be EOF
    @return whether the input is a proper JSON text
    */
    bool accept(bool strict = true);

  private:
    /*!
    @brief the actual parser
    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    */
    json parse_internal(bool keep);

    /*!
    @brief the acutal acceptor

    @invariant 1. The last token is not yet processed. Therefore, the
                  caller of this function must make sure a token has
                  been read.
               2. When this function returns, the last token is processed.
                  That is, the last read character was already considered.

    This invariant makes sure that no token needs to be "unput".
    */
    bool accept_internal();

    /// get next token from lexer
    lexer::token_type get_token()
    {
        last_token = m_lexer.scan();
        return last_token;
    }

    /*!
    @throw parse_error.101 if expected token did not occur
    */
    void expect(lexer::token_type t) const;

    /*!
    @throw parse_error.101 if unexpected token occurred
    */
    void unexpect(lexer::token_type t) const;

  private:
    /// current level of recursion
    int depth = 0;
    /// callback function
    const parser_callback_t callback = nullptr;
    /// the type of the last read token
    lexer::token_type last_token = lexer::token_type::uninitialized;
    /// the lexer
    lexer m_lexer;
};

json json::parser::parse(bool strict)
{
    // read first token
    get_token();

    json result = parse_internal(true);
    result.assert_invariant();

    if (strict)
    {
        get_token();
        expect(lexer::token_type::end_of_input);
    }

    // return parser result and replace it with null in case the
    // top-level value was discarded by the callback function
    return result.is_discarded() ? json() : std::move(result);
}

bool json::parser::accept(bool strict)
{
    // read first token
    get_token();

    if (!accept_internal())
    {
        return false;
    }

    if (strict && get_token() != lexer::token_type::end_of_input)
    {
        return false;
    }

    return true;
}

json json::parser::parse_internal(bool keep)
{
    auto result = json(value_t::discarded);

    switch (last_token)
    {
        case lexer::token_type::begin_object:
        {
            if (keep && (!callback
                          || ((keep = callback(depth++, parse_event_t::object_start, result)) != 0)))
            {
                // explicitly set result to object to cope with {}
                result.m_type = value_t::object;
                result.m_value = value_t::object;
            }

            // read next token
            get_token();

            // closing } -> we are done
            if (last_token == lexer::token_type::end_object)
            {
                if (keep && callback && !callback(--depth, parse_event_t::object_end, result))
                {
                    result = json(value_t::discarded);
                }
                return result;
            }

            // parse values
            while (true)
            {
                // store key
                expect(lexer::token_type::value_string);
                std::string key = m_lexer.get_string();

                bool keep_tag = false;
                if (keep)
                {
                    if (callback)
                    {
                        json k(key);
                        keep_tag = callback(depth, parse_event_t::key, k);
                    }
                    else
                    {
                        keep_tag = true;
                    }
                }

                // parse separator (:)
                get_token();
                expect(lexer::token_type::name_separator);

                // parse and add value
                get_token();
                auto value = parse_internal(keep);
                if (keep && keep_tag && !value.is_discarded())
                {
                    result[key] = std::move(value);
                }

                // comma -> next value
                get_token();
                if (last_token == lexer::token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing }
                expect(lexer::token_type::end_object);
                break;
            }

            if (keep && callback && !callback(--depth, parse_event_t::object_end, result))
            {
                result = json(value_t::discarded);
            }

            return result;
        }

        case lexer::token_type::begin_array:
        {
            if (keep && (!callback
                          || ((keep = callback(depth++, parse_event_t::array_start, result)) != 0)))
            {
                // explicitly set result to object to cope with []
                result.m_type = value_t::array;
                result.m_value = value_t::array;
            }

            // read next token
            get_token();

            // closing ] -> we are done
            if (last_token == lexer::token_type::end_array)
            {
                if (callback && !callback(--depth, parse_event_t::array_end, result))
                {
                    result = json(value_t::discarded);
                }
                return result;
            }

            // parse values
            while (true)
            {
                // parse value
                auto value = parse_internal(keep);
                if (keep && !value.is_discarded())
                {
                    result.push_back(std::move(value));
                }

                // comma -> next value
                get_token();
                if (last_token == lexer::token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing ]
                expect(lexer::token_type::end_array);
                break;
            }

            if (keep && callback && !callback(--depth, parse_event_t::array_end, result))
            {
                result = json(value_t::discarded);
            }

            return result;
        }

        case lexer::token_type::literal_null:
        {
            result.m_type = value_t::null;
            break;
        }

        case lexer::token_type::value_string:
        {
            result = json(m_lexer.get_string());
            break;
        }

        case lexer::token_type::literal_true:
        {
            result.m_type = value_t::boolean;
            result.m_value = true;
            break;
        }

        case lexer::token_type::literal_false:
        {
            result.m_type = value_t::boolean;
            result.m_value = false;
            break;
        }

        case lexer::token_type::value_unsigned:
        {
            result.m_type = value_t::number_unsigned;
            result.m_value = m_lexer.get_number_unsigned();
            break;
        }

        case lexer::token_type::value_integer:
        {
            result.m_type = value_t::number_integer;
            result.m_value = m_lexer.get_number_integer();
            break;
        }

        case lexer::token_type::value_float:
        {
            result.m_type = value_t::number_float;
            result.m_value = m_lexer.get_number_float();

            // throw in case of infinity or NAN
            if (JSON_UNLIKELY(!std::isfinite(result.m_value.number_float)))
            {
                JSON_THROW(json::out_of_range::create(406, "number overflow parsing '" + m_lexer.get_token_string() + "'"));
            }

            break;
        }

        default:
        {
            // the last token was unexpected
            unexpect(last_token);
        }
    }

    if (keep && callback && !callback(depth, parse_event_t::value, result))
    {
        result = json(value_t::discarded);
    }
    return result;
}

bool json::parser::accept_internal()
{
    switch (last_token)
    {
        case lexer::token_type::begin_object:
        {
            // read next token
            get_token();

            // closing } -> we are done
            if (last_token == lexer::token_type::end_object)
            {
                return true;
            }

            // parse values
            while (true)
            {
                // parse key
                if (last_token != lexer::token_type::value_string)
                {
                    return false;
                }

                // parse separator (:)
                get_token();
                if (last_token != lexer::token_type::name_separator)
                {
                    return false;
                }

                // parse value
                get_token();
                if (!accept_internal())
                {
                    return false;
                }

                // comma -> next value
                get_token();
                if (last_token == lexer::token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing }
                if (last_token != lexer::token_type::end_object)
                {
                    return false;
                }

                return true;
            }
        }

        case lexer::token_type::begin_array:
        {
            // read next token
            get_token();

            // closing ] -> we are done
            if (last_token == lexer::token_type::end_array)
            {
                return true;
            }

            // parse values
            while (true)
            {
                // parse value
                if (!accept_internal())
                {
                    return false;
                }

                // comma -> next value
                get_token();
                if (last_token == lexer::token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing ]
                if (last_token != lexer::token_type::end_array)
                {
                    return false;
                }

                return true;
            }
        }

        case lexer::token_type::literal_false:
        case lexer::token_type::literal_null:
        case lexer::token_type::literal_true:
        case lexer::token_type::value_float:
        case lexer::token_type::value_integer:
        case lexer::token_type::value_string:
        case lexer::token_type::value_unsigned:
        {
            return true;
        }

        default:
        {
            // the last token was unexpected
            return false;
        }
    }
}

void json::parser::expect(lexer::token_type t) const
{
    if (JSON_UNLIKELY(t != last_token))
    {
        std::string error_msg = "syntax error - ";
        if (last_token == lexer::token_type::parse_error)
        {
            error_msg += m_lexer.get_error_message() + "; last read: '" + m_lexer.get_token_string() + "'";
        }
        else
        {
            error_msg += "unexpected " + std::string(lexer::token_type_name(last_token));
        }

        error_msg += "; expected " + std::string(lexer::token_type_name(t));
        JSON_THROW(json::parse_error::create(101, m_lexer.get_position(), error_msg));
    }
}

void json::parser::unexpect(lexer::token_type t) const
{
    if (JSON_UNLIKELY(t == last_token))
    {
        std::string error_msg = "syntax error - ";
        if (last_token == lexer::token_type::parse_error)
        {
            error_msg += m_lexer.get_error_message() + "; last read '" + m_lexer.get_token_string() + "'";
        }
        else
        {
            error_msg += "unexpected " + std::string(lexer::token_type_name(last_token));
        }

        JSON_THROW(json::parse_error::create(101, m_lexer.get_position(), error_msg));
    }
}

json json::parse(llvm::StringRef s, const parser_callback_t cb)
{
    wpi::raw_mem_istream is(s.data(), s.size());
    return parser(is, cb).parse(true);
}

json json::parse(wpi::raw_istream& i, const parser_callback_t cb)
{
    return parser(i, cb).parse(true);
}

namespace wpi {

wpi::raw_istream& operator>>(wpi::raw_istream& i, json& j)
{
    j = json::parser(i).parse(false);
    return i;
}

}  // namespace wpi
