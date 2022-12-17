#define WPI_JSON_IMPLEMENTATION
#include "json_lexer.h"

namespace wpi {

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
            ss << fmt::format("<U+{:04X}>", c);
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

}  // namespace wpi
#undef WPI_JSON_IMPLEMENTATION
