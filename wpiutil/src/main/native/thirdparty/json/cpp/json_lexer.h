#pragma once

#include "wpi/json.h"

#include <clocale>
#include <cmath>
#include <cstdlib>

#include "fmt/format.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_istream.h"
#include "wpi/raw_ostream.h"

namespace wpi {
///////////
// lexer //
///////////

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
    std::string_view get_string()
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

}  // namespace wpi
