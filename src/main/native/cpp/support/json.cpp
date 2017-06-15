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

using namespace wpi;

namespace wpi {
namespace detail {

std::string exception::name(const std::string& ename, int id)
{
    return "[json.exception." + ename + "." + std::to_string(id) + "] ";
}

parse_error parse_error::create(int id, size_t byte_, const std::string& what_arg)
{
    std::string w = exception::name("parse_error", id) + "parse error" +
                    (byte_ != 0 ? (" at " + std::to_string(byte_)) : "") +
                    ": " + what_arg;
    return parse_error(id, byte_, w.c_str());
}

invalid_iterator invalid_iterator::create(int id, const std::string& what_arg)
{
    std::string w = exception::name("invalid_iterator", id) + what_arg;
    return invalid_iterator(id, w.c_str());
}

type_error type_error::create(int id, const std::string& what_arg)
{
    std::string w = exception::name("type_error", id) + what_arg;
    return type_error(id, w.c_str());
}

out_of_range out_of_range::create(int id, const std::string& what_arg)
{
    std::string w = exception::name("out_of_range", id) + what_arg;
    return out_of_range(id, w.c_str());
}

other_error other_error::create(int id, const std::string& what_arg)
{
    std::string w = exception::name("other_error", id) + what_arg;
    return other_error(id, w.c_str());
}

}  // namespace detail
}  // namespace wpi

json::json_value::json_value(value_t t)
{
    switch (t)
    {
        case value_t::object:
        {
            object = create<object_t>();
            break;
        }

        case value_t::array:
        {
            array = create<array_t>();
            break;
        }

        case value_t::string:
        {
            string = create<std::string>("");
            break;
        }

        case value_t::boolean:
        {
            boolean = false;
            break;
        }

        case value_t::number_integer:
        {
            number_integer = 0;
            break;
        }

        case value_t::number_unsigned:
        {
            number_unsigned = 0;
            break;
        }

        case value_t::number_float:
        {
            number_float = 0.0;
            break;
        }

        case value_t::null:
        {
            break;
        }

        default:
        {
            if (JSON_UNLIKELY(t == value_t::null))
            {
                JSON_THROW(other_error::create(500, "961c151d2e87f2686a955a9be24d316f1362bf21 2.1.1")); // LCOV_EXCL_LINE
            }
            break;
        }
    }
}

json::json_value::json_value(llvm::StringRef value)
{
    string = create<std::string>(value);
}

json::json_value::json_value(const std::string& value)
{
    string = create<std::string>(value);
}

json::json_value::json_value(const object_t& value)
{
    object = create<object_t>(value);
}

json::json_value::json_value(const array_t& value)
{
    array = create<array_t>(value);
}

json::json(std::initializer_list<json> init,
           bool type_deduction,
           value_t manual_type)
{
    // check if each element is an array with two elements whose first
    // element is a string
    bool is_an_object = std::all_of(init.begin(), init.end(),
                                    [](const json & element)
    {
        return element.is_array() && element.size() == 2 && element[0].is_string();
    });

    // adjust type if type deduction is not wanted
    if (!type_deduction)
    {
        // if array is wanted, do not create an object though possible
        if (manual_type == value_t::array)
        {
            is_an_object = false;
        }

        // if object is wanted but impossible, throw an exception
        if (manual_type == value_t::object && !is_an_object)
        {
            JSON_THROW(type_error::create(301, "cannot create object from initializer list"));
        }
    }

    if (is_an_object)
    {
        // the initializer list is a list of pairs -> create object
        m_type = value_t::object;
        m_value = value_t::object;

        std::for_each(init.begin(), init.end(), [this](const json & element)
        {
            m_value.object->emplace_second(*(element[0].m_value.string), element[1]);
        });
    }
    else
    {
        // the initializer list describes an array -> create array
        m_type = value_t::array;
        m_value.array = create<array_t>(init);
    }

    assert_invariant();
}

json::json(size_type cnt, const json& val)
    : m_type(value_t::array)
{
    m_value.array = create<array_t>(cnt, val);
    assert_invariant();
}

json::json(const json& other)
    : m_type(other.m_type)
{
    // check of passed value is valid
    other.assert_invariant();

    switch (m_type)
    {
        case value_t::object:
        {
            m_value = *other.m_value.object;
            break;
        }

        case value_t::array:
        {
            m_value = *other.m_value.array;
            break;
        }

        case value_t::string:
        {
            m_value = *other.m_value.string;
            break;
        }

        case value_t::boolean:
        {
            m_value = other.m_value.boolean;
            break;
        }

        case value_t::number_integer:
        {
            m_value = other.m_value.number_integer;
            break;
        }

        case value_t::number_unsigned:
        {
            m_value = other.m_value.number_unsigned;
            break;
        }

        case value_t::number_float:
        {
            m_value = other.m_value.number_float;
            break;
        }

        default:
        {
            break;
        }
    }

    assert_invariant();
}

json::~json()
{
    assert_invariant();

    switch (m_type)
    {
        case value_t::object:
        {
            std::allocator<object_t> alloc;
            alloc.destroy(m_value.object);
            alloc.deallocate(m_value.object, 1);
            break;
        }

        case value_t::array:
        {
            std::allocator<array_t> alloc;
            alloc.destroy(m_value.array);
            alloc.deallocate(m_value.array, 1);
            break;
        }

        case value_t::string:
        {
            std::allocator<std::string> alloc;
            alloc.destroy(m_value.string);
            alloc.deallocate(m_value.string, 1);
            break;
        }

        default:
        {
            // all other types need no specific destructor
            break;
        }
    }
}

json json::meta()
{
    json result;

    result["copyright"] = "(C) 2013-2017 Niels Lohmann, (C) 2017 FIRST";
    result["name"] = "WPI version of JSON for Modern C++";
    result["url"] = "https://github.com/wpilibsuite/wpiutil";
    result["version"] =
    {
        {"string", "2.1.1"}, {"major", 2}, {"minor", 1}, {"patch", 1}
    };

#ifdef _WIN32
    result["platform"] = "win32";
#elif defined __linux__
    result["platform"] = "linux";
#elif defined __APPLE__
    result["platform"] = "apple";
#elif defined __unix__
    result["platform"] = "unix";
#else
    result["platform"] = "unknown";
#endif

#if defined(__clang__)
    result["compiler"] = {{"family", "clang"}, {"version", __clang_version__}};
#elif defined(__ICC) || defined(__INTEL_COMPILER)
    result["compiler"] = {{"family", "icc"}, {"version", __INTEL_COMPILER}};
#elif defined(__GNUC__) || defined(__GNUG__)
    result["compiler"] = {{"family", "gcc"}, {"version", std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__)}};
#elif defined(__HP_cc) || defined(__HP_aCC)
    result["compiler"] = "hp"
#elif defined(__IBMCPP__)
    result["compiler"] = {{"family", "ilecpp"}, {"version", __IBMCPP__}};
#elif defined(_MSC_VER)
    result["compiler"] = {{"family", "msvc"}, {"version", _MSC_VER}};
#elif defined(__PGI)
    result["compiler"] = {{"family", "pgcpp"}, {"version", __PGI}};
#elif defined(__SUNPRO_CC)
    result["compiler"] = {{"family", "sunpro"}, {"version", __SUNPRO_CC}};
#else
    result["compiler"] = {{"family", "unknown"}, {"version", "unknown"}};
#endif

#ifdef __cplusplus
    result["compiler"]["c++"] = std::to_string(__cplusplus);
#else
    result["compiler"]["c++"] = "unknown";
#endif
    return result;
}

namespace wpi {

bool operator==(json::const_reference lhs, json::const_reference rhs) noexcept
{
    using value_t = json::value_t;
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type)
    {
        switch (lhs_type)
        {
            case value_t::array:
            {
                return *lhs.m_value.array == *rhs.m_value.array;
            }
            case value_t::object:
            {
                return *lhs.m_value.object == *rhs.m_value.object;
            }
            case value_t::null:
            {
                return true;
            }
            case value_t::string:
            {
                return *lhs.m_value.string == *rhs.m_value.string;
            }
            case value_t::boolean:
            {
                return lhs.m_value.boolean == rhs.m_value.boolean;
            }
            case value_t::number_integer:
            {
                return lhs.m_value.number_integer == rhs.m_value.number_integer;
            }
            case value_t::number_unsigned:
            {
                return lhs.m_value.number_unsigned == rhs.m_value.number_unsigned;
            }
            case value_t::number_float:
            {
                return lhs.m_value.number_float == rhs.m_value.number_float;
            }
            default:
            {
                return false;
            }
        }
    }
    else if (lhs_type == value_t::number_integer && rhs_type == value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_integer) == rhs.m_value.number_float;
    }
    else if (lhs_type == value_t::number_float && rhs_type == value_t::number_integer)
    {
        return lhs.m_value.number_float == static_cast<double>(rhs.m_value.number_integer);
    }
    else if (lhs_type == value_t::number_unsigned && rhs_type == value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_unsigned) == rhs.m_value.number_float;
    }
    else if (lhs_type == value_t::number_float && rhs_type == value_t::number_unsigned)
    {
        return lhs.m_value.number_float == static_cast<double>(rhs.m_value.number_unsigned);
    }
    else if (lhs_type == value_t::number_unsigned && rhs_type == value_t::number_integer)
    {
        return static_cast<std::int64_t>(lhs.m_value.number_unsigned) == rhs.m_value.number_integer;
    }
    else if (lhs_type == value_t::number_integer && rhs_type == value_t::number_unsigned)
    {
        return lhs.m_value.number_integer == static_cast<std::int64_t>(rhs.m_value.number_unsigned);
    }

    return false;
}

bool operator<(json::const_reference lhs, json::const_reference rhs) noexcept
{
    using value_t = json::value_t;
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type)
    {
        switch (lhs_type)
        {
            case value_t::array:
            {
                return (*lhs.m_value.array) < (*rhs.m_value.array);
            }
            case value_t::object:
            {
                return *lhs.m_value.object < *rhs.m_value.object;
            }
            case value_t::null:
            {
                return false;
            }
            case value_t::string:
            {
                return *lhs.m_value.string < *rhs.m_value.string;
            }
            case value_t::boolean:
            {
                return lhs.m_value.boolean < rhs.m_value.boolean;
            }
            case value_t::number_integer:
            {
                return lhs.m_value.number_integer < rhs.m_value.number_integer;
            }
            case value_t::number_unsigned:
            {
                return lhs.m_value.number_unsigned < rhs.m_value.number_unsigned;
            }
            case value_t::number_float:
            {
                return lhs.m_value.number_float < rhs.m_value.number_float;
            }
            default:
            {
                return false;
            }
        }
    }
    else if (lhs_type == value_t::number_integer && rhs_type == value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_integer) < rhs.m_value.number_float;
    }
    else if (lhs_type == value_t::number_float && rhs_type == value_t::number_integer)
    {
        return lhs.m_value.number_float < static_cast<double>(rhs.m_value.number_integer);
    }
    else if (lhs_type == value_t::number_unsigned && rhs_type == value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_unsigned) < rhs.m_value.number_float;
    }
    else if (lhs_type == value_t::number_float && rhs_type == value_t::number_unsigned)
    {
        return lhs.m_value.number_float < static_cast<double>(rhs.m_value.number_unsigned);
    }
    else if (lhs_type == value_t::number_integer && rhs_type == value_t::number_unsigned)
    {
        return lhs.m_value.number_integer < static_cast<std::int64_t>(rhs.m_value.number_unsigned);
    }
    else if (lhs_type == value_t::number_unsigned && rhs_type == value_t::number_integer)
    {
        return static_cast<std::int64_t>(lhs.m_value.number_unsigned) < rhs.m_value.number_integer;
    }

    // We only reach this line if we cannot compare values. In that case,
    // we compare types. Note we have to call the operator explicitly,
    // because MSVC has problems otherwise.
    return operator<(lhs_type, rhs_type);
}

}  // namespace wpi

std::string json::type_name() const
{
    {
        switch (m_type)
        {
            case value_t::null:
                return "null";
            case value_t::object:
                return "object";
            case value_t::array:
                return "array";
            case value_t::string:
                return "string";
            case value_t::boolean:
                return "boolean";
            case value_t::discarded:
                return "discarded";
            default:
                return "number";
        }
    }
}

bool json::empty() const noexcept
{
    switch (m_type)
    {
        case value_t::null:
        {
            // null values are empty
            return true;
        }

        case value_t::array:
        {
            // delegate call to array_t::empty()
            return m_value.array->empty();
        }

        case value_t::object:
        {
            // delegate call to object_t::empty()
            return m_value.object->empty();
        }

        default:
        {
            // all other types are nonempty
            return false;
        }
    }
}

json::size_type json::size() const noexcept
{
    switch (m_type)
    {
        case value_t::null:
        {
            // null values are empty
            return 0;
        }

        case value_t::array:
        {
            // delegate call to array_t::size()
            return m_value.array->size();
        }

        case value_t::object:
        {
            // delegate call to object_t::size()
            return m_value.object->size();
        }

        default:
        {
            // all other types have size 1
            return 1;
        }
    }
}

json::size_type json::max_size() const noexcept
{
    switch (m_type)
    {
        case value_t::array:
        {
            // delegate call to array_t::max_size()
            return m_value.array->max_size();
        }

        case value_t::object:
        {
            // delegate call to std::allocator<json>::max_size()
            return std::allocator<json>().max_size();
        }

        default:
        {
            // all other types have max_size() == size()
            return size();
        }
    }
}

void json::clear() noexcept
{
    switch (m_type)
    {
        case value_t::number_integer:
        {
            m_value.number_integer = 0;
            break;
        }

        case value_t::number_unsigned:
        {
            m_value.number_unsigned = 0;
            break;
        }

        case value_t::number_float:
        {
            m_value.number_float = 0.0;
            break;
        }

        case value_t::boolean:
        {
            m_value.boolean = false;
            break;
        }

        case value_t::string:
        {
            m_value.string->clear();
            break;
        }

        case value_t::array:
        {
            m_value.array->clear();
            break;
        }

        case value_t::object:
        {
            m_value.object->clear();
            break;
        }

        default:
        {
            break;
        }
    }
}

void json::push_back(json&& val)
{
    // push_back only works for null objects or arrays
    if (!(is_null() || is_array()))
    {
        JSON_THROW(type_error::create(308, "cannot use push_back() with " + type_name()));
    }

    // transform null object into an array
    if (is_null())
    {
        m_type = value_t::array;
        m_value = value_t::array;
        assert_invariant();
    }

    // add element to array (move semantics)
    m_value.array->push_back(std::move(val));
    // invalidate object
    val.m_type = value_t::null;
}

void json::push_back(const json& val)
{
    // push_back only works for null objects or arrays
    if (!(is_null() || is_array()))
    {
        JSON_THROW(type_error::create(308, "cannot use push_back() with " + type_name()));
    }

    // transform null object into an array
    if (is_null())
    {
        m_type = value_t::array;
        m_value = value_t::array;
        assert_invariant();
    }

    // add element to array
    m_value.array->push_back(val);
}

void json::push_back(const std::pair<llvm::StringRef, json>& val)
{
    // push_back only works for null objects or objects
    if (!(is_null() || is_object()))
    {
        JSON_THROW(type_error::create(308, "cannot use push_back() with " + type_name()));
    }

    // transform null object into an object
    if (is_null())
    {
        m_type = value_t::object;
        m_value = value_t::object;
        assert_invariant();
    }

    // add element to array
    m_value.object->insert(val);
}

void json::push_back(std::initializer_list<json> init)
{
    if (is_object() && init.size() == 2 && init.begin()->is_string())
    {
        const std::string key = *init.begin();
        push_back(std::pair<llvm::StringRef, json>(key, *(init.begin() + 1)));
    }
    else
    {
        push_back(json(init));
    }
}

json::reference json::at(size_type idx)
{
    // at only works for arrays
    if (is_array())
    {
        JSON_TRY
        {
            return m_value.array->at(idx);
        }
        JSON_CATCH (std::out_of_range&)
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
        }
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + type_name()));
    }
}

json::const_reference json::at(size_type idx) const
{
    // at only works for arrays
    if (is_array())
    {
        JSON_TRY
        {
            return m_value.array->at(idx);
        }
        JSON_CATCH (std::out_of_range&)
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
        }
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + type_name()));
    }
}

json::reference json::at(llvm::StringRef key)
{
    // at only works for objects
    if (is_object())
    {
        auto it = m_value.object->find(key);
        if (it == m_value.object->end())
        {
            JSON_THROW(out_of_range::create(403, "key '" + key.str() + "' not found"));
        }
        return it->second;
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + type_name()));
    }
}

json::const_reference json::at(llvm::StringRef key) const
{
    // at only works for objects
    if (is_object())
    {
        auto it = m_value.object->find(key);
        if (it == m_value.object->end())
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(403, "key '" + key.str() + "' not found"));
        }
        return it->second;
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + type_name()));
    }
}

json::reference json::operator[](size_type idx)
{
    // implicitly convert null value to an empty array
    if (is_null())
    {
        m_type = value_t::array;
        m_value.array = create<array_t>();
        assert_invariant();
    }

    // operator[] only works for arrays
    if (is_array())
    {
        // fill up array with null values if given idx is outside range
        if (idx >= m_value.array->size())
        {
            m_value.array->insert(m_value.array->end(),
                                  idx - m_value.array->size() + 1,
                                  json());
        }

        return m_value.array->operator[](idx);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + type_name()));
}

json::const_reference json::operator[](size_type idx) const
{
    // const operator[] only works for arrays
    if (is_array())
    {
        return m_value.array->operator[](idx);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + type_name()));
}

json::reference json::operator[](llvm::StringRef key)
{
    // implicitly convert null value to an empty object
    if (is_null())
    {
        m_type = value_t::object;
        m_value.object = create<object_t>();
        assert_invariant();
    }

    // operator[] only works for objects
    if (is_object())
    {
        return m_value.object->operator[](key);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + type_name()));
}

json::const_reference json::operator[](llvm::StringRef key) const
{
    // const operator[] only works for objects
    if (is_object())
    {
        assert(m_value.object->find(key) != m_value.object->end());
        return m_value.object->find(key)->second;
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + type_name()));
}

json::reference json::back()
{
    switch (m_type)
    {
        case value_t::object:
        {
            JSON_THROW(type_error::create(305, "cannot use back() with " + type_name()));
        }

        case value_t::array:
        {
            return m_value.array->back();
        }

        case value_t::null:
        {
            JSON_THROW(invalid_iterator::create(214, "cannot get value"));
        }

        default:
        {
            return *this;
        }
    }
}

json::const_reference json::back() const
{
    switch (m_type)
    {
        case value_t::object:
        {
            JSON_THROW(type_error::create(305, "cannot use back() with " + type_name()));
        }

        case value_t::array:
        {
            return m_value.array->back();
        }

        case value_t::null:
        {
            JSON_THROW(invalid_iterator::create(214, "cannot get value"));
        }

        default:
        {
            return *this;
        }
    }
}

json::size_type json::erase(llvm::StringRef key)
{
    // this erase only works for objects
    if (is_object())
    {
        return m_value.object->erase(key);
    }

    JSON_THROW(type_error::create(307, "cannot use erase() with " + type_name()));
}

void json::erase(const size_type idx)
{
    // this erase only works for arrays
    if (is_array())
    {
        if (idx >= size())
        {
            JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
        }

        m_value.array->erase(m_value.array->begin() + static_cast<difference_type>(idx));
    }
    else
    {
        JSON_THROW(type_error::create(307, "cannot use erase() with " + type_name()));
    }
}

json::iterator json::find(llvm::StringRef key)
{
    auto result = end();

    if (is_object())
    {
        result.m_it.object_iterator = m_value.object->find(key);
    }

    return result;
}

json::const_iterator json::find(llvm::StringRef key) const
{
    auto result = cend();

    if (is_object())
    {
        result.m_it.object_iterator = m_value.object->find(key);
    }

    return result;
}

json::iterator json::insert(const_iterator pos, const json& val)
{
    // insert only works for arrays
    if (is_array())
    {
        // check if iterator pos fits to this JSON value
        if (pos.m_object != this)
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        iterator result(this);
        result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, val);
        return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + type_name()));
}

json::iterator json::insert(const_iterator pos, size_type cnt, const json& val)
{
    // insert only works for arrays
    if (is_array())
    {
        // check if iterator pos fits to this JSON value
        if (pos.m_object != this)
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        iterator result(this);
#if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
        // handle C++11 noncompliance: insert(it, cnt, val) returns void
        if (cnt == 0) {
            result.m_it.array_iterator = pos.m_it.array_iterator;
        } else {
            size_t ipos = pos.m_it.array_iterator - m_value.array->begin();
            m_value.array->insert(pos.m_it.array_iterator, cnt, val);
            result.m_it.array_iterator = m_value.array->begin() + ipos;
        }
#else
        result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, cnt, val);
#endif
        return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + type_name()));
}

json::iterator json::insert(const_iterator pos, const_iterator first, const_iterator last)
{
    // insert only works for arrays
    if (!is_array())
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + type_name()));
    }

    // check if iterator pos fits to this JSON value
    if (pos.m_object != this)
    {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // check if range iterators belong to the same JSON object
    if (first.m_object != last.m_object)
    {
        JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    if (first.m_object == this || last.m_object == this)
    {
        JSON_THROW(invalid_iterator::create(211, "passed iterators may not belong to container"));
    }

    // insert to array and return iterator
    iterator result(this);
#if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
    // handle C++11 noncompliance: insert(it, first, last) returns void
    if (first == last) {
        result.m_it.array_iterator = pos.m_it.array_iterator;
    } else {
        size_t ipos = pos.m_it.array_iterator - m_value.array->begin();
        m_value.array->insert(pos.m_it.array_iterator, first.m_it.array_iterator,
                              last.m_it.array_iterator);
        result.m_it.array_iterator = m_value.array->begin() + ipos;
    }
#else
    result.m_it.array_iterator = m_value.array->insert(
                                     pos.m_it.array_iterator,
                                     first.m_it.array_iterator,
                                     last.m_it.array_iterator);
#endif
    return result;
}

json::iterator json::insert(const_iterator pos, std::initializer_list<json> ilist)
{
    // insert only works for arrays
    if (!is_array())
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + type_name()));
    }

    // check if iterator pos fits to this JSON value
    if (pos.m_object != this)
    {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // insert to array and return iterator
    iterator result(this);
#if defined(__GNUC__) && (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
    // handle C++11 noncompliance: insert(it, ilist) returns void
    if (ilist.size() == 0) {
        result.m_it.array_iterator = pos.m_it.array_iterator;
    } else {
        size_t ipos = pos.m_it.array_iterator - m_value.array->begin();
        m_value.array->insert(pos.m_it.array_iterator, ilist);
        result.m_it.array_iterator = m_value.array->begin() + ipos;
    }
#else
    result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, ilist);
#endif
    return result;
}

void json::insert(const_iterator first, const_iterator last)
{
    // insert only works for objects
    if (!is_object())
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + type_name()));
    }

    // check if range iterators belong to the same JSON object
    if (first.m_object != last.m_object)
    {
        JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    // passed iterators must belong to objects
    if (!first.m_object->is_object() || !first.m_object->is_object())
    {
        JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
    }

    for (auto it = first.m_it.object_iterator, end = last.m_it.object_iterator; it != end; ++it)
    {
        m_value.object->insert(std::make_pair(it->first(), it->second));
    }
}

namespace std {

std::size_t hash<wpi::json>::operator()(const wpi::json& j) const
{
    // a naive hashing via the string representation
    const auto& h = hash<std::string>();
    llvm::SmallVector<char, 128> buf;
    llvm::raw_svector_ostream os(buf);
    j.dump(os);
    return h(os.str());
}

}  // namespace std
