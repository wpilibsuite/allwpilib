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

#include "llvm/SmallString.h"
#include "llvm/StringExtras.h"

#include "json_serializer.h"

using namespace wpi;

void json::serializer::dump(const json& val,
          const bool pretty_print,
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

            if (pretty_print)
            {
                o << "{\n";

                // variable to hold indentation for recursive calls
                const auto new_indent = current_indent + indent_step;

                // first n-1 elements
                auto i = val.m_value.object->begin();
                for (size_t cnt = 0; cnt < val.m_value.object->size() - 1; ++cnt, ++i)
                {
                    o.indent(new_indent);
                    o << '\"';
                    dump_escaped(i->first());
                    o << "\": ";
                    dump(i->second, true, indent_step, new_indent);
                    o << ",\n";
                }

                // last element
                assert(i != val.m_value.object->end());
                o.indent(new_indent);
                o << '\"';
                dump_escaped(i->first());
                o << "\": ";
                dump(i->second, true, indent_step, new_indent);

                o << '\n';
                o.indent(current_indent);
                o << '}';
            }
            else
            {
                o << '{';

                // first n-1 elements
                auto i = val.m_value.object->begin();
                for (size_t cnt = 0; cnt < val.m_value.object->size() - 1; ++cnt, ++i)
                {
                    o << '\"';
                    dump_escaped(i->first());
                    o << "\":";
                    dump(i->second, false, indent_step, current_indent);
                    o << ',';
                }

                // last element
                assert(i != val.m_value.object->end());
                o << '\"';
                dump_escaped(i->first());
                o << "\":";
                dump(i->second, false, indent_step, current_indent);

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

                // first n-1 elements
                for (auto i = val.m_value.array->cbegin(); i != val.m_value.array->cend() - 1; ++i)
                {
                    o.indent(new_indent);
                    dump(*i, true, indent_step, new_indent);
                    o << ",\n";
                }

                // last element
                assert(!val.m_value.array->empty());
                o.indent(new_indent);
                dump(val.m_value.array->back(), true, indent_step, new_indent);

                o << '\n';
                o.indent(current_indent);
                o << ']';
            }
            else
            {
                o << '[';

                // first n-1 elements
                for (auto i = val.m_value.array->cbegin(); i != val.m_value.array->cend() - 1; ++i)
                {
                    dump(*i, false, indent_step, current_indent);
                    o << ',';
                }

                // last element
                assert(!val.m_value.array->empty());
                dump(val.m_value.array->back(), false, indent_step, current_indent);

                o << ']';
            }

            return;
        }

        case value_t::string:
        {
            o << '\"';
            dump_escaped(*val.m_value.string);
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
            o << static_cast<long long>(val.m_value.number_integer);
            return;
        }

        case value_t::number_unsigned:
        {
            o << static_cast<unsigned long long>(val.m_value.number_unsigned);
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

void json::serializer::dump_escaped(llvm::StringRef s) const
{
    for (const auto& c : s)
    {
        switch (c)
        {
            // quotation mark (0x22)
            case '"':
            {
                o << '\\' << '"';
                break;
            }

            // reverse solidus (0x5c)
            case '\\':
            {
                // nothing to change
                o << '\\' << '\\';
                break;
            }

            // backspace (0x08)
            case '\b':
            {
                o << '\\' << 'b';
                break;
            }

            // formfeed (0x0c)
            case '\f':
            {
                o << '\\' << 'f';
                break;
            }

            // newline (0x0a)
            case '\n':
            {
                o << '\\' << 'n';
                break;
            }

            // carriage return (0x0d)
            case '\r':
            {
                o << '\\' << 'r';
                break;
            }

            // horizontal tab (0x09)
            case '\t':
            {
                o << '\\' << 't';
                break;
            }

            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x0b:
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
                // print character c as \uxxxx
                o << "\\u00";
                o << llvm::hexdigit((c >> 4) & 0xf, true);
                o << llvm::hexdigit((c >> 0) & 0xf, true);
                break;
            }

            default:
            {
                // all other characters are added as-is
                o << c;
                break;
            }
        }
    }
}

void json::serializer::dump_float(double x)
{
    // NaN / inf
    if (!std::isfinite(x) || std::isnan(x))
    {
        o << "null";
        return;
    }

    // special case for 0.0 and -0.0
    if (x == 0)
    {
        if (std::signbit(x))
        {
            o << "-0.0";
        }
        else
        {
            o << "0.0";
        }
        return;
    }

    // get number of digits for a text -> float -> text round-trip
    static constexpr auto d = std::numeric_limits<double>::digits10;

    // the actual conversion
    llvm::SmallString<64> number_buffer;
    number_buffer.resize(64);
    std::ptrdiff_t len = snprintf(number_buffer.data(), number_buffer.size(),
                                  "%.*g", d, x);

    // negative value indicates an error
    assert(len > 0);
    // check if buffer was large enough
    assert(static_cast<size_t>(len) < number_buffer.size());

    // erase thousands separator
    if (thousands_sep != '\0')
    {
        const auto end = std::remove(number_buffer.begin(),
                                     number_buffer.begin() + len,
                                     thousands_sep);
        std::fill(end, number_buffer.end(), '\0');
        assert((end - number_buffer.begin()) <= len);
        len = (end - number_buffer.begin());
    }

    // convert decimal point to '.'
    if (decimal_point != '\0' && decimal_point != '.')
    {
        for (auto& c : number_buffer)
        {
            if (c == decimal_point)
            {
                c = '.';
                break;
            }
        }
    }

    o.write(number_buffer.data(), static_cast<size_t>(len));

    // determine if need to append ".0"
    const bool value_is_int_like = std::none_of(number_buffer.begin(),
                                   number_buffer.begin() + len + 1,
                                   [](char c)
    {
        return c == '.' || c == 'e';
    });

    if (value_is_int_like)
    {
        o << ".0";
    }
}

namespace wpi {

llvm::raw_ostream& operator<<(llvm::raw_ostream& o, const json& j)
{
    j.dump(o, 0);
    return o;
}

}  // namespace wpi

std::string json::dump(int indent) const
{
    std::string s;
    llvm::raw_string_ostream os(s);
    dump(os, indent);
    os.flush();
    return s;
}

void json::dump(llvm::raw_ostream& os, int indent) const
{
    serializer s(os);

    if (indent >= 0)
    {
        s.dump(*this, true, static_cast<unsigned int>(indent));
    }
    else
    {
        s.dump(*this, false, 0);
    }
}
