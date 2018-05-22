/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) 2017-2018 FIRST. All Rights Reserved.          */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
|  |  |__   |  |  | | | |  version 3.1.2
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2018 Niels Lohmann <http://nlohmann.me>.

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
#include "wpi/json.h"

#include <clocale>
#include <cmath>
#include <cstdlib>

#include "wpi/Format.h"
#include "wpi/SmallString.h"
#include "wpi/raw_istream.h"
#include "wpi/raw_ostream.h"

namespace wpi {

/*!
@brief lexical analysis

This class organizes the lexical analysis during JSON deserialization.
*/
class json::lexer
{
  public:
    /// token types for the parser
    enum class token_type
    {
        uninitialized,    ///< indicating the scanner is uninitialized
        literal_true,     ///< the `true` literal
        literal_false,    ///< the `false` literal
        literal_null,     ///< the `null` literal
        value_string,     ///< a string -- use get_string() for actual value
        value_unsigned,   ///< an unsigned integer -- use get_number_unsigned() for actual value
        value_integer,    ///< a signed integer -- use get_number_integer() for actual value
        value_float,      ///< an floating point number -- use get_number_float() for actual value
        begin_array,      ///< the character for array begin `[`
        begin_object,     ///< the character for object begin `{`
        end_array,        ///< the character for array end `]`
        end_object,       ///< the character for object end `}`
        name_separator,   ///< the name separator `:`
        value_separator,  ///< the value separator `,`
        parse_error,      ///< indicating a parse error
        end_of_input,     ///< indicating the end of the input buffer
        literal_or_value  ///< a literal or the begin of a value (only for diagnostics)
    };

    /// return name of values of type token_type (only used for errors)
    static const char* token_type_name(const token_type t) noexcept;

    explicit lexer(raw_istream& s);

    // delete because of pointer members
    lexer(const lexer&) = delete;
    lexer& operator=(lexer&) = delete;

  private:
    /////////////////////
    // locales
    /////////////////////

    /// return the locale-dependent decimal point
    static char get_decimal_point() noexcept
    {
        const auto loc = localeconv();
        assert(loc != nullptr);
        return (loc->decimal_point == nullptr) ? '.' : *(loc->decimal_point);
    }

    /////////////////////
    // scan functions
    /////////////////////

    /*!
    @brief get codepoint from 4 hex characters following `\u`

    For input "\u c1 c2 c3 c4" the codepoint is:
      (c1 * 0x1000) + (c2 * 0x0100) + (c3 * 0x0010) + c4
    = (c1 << 12) + (c2 << 8) + (c3 << 4) + (c4 << 0)

    Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f'
    must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp. The
    conversion is done by subtracting the offset (0x30, 0x37, and 0x57)
    between the ASCII value of the character and the desired integer value.

    @return codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or
            non-hex character)
    */
    int get_codepoint();

    /*!
    @brief check if the next byte(s) are inside a given range

    Adds the current byte and, for each passed range, reads a new byte and
    checks if it is inside the range. If a violation was detected, set up an
    error message and return false. Otherwise, return true.

    @param[in] ranges  list of integers; interpreted as list of pairs of
                       inclusive lower and upper bound, respectively

    @pre The passed list @a ranges must have 2, 4, or 6 elements; that is,
         1, 2, or 3 pairs. This precondition is enforced by an assertion.

    @return true if and only if no range violation was detected
    */
    bool next_byte_in_range(std::initializer_list<int> ranges)
    {
        assert(ranges.size() == 2 or ranges.size() == 4 or ranges.size() == 6);
        add(current);

        for (auto range = ranges.begin(); range != ranges.end(); ++range)
        {
            get();
            if (JSON_LIKELY(*range <= current and current <= *(++range)))
            {
                add(current);
            }
            else
            {
                error_message = "invalid string: ill-formed UTF-8 byte";
                return false;
            }
        }

        return true;
    }

    /*!
    @brief scan a string literal

    This function scans a string according to Sect. 7 of RFC 7159. While
    scanning, bytes are escaped and copied into buffer token_buffer. Then the
    function returns successfully, token_buffer is *not* null-terminated (as it
    may contain \0 bytes), and token_buffer.size() is the number of bytes in the
    string.

    @return token_type::value_string if string could be successfully scanned,
            token_type::parse_error otherwise

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

    The function is realized with a deterministic finite state machine derived
    from the grammar described in RFC 7159. Starting in state "init", the
    input is read and used to determined the next state. Only state "done"
    accepts the number. State "error" is a trap state to model errors. In the
    table below, "anything" means any character but the ones listed before.

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

    During scanning, the read bytes are stored in token_buffer. This string is
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

    /*!
    @param[in] literal_text  the literal text to expect
    @param[in] length        the length of the passed literal text
    @param[in] return_type   the token type to return on success
    */
    token_type scan_literal(const char* literal_text, const std::size_t length,
                            token_type return_type);

    /////////////////////
    // input management
    /////////////////////

    /// reset token_buffer; current character is beginning of token
    void reset() noexcept
    {
        token_buffer.clear();
        token_string.clear();
        token_string.push_back(std::char_traits<char>::to_char_type(current));
    }

    /*
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a
    `std::char_traits<char>::eof()` in that case.  Stores the scanned characters
    for use in error messages.

    @return character read from the input
    */
    std::char_traits<char>::int_type get()
    {
        ++chars_read;
        if (JSON_UNLIKELY(!unget_chars.empty()))
        {
            current = unget_chars.back();
            unget_chars.pop_back();
            token_string.push_back(current);
            return current;
        }
        char c;
        is.read(c);
        if (JSON_UNLIKELY(is.has_error()))
        {
            current = std::char_traits<char>::eof();
        }
        else
        {
            current = std::char_traits<char>::to_int_type(c);
            token_string.push_back(c);
        }
        return current;
    }

    /// unget current character (return it again on next get)
    void unget()
    {
        --chars_read;
        if (JSON_LIKELY(current != std::char_traits<char>::eof()))
        {
            unget_chars.emplace_back(current);
            assert(token_string.size() != 0);
            token_string.pop_back();
            if (!token_string.empty())
            {
                current = token_string.back();
            }
        }
    }

    /// put back character (returned on next get)
    void putback(std::char_traits<char>::int_type c)
    {
        --chars_read;
        unget_chars.emplace_back(c);
    }

    /// add a character to token_buffer
    void add(int c)
    {
        token_buffer.push_back(std::char_traits<char>::to_char_type(c));
    }

  public:
    /////////////////////
    // value getters
    /////////////////////

    /// return integer value
    int64_t get_number_integer() const noexcept
    {
        return value_integer;
    }

    /// return unsigned integer value
    uint64_t get_number_unsigned() const noexcept
    {
        return value_unsigned;
    }

    /// return floating-point value
    double get_number_float() const noexcept
    {
        return value_float;
    }

    /// return current string value
    StringRef get_string()
    {
        return token_buffer;
    }

    /////////////////////
    // diagnostics
    /////////////////////

    /// return position of last read token
    std::size_t get_position() const noexcept
    {
        return chars_read;
    }

    /// return the last read token (for errors only).  Will never contain EOF
    /// (an arbitrary value that is not a valid char value, often -1), because
    /// 255 may legitimately occur.  May contain NUL, which should be escaped.
    std::string get_token_string() const;

    /// return syntax error message
    const char* get_error_message() const noexcept
    {
        return error_message;
    }

    /////////////////////
    // actual scanner
    /////////////////////

    token_type scan();

  private:
    /// input adapter
    raw_istream& is;

    /// the current character
    std::char_traits<char>::int_type current = std::char_traits<char>::eof();

    /// unget characters
    SmallVector<std::char_traits<char>::int_type, 4> unget_chars;

    /// the number of characters read
    std::size_t chars_read = 0;

    /// raw input token string (for error messages)
    SmallString<128> token_string {};

    /// buffer for variable-length tokens (numbers, strings)
    SmallString<128> token_buffer {};

    /// a description of occurred lexer errors
    const char* error_message = "";

    // number values
    int64_t value_integer = 0;
    uint64_t value_unsigned = 0;
    double value_float = 0;

    /// the decimal point
    const char decimal_point_char = '.';
};

////////////
// parser //
////////////

/*!
@brief syntax analysis

This class implements a recursive decent parser.
*/
class json::parser
{
    using lexer_t = json::lexer;
    using token_type = typename lexer_t::token_type;

  public:
    /// a parser reading from an input adapter
    explicit parser(raw_istream& s,
                    const parser_callback_t cb = nullptr,
                    const bool allow_exceptions_ = true)
        : callback(cb), m_lexer(s), allow_exceptions(allow_exceptions_)
    {}

    /*!
    @brief public parser interface

    @param[in] strict      whether to expect the last token to be EOF
    @param[in,out] result  parsed JSON value

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    */
    void parse(const bool strict, json& result);

    /*!
    @brief public accept interface

    @param[in] strict  whether to expect the last token to be EOF
    @return whether the input is a proper JSON text
    */
    bool accept(const bool strict = true)
    {
        // read first token
        get_token();

        if (not accept_internal())
        {
            return false;
        }

        // strict => last token must be EOF
        return not strict or (get_token() == token_type::end_of_input);
    }

  private:
    /*!
    @brief the actual parser
    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    */
    void parse_internal(bool keep, json& result);

    /*!
    @brief the actual acceptor

    @invariant 1. The last token is not yet processed. Therefore, the caller
                  of this function must make sure a token has been read.
               2. When this function returns, the last token is processed.
                  That is, the last read character was already considered.

    This invariant makes sure that no token needs to be "unput".
    */
    bool accept_internal();

    /// get next token from lexer
    token_type get_token()
    {
        return (last_token = m_lexer.scan());
    }

    /*!
    @throw parse_error.101 if expected token did not occur
    */
    bool expect(token_type t)
    {
        if (JSON_UNLIKELY(t != last_token))
        {
            errored = true;
            expected = t;
            if (allow_exceptions)
            {
                throw_exception();
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    [[noreturn]] void throw_exception() const;

  private:
    /// current level of recursion
    int depth = 0;
    /// callback function
    const parser_callback_t callback = nullptr;
    /// the type of the last read token
    token_type last_token = token_type::uninitialized;
    /// the lexer
    lexer_t m_lexer;
    /// whether a syntax error occurred
    bool errored = false;
    /// possible reason for the syntax error
    token_type expected = token_type::uninitialized;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
};

const char* json::lexer::token_type_name(const token_type t) noexcept
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
        case token_type::literal_or_value:
            return "'[', '{', or a literal";
        default: // catch non-enum values
            return "unknown token"; // LCOV_EXCL_LINE
    }
}

json::lexer::lexer(raw_istream& s)
    : is(s), decimal_point_char(get_decimal_point())
{
    // skip byte order mark
    std::char_traits<char>::int_type c;
    if ((c = get()) == 0xEF)
    {
        if ((c = get()) == 0xBB)
        {
            if ((c = get()) == 0xBF)
            {
                chars_read = 0;
                return; // Ignore BOM
            }
            else if (c != std::char_traits<char>::eof())
            {
                unget();
            }
            putback('\xBB');
        }
        else if (c != std::char_traits<char>::eof())
        {
            unget();
        }
        putback('\xEF');
    }
    unget(); // no byte order mark; process as usual
}

int json::lexer::get_codepoint()
{
    // this function only makes sense after reading `\u`
    assert(current == 'u');
    int codepoint = 0;

    const auto factors = { 12, 8, 4, 0 };
    for (const auto factor : factors)
    {
        get();

        if (current >= '0' and current <= '9')
        {
            codepoint += ((current - 0x30) << factor);
        }
        else if (current >= 'A' and current <= 'F')
        {
            codepoint += ((current - 0x37) << factor);
        }
        else if (current >= 'a' and current <= 'f')
        {
            codepoint += ((current - 0x57) << factor);
        }
        else
        {
            return -1;
        }
    }

    assert(0x0000 <= codepoint and codepoint <= 0xFFFF);
    return codepoint;
}

json::lexer::token_type json::lexer::scan_string()
{
    // reset token_buffer (ignore opening quote)
    reset();

    // we entered the function by reading an open quote
    assert(current == '\"');

    while (true)
    {
        // get next character
        switch (get())
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
                        const int codepoint1 = get_codepoint();
                        int codepoint = codepoint1; // start with codepoint1

                        if (JSON_UNLIKELY(codepoint1 == -1))
                        {
                            error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                            return token_type::parse_error;
                        }

                        // check if code point is a high surrogate
                        if (0xD800 <= codepoint1 and codepoint1 <= 0xDBFF)
                        {
                            // expect next \uxxxx entry
                            if (JSON_LIKELY(get() == '\\' and get() == 'u'))
                            {
                                const int codepoint2 = get_codepoint();

                                if (JSON_UNLIKELY(codepoint2 == -1))
                                {
                                    error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                    return token_type::parse_error;
                                }

                                // check if codepoint2 is a low surrogate
                                if (JSON_LIKELY(0xDC00 <= codepoint2 and codepoint2 <= 0xDFFF))
                                {
                                    // overwrite codepoint
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
                                    error_message = "invalid string: surrogate U+DC00..U+DFFF must be followed by U+DC00..U+DFFF";
                                    return token_type::parse_error;
                                }
                            }
                            else
                            {
                                error_message = "invalid string: surrogate U+DC00..U+DFFF must be followed by U+DC00..U+DFFF";
                                return token_type::parse_error;
                            }
                        }
                        else
                        {
                            if (JSON_UNLIKELY(0xDC00 <= codepoint1 and codepoint1 <= 0xDFFF))
                            {
                                error_message = "invalid string: surrogate U+DC00..U+DFFF must follow U+D800..U+DBFF";
                                return token_type::parse_error;
                            }
                        }

                        // result of the above calculation yields a proper codepoint
                        assert(0x00 <= codepoint and codepoint <= 0x10FFFF);

                        // translate codepoint into bytes
                        if (codepoint < 0x80)
                        {
                            // 1-byte characters: 0xxxxxxx (ASCII)
                            add(codepoint);
                        }
                        else if (codepoint <= 0x7FF)
                        {
                            // 2-byte characters: 110xxxxx 10xxxxxx
                            add(0xC0 | (codepoint >> 6));
                            add(0x80 | (codepoint & 0x3F));
                        }
                        else if (codepoint <= 0xFFFF)
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
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
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
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            {
                error_message = "invalid string: control character must be escaped";
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
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
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
            case 0x3A:
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
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
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
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
            case 0x5A:
            case 0x5B:
            case 0x5D:
            case 0x5E:
            case 0x5F:
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
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
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
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
            case 0x7E:
            case 0x7F:
            {
                add(current);
                break;
            }

            // U+0080..U+07FF: bytes C2..DF 80..BF
            case 0xC2:
            case 0xC3:
            case 0xC4:
            case 0xC5:
            case 0xC6:
            case 0xC7:
            case 0xC8:
            case 0xC9:
            case 0xCA:
            case 0xCB:
            case 0xCC:
            case 0xCD:
            case 0xCE:
            case 0xCF:
            case 0xD0:
            case 0xD1:
            case 0xD2:
            case 0xD3:
            case 0xD4:
            case 0xD5:
            case 0xD6:
            case 0xD7:
            case 0xD8:
            case 0xD9:
            case 0xDA:
            case 0xDB:
            case 0xDC:
            case 0xDD:
            case 0xDE:
            case 0xDF:
            {
                if (JSON_UNLIKELY(not next_byte_in_range({0x80, 0xBF})))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+0800..U+0FFF: bytes E0 A0..BF 80..BF
            case 0xE0:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0xA0, 0xBF, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+1000..U+CFFF: bytes E1..EC 80..BF 80..BF
            // U+E000..U+FFFF: bytes EE..EF 80..BF 80..BF
            case 0xE1:
            case 0xE2:
            case 0xE3:
            case 0xE4:
            case 0xE5:
            case 0xE6:
            case 0xE7:
            case 0xE8:
            case 0xE9:
            case 0xEA:
            case 0xEB:
            case 0xEC:
            case 0xEE:
            case 0xEF:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0x80, 0xBF, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+D000..U+D7FF: bytes ED 80..9F 80..BF
            case 0xED:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0x80, 0x9F, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+10000..U+3FFFF F0 90..BF 80..BF 80..BF
            case 0xF0:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0x90, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+40000..U+FFFFF F1..F3 80..BF 80..BF 80..BF
            case 0xF1:
            case 0xF2:
            case 0xF3:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
            }

            // U+100000..U+10FFFF F4 80..8F 80..BF 80..BF
            case 0xF4:
            {
                if (JSON_UNLIKELY(not (next_byte_in_range({0x80, 0x8F, 0x80, 0xBF, 0x80, 0xBF}))))
                {
                    return token_type::parse_error;
                }
                break;
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

json::lexer::token_type json::lexer::scan_number()
{
    // reset token_buffer to store the number's bytes
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
            assert(false); // LCOV_EXCL_LINE
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
            goto scan_number_done;
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
            goto scan_number_done;
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
            goto scan_number_done;
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
            error_message =
                "invalid number; expected '+', '-', or digit after exponent";
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
            goto scan_number_done;
    }

scan_number_done:
    // unget the character after the number (we only read it to know that
    // we are done scanning a number)
    unget();

    char* endptr = nullptr;
    errno = 0;

    // try to parse integers first and fall back to floats
    if (number_type == token_type::value_unsigned)
    {
        const auto x = std::strtoull(token_buffer.c_str(), &endptr, 10);

        // we checked the number format before
        assert(endptr == token_buffer.data() + token_buffer.size());

        if (errno == 0)
        {
            value_unsigned = static_cast<uint64_t>(x);
            if (value_unsigned == x)
            {
                return token_type::value_unsigned;
            }
        }
    }
    else if (number_type == token_type::value_integer)
    {
        const auto x = std::strtoll(token_buffer.c_str(), &endptr, 10);

        // we checked the number format before
        assert(endptr == token_buffer.data() + token_buffer.size());

        if (errno == 0)
        {
            value_integer = static_cast<int64_t>(x);
            if (value_integer == x)
            {
                return token_type::value_integer;
            }
        }
    }

    // this code is reached if we parse a floating-point number or if an
    // integer conversion above failed
    strtof(value_float, token_buffer.c_str(), &endptr);

    // we checked the number format before
    assert(endptr == token_buffer.data() + token_buffer.size());

    return token_type::value_float;
}

json::lexer::token_type json::lexer::scan_literal(const char* literal_text, const std::size_t length,
                        token_type return_type)
{
    assert(current == literal_text[0]);
    for (std::size_t i = 1; i < length; ++i)
    {
        if (JSON_UNLIKELY(get() != literal_text[i]))
        {
            error_message = "invalid literal";
            return token_type::parse_error;
        }
    }
    return return_type;
}

std::string json::lexer::get_token_string() const
{
    // escape control characters
    std::string result;
    raw_string_ostream ss(result);
    for (const unsigned char c : token_string)
    {
        if (c <= '\x1F')
        {
            // escape control characters
            ss << "<U+" << format_hex_no_prefix(c, 4, true) << '>';
        }
        else
        {
            // add character as is
            ss << c;
        }
    }

    ss.flush();
    return result;
}

json::lexer::token_type json::lexer::scan()
{
    // read next character and ignore whitespace
    do
    {
        get();
    }
    while (current == ' ' or current == '\t' or current == '\n' or current == '\r');

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
            return scan_literal("true", 4, token_type::literal_true);
        case 'f':
            return scan_literal("false", 5, token_type::literal_false);
        case 'n':
            return scan_literal("null", 4, token_type::literal_null);

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

void json::parser::parse(const bool strict, json& result)
{
    // read first token
    get_token();

    parse_internal(true, result);
    result.assert_invariant();

    // in strict mode, input must be completely read
    if (strict)
    {
        get_token();
        expect(token_type::end_of_input);
    }

    // in case of an error, return discarded value
    if (errored)
    {
        result = value_t::discarded;
        return;
    }

    // set top-level value to null if it was discarded by the callback
    // function
    if (result.is_discarded())
    {
        result = nullptr;
    }
}

void json::parser::parse_internal(bool keep, json& result)
{
    // never parse after a parse error was detected
    assert(not errored);

    // start with a discarded value
    if (not result.is_discarded())
    {
        result.m_value.destroy(result.m_type);
        result.m_type = value_t::discarded;
    }

    switch (last_token)
    {
        case token_type::begin_object:
        {
            if (keep)
            {
                if (callback)
                {
                    keep = callback(depth++, parse_event_t::object_start, result);
                }

                if (not callback or keep)
                {
                    // explicitly set result to object to cope with {}
                    result.m_type = value_t::object;
                    result.m_value = value_t::object;
                }
            }

            // read next token
            get_token();

            // closing } -> we are done
            if (last_token == token_type::end_object)
            {
                if (keep and callback and not callback(--depth, parse_event_t::object_end, result))
                {
                    result.m_value.destroy(result.m_type);
                    result.m_type = value_t::discarded;
                }
                break;
            }

            // parse values
            SmallString<128> key;
            json value;
            while (true)
            {
                // store key
                if (not expect(token_type::value_string))
                {
                    return;
                }
                key = m_lexer.get_string();

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
                if (not expect(token_type::name_separator))
                {
                    return;
                }

                // parse and add value
                get_token();
                value.m_value.destroy(value.m_type);
                value.m_type = value_t::discarded;
                parse_internal(keep, value);

                if (JSON_UNLIKELY(errored))
                {
                    return;
                }

                if (keep and keep_tag and not value.is_discarded())
                {
                    result.m_value.object->try_emplace(StringRef(key.data(), key.size()), std::move(value));
                }

                // comma -> next value
                get_token();
                if (last_token == token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing }
                if (not expect(token_type::end_object))
                {
                    return;
                }
                break;
            }

            if (keep and callback and not callback(--depth, parse_event_t::object_end, result))
            {
                result.m_value.destroy(result.m_type);
                result.m_type = value_t::discarded;
            }
            break;
        }

        case token_type::begin_array:
        {
            if (keep)
            {
                if (callback)
                {
                    keep = callback(depth++, parse_event_t::array_start, result);
                }

                if (not callback or keep)
                {
                    // explicitly set result to array to cope with []
                    result.m_type = value_t::array;
                    result.m_value = value_t::array;
                }
            }

            // read next token
            get_token();

            // closing ] -> we are done
            if (last_token == token_type::end_array)
            {
                if (callback and not callback(--depth, parse_event_t::array_end, result))
                {
                    result.m_value.destroy(result.m_type);
                    result.m_type = value_t::discarded;
                }
                break;
            }

            // parse values
            json value;
            while (true)
            {
                // parse value
                value.m_value.destroy(value.m_type);
                value.m_type = value_t::discarded;
                parse_internal(keep, value);

                if (JSON_UNLIKELY(errored))
                {
                    return;
                }

                if (keep and not value.is_discarded())
                {
                    result.m_value.array->push_back(std::move(value));
                }

                // comma -> next value
                get_token();
                if (last_token == token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing ]
                if (not expect(token_type::end_array))
                {
                    return;
                }
                break;
            }

            if (keep and callback and not callback(--depth, parse_event_t::array_end, result))
            {
                result.m_value.destroy(result.m_type);
                result.m_type = value_t::discarded;
            }
            break;
        }

        case token_type::literal_null:
        {
            result.m_type = value_t::null;
            break;
        }

        case token_type::value_string:
        {
            result.m_type = value_t::string;
            result.m_value = m_lexer.get_string();
            break;
        }

        case token_type::literal_true:
        {
            result.m_type = value_t::boolean;
            result.m_value = true;
            break;
        }

        case token_type::literal_false:
        {
            result.m_type = value_t::boolean;
            result.m_value = false;
            break;
        }

        case token_type::value_unsigned:
        {
            result.m_type = value_t::number_unsigned;
            result.m_value = m_lexer.get_number_unsigned();
            break;
        }

        case token_type::value_integer:
        {
            result.m_type = value_t::number_integer;
            result.m_value = m_lexer.get_number_integer();
            break;
        }

        case token_type::value_float:
        {
            result.m_type = value_t::number_float;
            result.m_value = m_lexer.get_number_float();

            // throw in case of infinity or NAN
            if (JSON_UNLIKELY(not std::isfinite(result.m_value.number_float)))
            {
                if (allow_exceptions)
                {
                    JSON_THROW(out_of_range::create(406, "number overflow parsing '" +
                                                    Twine(m_lexer.get_token_string()) + "'"));
                }
                expect(token_type::uninitialized);
            }
            break;
        }

        case token_type::parse_error:
        {
            // using "uninitialized" to avoid "expected" message
            if (not expect(token_type::uninitialized))
            {
                return;
            }
            break; // LCOV_EXCL_LINE
        }

        default:
        {
            // the last token was unexpected; we expected a value
            if (not expect(token_type::literal_or_value))
            {
                return;
            }
            break; // LCOV_EXCL_LINE
        }
    }

    if (keep and callback and not callback(depth, parse_event_t::value, result))
    {
        result.m_value.destroy(result.m_type);
        result.m_type = value_t::discarded;
    }
}

bool json::parser::accept_internal()
{
    switch (last_token)
    {
        case token_type::begin_object:
        {
            // read next token
            get_token();

            // closing } -> we are done
            if (last_token == token_type::end_object)
            {
                return true;
            }

            // parse values
            while (true)
            {
                // parse key
                if (last_token != token_type::value_string)
                {
                    return false;
                }

                // parse separator (:)
                get_token();
                if (last_token != token_type::name_separator)
                {
                    return false;
                }

                // parse value
                get_token();
                if (not accept_internal())
                {
                    return false;
                }

                // comma -> next value
                get_token();
                if (last_token == token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing }
                return (last_token == token_type::end_object);
            }
        }

        case token_type::begin_array:
        {
            // read next token
            get_token();

            // closing ] -> we are done
            if (last_token == token_type::end_array)
            {
                return true;
            }

            // parse values
            while (true)
            {
                // parse value
                if (not accept_internal())
                {
                    return false;
                }

                // comma -> next value
                get_token();
                if (last_token == token_type::value_separator)
                {
                    get_token();
                    continue;
                }

                // closing ]
                return (last_token == token_type::end_array);
            }
        }

        case token_type::value_float:
        {
            // reject infinity or NAN
            return std::isfinite(m_lexer.get_number_float());
        }

        case token_type::literal_false:
        case token_type::literal_null:
        case token_type::literal_true:
        case token_type::value_integer:
        case token_type::value_string:
        case token_type::value_unsigned:
            return true;

        default: // the last token was unexpected
            return false;
    }
}

void json::parser::throw_exception() const
{
    std::string error_msg = "syntax error - ";
    if (last_token == token_type::parse_error)
    {
        error_msg += std::string(m_lexer.get_error_message()) + "; last read: '" +
                     m_lexer.get_token_string() + "'";
    }
    else
    {
        error_msg += "unexpected " + std::string(lexer_t::token_type_name(last_token));
    }

    if (expected != token_type::uninitialized)
    {
        error_msg += "; expected " + std::string(lexer_t::token_type_name(expected));
    }

    JSON_THROW(parse_error::create(101, m_lexer.get_position(), error_msg));
}

json json::parse(StringRef s,
                        const parser_callback_t cb,
                        const bool allow_exceptions)
{
    raw_mem_istream is(makeArrayRef(s.data(), s.size()));
    return parse(is, cb, allow_exceptions);
}

json json::parse(ArrayRef<uint8_t> arr,
                        const parser_callback_t cb,
                        const bool allow_exceptions)
{
    raw_mem_istream is(arr);
    return parse(is, cb, allow_exceptions);
}

json json::parse(raw_istream& i,
                        const parser_callback_t cb,
                        const bool allow_exceptions)
{
    json result;
    parser(i, cb, allow_exceptions).parse(true, result);
    return result;
}

bool json::accept(StringRef s)
{
    raw_mem_istream is(makeArrayRef(s.data(), s.size()));
    return parser(is).accept(true);
}

bool json::accept(ArrayRef<uint8_t> arr)
{
    raw_mem_istream is(arr);
    return parser(is).accept(true);
}

bool json::accept(raw_istream& i)
{
    return parser(i).accept(true);
}

raw_istream& operator>>(raw_istream& i, json& j)
{
    json::parser(i).parse(false, j);
    return i;
}

}  // namespace wpi
