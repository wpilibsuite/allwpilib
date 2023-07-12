#define WPI_JSON_IMPLEMENTATION
#include "wpi/json.h"

#include "fmt/format.h"
#include "wpi/SmallString.h"
#include "wpi/raw_os_ostream.h"

#include "wpi/detail/output/serializer.h"

namespace wpi
{

void json::serializer::dump(const json& val, const bool pretty_print,
          const bool ensure_ascii,
          const unsigned int indent_step,
          const unsigned int current_indent)
{
    switch (val.m_type)
    {
        case value_t::object:
        {
            if (val.m_value.object->empty())
            {
                o << "{}";
                return;
            }

            // we need to iterate over the object values in sorted key order
            SmallVector<StringMapConstIterator<json>, 64> sorted;
            for (auto i = val.m_value.object->begin(),
                 end = val.m_value.object->end(); i != end; ++i)
            {
                sorted.push_back(i);
            }
            std::sort(sorted.begin(), sorted.end(),
                      [](const StringMapConstIterator<json>& a,
                         const StringMapConstIterator<json>& b) {
                        return a->getKey() < b->getKey();
                      });

            if (pretty_print)
            {
                o << "{\n";

                // variable to hold indentation for recursive calls
                const auto new_indent = current_indent + indent_step;
                if (JSON_UNLIKELY(indent_string.size() < new_indent))
                {
                    indent_string.resize(indent_string.size() * 2, indent_char);
                }

                // first n-1 elements
                auto i = sorted.begin();
                for (std::size_t cnt = 0; cnt < sorted.size() - 1; ++cnt, ++i)
                {
                    o.write(indent_string.c_str(), new_indent);
                    o << '\"';
                    dump_escaped((*i)->first(), ensure_ascii);
                    o << "\": ";
                    dump((*i)->second, true, ensure_ascii, indent_step, new_indent);
                    o << ",\n";
                }

                // last element
                assert(i != sorted.end());
                //assert(std::next(i) == val.m_value.object->end());
                o.write(indent_string.c_str(), new_indent);
                o << '\"';
                dump_escaped((*i)->first(), ensure_ascii);
                o << "\": ";
                dump((*i)->second, true, ensure_ascii, indent_step, new_indent);

                o << '\n';
                o.write(indent_string.c_str(), current_indent);
                o << '}';
            }
            else
            {
                o << '{';

                // first n-1 elements
                auto i = sorted.begin();
                for (std::size_t cnt = 0; cnt < sorted.size() - 1; ++cnt, ++i)
                {
                    o << '\"';
                    dump_escaped((*i)->first(), ensure_ascii);
                    o << "\":";
                    dump((*i)->second, false, ensure_ascii, indent_step, current_indent);
                    o << ',';
                }

                // last element
                assert(i != sorted.end());
                //assert(std::next(i) == val.m_value.object->end());
                o << '\"';
                dump_escaped((*i)->first(), ensure_ascii);
                o << "\":";
                dump((*i)->second, false, ensure_ascii, indent_step, current_indent);

                o << '}';
            }

            return;
        }

        case value_t::array:
        {
            if (val.m_value.array->empty())
            {
                o << "[]";
                return;
            }

            if (pretty_print)
            {
                o << "[\n";

                // variable to hold indentation for recursive calls
                const auto new_indent = current_indent + indent_step;
                if (JSON_UNLIKELY(indent_string.size() < new_indent))
                {
                    indent_string.resize(indent_string.size() * 2, indent_char);
                }

                // first n-1 elements
                for (auto i = val.m_value.array->cbegin();
                        i != val.m_value.array->cend() - 1; ++i)
                {
                    o.write(indent_string.c_str(), new_indent);
                    dump(*i, true, ensure_ascii, indent_step, new_indent);
                    o << ",\n";
                }

                // last element
                assert(not val.m_value.array->empty());
                o.write(indent_string.c_str(), new_indent);
                dump(val.m_value.array->back(), true, ensure_ascii, indent_step, new_indent);

                o << '\n';
                o.write(indent_string.c_str(), current_indent);
                o << ']';
            }
            else
            {
                o << '[';

                // first n-1 elements
                for (auto i = val.m_value.array->cbegin();
                        i != val.m_value.array->cend() - 1; ++i)
                {
                    dump(*i, false, ensure_ascii, indent_step, current_indent);
                    o << ',';
                }

                // last element
                assert(not val.m_value.array->empty());
                dump(val.m_value.array->back(), false, ensure_ascii, indent_step, current_indent);

                o << ']';
            }

            return;
        }

        case value_t::string:
        {
            o << '\"';
            dump_escaped(*val.m_value.string, ensure_ascii);
            o << '\"';
            return;
        }

        case value_t::boolean:
        {
            if (val.m_value.boolean)
            {
                o << "true";
            }
            else
            {
                o << "false";
            }
            return;
        }

        case value_t::number_integer:
        {
            dump_integer(val.m_value.number_integer);
            return;
        }

        case value_t::number_unsigned:
        {
            dump_integer(val.m_value.number_unsigned);
            return;
        }

        case value_t::number_float:
        {
            dump_float(val.m_value.number_float);
            return;
        }

        case value_t::discarded:
        {
            o << "<discarded>";
            return;
        }

        case value_t::null:
        {
            o << "null";
            return;
        }
    }
}

void json::serializer::dump_escaped(std::string_view s, const bool ensure_ascii)
{
    uint32_t codepoint;
    uint8_t state = UTF8_ACCEPT;

    for (std::size_t i = 0; i < s.size(); ++i)
    {
        const auto byte = static_cast<uint8_t>(s[i]);

        switch (decode(state, codepoint, byte))
        {
            case UTF8_ACCEPT:  // decode found a new code point
            {
                switch (codepoint)
                {
                    case 0x08: // backspace
                    {
                        o << '\\' << 'b';
                        break;
                    }

                    case 0x09: // horizontal tab
                    {
                        o << '\\' << 't';
                        break;
                    }

                    case 0x0A: // newline
                    {
                        o << '\\' << 'n';
                        break;
                    }

                    case 0x0C: // formfeed
                    {
                        o << '\\' << 'f';
                        break;
                    }

                    case 0x0D: // carriage return
                    {
                        o << '\\' << 'r';
                        break;
                    }

                    case 0x22: // quotation mark
                    {
                        o << '\\' << '\"';
                        break;
                    }

                    case 0x5C: // reverse solidus
                    {
                        o << '\\' << '\\';
                        break;
                    }

                    default:
                    {
                        // escape control characters (0x00..0x1F) or, if
                        // ensure_ascii parameter is used, non-ASCII characters
                        if ((codepoint <= 0x1F) or (ensure_ascii and (codepoint >= 0x7F)))
                        {
                            if (codepoint <= 0xFFFF)
                            {
                                o << fmt::format("\\u{:04x}", codepoint);
                            }
                            else
                            {
                                o << fmt::format("\\u{:04x}", 0xD7C0 + (codepoint >> 10));
                                o << fmt::format("\\u{:04x}", 0xDC00 + (codepoint & 0x3FF));
                            }
                        }
                        else
                        {
                            // copy byte to buffer (all previous bytes
                            // been copied have in default case above)
                            o << s[i];
                        }
                        break;
                    }
                }
                break;
            }

            case UTF8_REJECT:  // decode found invalid UTF-8 byte
            {
                JSON_THROW(type_error::create(316, fmt::format("invalid UTF-8 byte at index {}: {:#02x}", i, byte)));
            }

            default:  // decode found yet incomplete multi-byte code point
            {
                if (not ensure_ascii)
                {
                    // code point will not be escaped - copy byte to buffer
                    o << s[i];
                }
                break;
            }
        }
    }

    if (JSON_UNLIKELY(state != UTF8_ACCEPT))
    {
        // we finish reading, but do not accept: string was incomplete
        JSON_THROW(type_error::create(316, fmt::format("incomplete UTF-8 string; last byte: {:#02x}", s.back())));
    }
}

}  // namespace wpi
