#define WPI_JSON_IMPLEMENTATION
#include "json_parser.h"

namespace wpi {

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
                    result.m_value.object->try_emplace(std::string_view(key.data(), key.size()), std::move(value));
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
                    JSON_THROW(out_of_range::create(406,
                        fmt::format("number overflow parsing '{}'", m_lexer.get_token_string())));
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

}  // namespace wpi
