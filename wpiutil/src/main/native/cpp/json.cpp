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

#include "wpi/raw_ostream.h"

namespace wpi {
namespace detail {

exception::exception(int id_, const Twine& what_arg)
    : id(id_), m(what_arg.str()) {}

parse_error parse_error::create(int id_, std::size_t byte_, const Twine& what_arg)
{
    return parse_error(id_, byte_, "[json.exception.parse_error." + Twine(id_) + "] parse error" +
                    (byte_ != 0 ? (" at " + Twine(byte_)) : Twine("")) +
                    ": " + what_arg);
}

invalid_iterator invalid_iterator::create(int id_, const Twine& what_arg)
{
    return invalid_iterator(id_, "[json.exception.invalid_iterator." + Twine(id_) + "] " + what_arg);
}

type_error type_error::create(int id_, const Twine& what_arg)
{
    return type_error(id_, "[json.exception.type_error." + Twine(id_) + "] " + what_arg);
}

out_of_range out_of_range::create(int id_, const Twine& what_arg)
{
    return out_of_range(id_, "[json.exception.out_of_range." + Twine(id_) + "] " + what_arg);
}

other_error other_error::create(int id_, const Twine& what_arg)
{
    return other_error(id_, "[json.exception.other_error." + Twine(id_) + "] " + what_arg);
}

}  // namespace detail

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
            number_unsigned = 0u;
            break;
        }

        case value_t::number_float:
        {
            number_float = 0.0;
            break;
        }

        case value_t::null:
        {
            object = nullptr;  // silence warning, see #821
            break;
        }

        default:
        {
            object = nullptr;  // silence warning, see #821
            if (JSON_UNLIKELY(t == value_t::null))
            {
                JSON_THROW(other_error::create(500, "961c151d2e87f2686a955a9be24d316f1362bf21 3.1.2")); // LCOV_EXCL_LINE
            }
            break;
        }
    }
}

void json::json_value::destroy(value_t t) noexcept
{
    switch (t)
    {
        case value_t::object:
        {
            std::allocator<object_t> alloc;
            alloc.destroy(object);
            alloc.deallocate(object, 1);
            break;
        }

        case value_t::array:
        {
            std::allocator<array_t> alloc;
            alloc.destroy(array);
            alloc.deallocate(array, 1);
            break;
        }

        case value_t::string:
        {
            std::allocator<std::string> alloc;
            alloc.destroy(string);
            alloc.deallocate(string, 1);
            break;
        }

        default:
        {
            break;
        }
    }
}

json::json(initializer_list_t init,
           bool type_deduction,
           value_t manual_type)
{
    // check if each element is an array with two elements whose first
    // element is a string
    bool is_an_object = std::all_of(init.begin(), init.end(),
                                    [](const detail::json_ref<json>& element_ref)
    {
        return (element_ref->is_array() and element_ref->size() == 2 and (*element_ref)[0].is_string());
    });

    // adjust type if type deduction is not wanted
    if (not type_deduction)
    {
        // if array is wanted, do not create an object though possible
        if (manual_type == value_t::array)
        {
            is_an_object = false;
        }

        // if object is wanted but impossible, throw an exception
        if (JSON_UNLIKELY(manual_type == value_t::object and not is_an_object))
        {
            JSON_THROW(type_error::create(301, "cannot create object from initializer list"));
        }
    }

    if (is_an_object)
    {
        // the initializer list is a list of pairs -> create object
        m_type = value_t::object;
        m_value = value_t::object;

        std::for_each(init.begin(), init.end(), [this](const detail::json_ref<json>& element_ref)
        {
            auto element = element_ref.moved_or_copied();
            m_value.object->try_emplace(
                *((*element.m_value.array)[0].m_value.string),
                std::move((*element.m_value.array)[1]));
        });
    }
    else
    {
        // the initializer list describes an array -> create array
        m_type = value_t::array;
        m_value.array = create<array_t>(init.begin(), init.end());
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
            break;
    }

    assert_invariant();
}

json json::meta()
{
    json result;

    result["copyright"] = "(C) 2013-2017 Niels Lohmann, (C) 2017-2018 FIRST";
    result["name"] = "WPI version of JSON for Modern C++";
    result["url"] = "https://github.com/wpilibsuite/allwpilib";
    result["version"]["string"] =
        std::to_string(NLOHMANN_JSON_VERSION_MAJOR) + "." +
        std::to_string(NLOHMANN_JSON_VERSION_MINOR) + "." +
        std::to_string(NLOHMANN_JSON_VERSION_PATCH);
    result["version"]["major"] = NLOHMANN_JSON_VERSION_MAJOR;
    result["version"]["minor"] = NLOHMANN_JSON_VERSION_MINOR;
    result["version"]["patch"] = NLOHMANN_JSON_VERSION_PATCH;

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

#if defined(__ICC) || defined(__INTEL_COMPILER)
    result["compiler"] = {{"family", "icc"}, {"version", __INTEL_COMPILER}};
#elif defined(__clang__)
    result["compiler"] = {{"family", "clang"}, {"version", __clang_version__}};
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

json::reference json::at(size_type idx)
{
    // at only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        JSON_TRY
        {
            return m_value.array->at(idx);
        }
        JSON_CATCH (std::out_of_range&)
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(401, "array index " + Twine(idx) + " is out of range"));
        }
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + Twine(type_name())));
    }
}

json::const_reference json::at(size_type idx) const
{
    // at only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        JSON_TRY
        {
            return m_value.array->at(idx);
        }
        JSON_CATCH (std::out_of_range&)
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(401, "array index " + Twine(idx) + " is out of range"));
        }
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + Twine(type_name())));
    }
}

json::reference json::at(StringRef key)
{
    // at only works for objects
    if (JSON_LIKELY(is_object()))
    {
        auto it = m_value.object->find(key);
        if (it == m_value.object->end())
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(403, "key '" + Twine(key) + "' not found"));
        }
        return it->second;
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + Twine(type_name())));
    }
}

json::const_reference json::at(StringRef key) const
{
    // at only works for objects
    if (JSON_LIKELY(is_object()))
    {
        auto it = m_value.object->find(key);
        if (it == m_value.object->end())
        {
            // create better exception explanation
            JSON_THROW(out_of_range::create(403, "key '" + Twine(key) + "' not found"));
        }
        return it->second;
    }
    else
    {
        JSON_THROW(type_error::create(304, "cannot use at() with " + Twine(type_name())));
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
    if (JSON_LIKELY(is_array()))
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

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
}

json::const_reference json::operator[](size_type idx) const
{
    // const operator[] only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        return m_value.array->operator[](idx);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
}

json::reference json::operator[](StringRef key)
{
    // implicitly convert null value to an empty object
    if (is_null())
    {
        m_type = value_t::object;
        m_value.object = create<object_t>();
        assert_invariant();
    }

    // operator[] only works for objects
    if (JSON_LIKELY(is_object()))
    {
        return m_value.object->operator[](key);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
}

json::const_reference json::operator[](StringRef key) const
{
    // const operator[] only works for objects
    if (JSON_LIKELY(is_object()))
    {
        assert(m_value.object->find(key) != m_value.object->end());
        return m_value.object->find(key)->second;
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
}

json::size_type json::erase(StringRef key)
{
    // this erase only works for objects
    if (JSON_LIKELY(is_object()))
    {
        return m_value.object->erase(key);
    }

    JSON_THROW(type_error::create(307, "cannot use erase() with " + Twine(type_name())));
}

void json::erase(const size_type idx)
{
    // this erase only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        if (JSON_UNLIKELY(idx >= size()))
        {
            JSON_THROW(out_of_range::create(401, "array index " + Twine(idx) + " is out of range"));
        }

        m_value.array->erase(m_value.array->begin() + static_cast<difference_type>(idx));
    }
    else
    {
        JSON_THROW(type_error::create(307, "cannot use erase() with " + Twine(type_name())));
    }
}

json::iterator json::find(StringRef key)
{
    auto result = end();

    if (is_object())
    {
        result.m_it.object_iterator = m_value.object->find(key);
    }

    return result;
}

json::const_iterator json::find(StringRef key) const
{
    auto result = cend();

    if (is_object())
    {
        result.m_it.object_iterator = m_value.object->find(key);
    }

    return result;
}

json::size_type json::count(StringRef key) const
{
    // return 0 for all nonobject types
    return is_object() ? m_value.object->count(key) : 0;
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
            break;
    }
}

void json::push_back(json&& val)
{
    // push_back only works for null objects or arrays
    if (JSON_UNLIKELY(not(is_null() or is_array())))
    {
        JSON_THROW(type_error::create(308, "cannot use push_back() with " + Twine(type_name())));
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
    if (JSON_UNLIKELY(not(is_null() or is_array())))
    {
        JSON_THROW(type_error::create(308, "cannot use push_back() with " + Twine(type_name())));
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

void json::push_back(initializer_list_t init)
{
    if (is_object() and init.size() == 2 and (*init.begin())->is_string())
    {
        std::string key = init.begin()->moved_or_copied();
        push_back(std::pair<StringRef, json>(key, (init.begin() + 1)->moved_or_copied()));
    }
    else
    {
        push_back(json(init));
    }
}

json::iterator json::insert(const_iterator pos, const json& val)
{
    // insert only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        // check if iterator pos fits to this JSON value
        if (JSON_UNLIKELY(pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        iterator result(this);
        result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, val);
        return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + Twine(type_name())));
}

json::iterator json::insert(const_iterator pos, size_type cnt, const json& val)
{
    // insert only works for arrays
    if (JSON_LIKELY(is_array()))
    {
        // check if iterator pos fits to this JSON value
        if (JSON_UNLIKELY(pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        iterator result(this);
        result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, cnt, val);
        return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + Twine(type_name())));
}

json::iterator json::insert(const_iterator pos, const_iterator first, const_iterator last)
{
    // insert only works for arrays
    if (JSON_UNLIKELY(not is_array()))
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + Twine(type_name())));
    }

    // check if iterator pos fits to this JSON value
    if (JSON_UNLIKELY(pos.m_object != this))
    {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // check if range iterators belong to the same JSON object
    if (JSON_UNLIKELY(first.m_object != last.m_object))
    {
        JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    if (JSON_UNLIKELY(first.m_object == this))
    {
        JSON_THROW(invalid_iterator::create(211, "passed iterators may not belong to container"));
    }

    // insert to array and return iterator
    iterator result(this);
    result.m_it.array_iterator = m_value.array->insert(
                                     pos.m_it.array_iterator,
                                     first.m_it.array_iterator,
                                     last.m_it.array_iterator);
    return result;
}

json::iterator json::insert(const_iterator pos, initializer_list_t ilist)
{
    // insert only works for arrays
    if (JSON_UNLIKELY(not is_array()))
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + Twine(type_name())));
    }

    // check if iterator pos fits to this JSON value
    if (JSON_UNLIKELY(pos.m_object != this))
    {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // insert to array and return iterator
    iterator result(this);
    result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, ilist.begin(), ilist.end());
    return result;
}

void json::insert(const_iterator first, const_iterator last)
{
    // insert only works for objects
    if (JSON_UNLIKELY(not is_object()))
    {
        JSON_THROW(type_error::create(309, "cannot use insert() with " + Twine(type_name())));
    }

    // check if range iterators belong to the same JSON object
    if (JSON_UNLIKELY(first.m_object != last.m_object))
    {
        JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    // passed iterators must belong to objects
    if (JSON_UNLIKELY(not first.m_object->is_object()))
    {
        JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
    }

    for (auto it = first.m_it.object_iterator, end = last.m_it.object_iterator; it != end; ++it)
    {
        m_value.object->insert(std::make_pair(it->first(), it->second));
    }
}

void json::update(const_reference j)
{
    // implicitly convert null value to an empty object
    if (is_null())
    {
        m_type = value_t::object;
        m_value.object = create<object_t>();
        assert_invariant();
    }

    if (JSON_UNLIKELY(not is_object()))
    {
        JSON_THROW(type_error::create(312, "cannot use update() with " + Twine(type_name())));
    }
    if (JSON_UNLIKELY(not j.is_object()))
    {
        JSON_THROW(type_error::create(312, "cannot use update() with " + Twine(j.type_name())));
    }

    for (auto it = j.cbegin(); it != j.cend(); ++it)
    {
        m_value.object->operator[](it.key()) = it.value();
    }
}

void json::update(const_iterator first, const_iterator last)
{
    // implicitly convert null value to an empty object
    if (is_null())
    {
        m_type = value_t::object;
        m_value.object = create<object_t>();
        assert_invariant();
    }

    if (JSON_UNLIKELY(not is_object()))
    {
        JSON_THROW(type_error::create(312, "cannot use update() with " + Twine(type_name())));
    }

    // check if range iterators belong to the same JSON object
    if (JSON_UNLIKELY(first.m_object != last.m_object))
    {
        JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    // passed iterators must belong to objects
    if (JSON_UNLIKELY(not first.m_object->is_object()
                      or not last.m_object->is_object()))
    {
        JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
    }

    for (auto it = first; it != last; ++it)
    {
        m_value.object->operator[](it.key()) = it.value();
    }
}

bool operator==(json::const_reference lhs, json::const_reference rhs) noexcept
{
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type)
    {
        switch (lhs_type)
        {
            case json::value_t::array:
                return (*lhs.m_value.array == *rhs.m_value.array);

            case json::value_t::object:
                return (*lhs.m_value.object == *rhs.m_value.object);

            case json::value_t::null:
                return true;

            case json::value_t::string:
                return (*lhs.m_value.string == *rhs.m_value.string);

            case json::value_t::boolean:
                return (lhs.m_value.boolean == rhs.m_value.boolean);

            case json::value_t::number_integer:
                return (lhs.m_value.number_integer == rhs.m_value.number_integer);

            case json::value_t::number_unsigned:
                return (lhs.m_value.number_unsigned == rhs.m_value.number_unsigned);

            case json::value_t::number_float:
                return (lhs.m_value.number_float == rhs.m_value.number_float);

            default:
                return false;
        }
    }
    else if (lhs_type == json::value_t::number_integer and rhs_type == json::value_t::number_float)
    {
        return (static_cast<double>(lhs.m_value.number_integer) == rhs.m_value.number_float);
    }
    else if (lhs_type == json::value_t::number_float and rhs_type == json::value_t::number_integer)
    {
        return (lhs.m_value.number_float == static_cast<double>(rhs.m_value.number_integer));
    }
    else if (lhs_type == json::value_t::number_unsigned and rhs_type == json::value_t::number_float)
    {
        return (static_cast<double>(lhs.m_value.number_unsigned) == rhs.m_value.number_float);
    }
    else if (lhs_type == json::value_t::number_float and rhs_type == json::value_t::number_unsigned)
    {
        return (lhs.m_value.number_float == static_cast<double>(rhs.m_value.number_unsigned));
    }
    else if (lhs_type == json::value_t::number_unsigned and rhs_type == json::value_t::number_integer)
    {
        return (static_cast<int64_t>(lhs.m_value.number_unsigned) == rhs.m_value.number_integer);
    }
    else if (lhs_type == json::value_t::number_integer and rhs_type == json::value_t::number_unsigned)
    {
        return (lhs.m_value.number_integer == static_cast<int64_t>(rhs.m_value.number_unsigned));
    }

    return false;
}

bool operator<(json::const_reference lhs, json::const_reference rhs) noexcept
{
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type)
    {
        switch (lhs_type)
        {
            case json::value_t::array:
                return (*lhs.m_value.array) < (*rhs.m_value.array);

            case json::value_t::object:
                return *lhs.m_value.object < *rhs.m_value.object;

            case json::value_t::null:
                return false;

            case json::value_t::string:
                return *lhs.m_value.string < *rhs.m_value.string;

            case json::value_t::boolean:
                return lhs.m_value.boolean < rhs.m_value.boolean;

            case json::value_t::number_integer:
                return lhs.m_value.number_integer < rhs.m_value.number_integer;

            case json::value_t::number_unsigned:
                return lhs.m_value.number_unsigned < rhs.m_value.number_unsigned;

            case json::value_t::number_float:
                return lhs.m_value.number_float < rhs.m_value.number_float;

            default:
                return false;
        }
    }
    else if (lhs_type == json::value_t::number_integer and rhs_type == json::value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_integer) < rhs.m_value.number_float;
    }
    else if (lhs_type == json::value_t::number_float and rhs_type == json::value_t::number_integer)
    {
        return lhs.m_value.number_float < static_cast<double>(rhs.m_value.number_integer);
    }
    else if (lhs_type == json::value_t::number_unsigned and rhs_type == json::value_t::number_float)
    {
        return static_cast<double>(lhs.m_value.number_unsigned) < rhs.m_value.number_float;
    }
    else if (lhs_type == json::value_t::number_float and rhs_type == json::value_t::number_unsigned)
    {
        return lhs.m_value.number_float < static_cast<double>(rhs.m_value.number_unsigned);
    }
    else if (lhs_type == json::value_t::number_integer and rhs_type == json::value_t::number_unsigned)
    {
        return lhs.m_value.number_integer < static_cast<int64_t>(rhs.m_value.number_unsigned);
    }
    else if (lhs_type == json::value_t::number_unsigned and rhs_type == json::value_t::number_integer)
    {
        return static_cast<int64_t>(lhs.m_value.number_unsigned) < rhs.m_value.number_integer;
    }

    // We only reach this line if we cannot compare values. In that case,
    // we compare types. Note we have to call the operator explicitly,
    // because MSVC has problems otherwise.
    return operator<(lhs_type, rhs_type);
}

const char* json::type_name() const noexcept
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

json json::patch(const json& json_patch) const
{
    // make a working copy to apply the patch to
    json result = *this;

    // the valid JSON Patch operations
    enum class patch_operations {add, remove, replace, move, copy, test, invalid};

    const auto get_op = [](const std::string & op)
    {
        if (op == "add")
        {
            return patch_operations::add;
        }
        if (op == "remove")
        {
            return patch_operations::remove;
        }
        if (op == "replace")
        {
            return patch_operations::replace;
        }
        if (op == "move")
        {
            return patch_operations::move;
        }
        if (op == "copy")
        {
            return patch_operations::copy;
        }
        if (op == "test")
        {
            return patch_operations::test;
        }

        return patch_operations::invalid;
    };

    // wrapper for "add" operation; add value at ptr
    const auto operation_add = [&result](json_pointer & ptr, json val)
    {
        // adding to the root of the target document means replacing it
        if (ptr.is_root())
        {
            result = val;
        }
        else
        {
            // make sure the top element of the pointer exists
            json_pointer top_pointer = ptr.top();
            if (top_pointer != ptr)
            {
                result.at(top_pointer);
            }

            // get reference to parent of JSON pointer ptr
            const auto last_path = ptr.pop_back();
            json& parent = result[ptr];

            switch (parent.m_type)
            {
                case value_t::null:
                case value_t::object:
                {
                    // use operator[] to add value
                    parent[last_path] = val;
                    break;
                }

                case value_t::array:
                {
                    if (last_path == "-")
                    {
                        // special case: append to back
                        parent.push_back(val);
                    }
                    else
                    {
                        const auto idx = json_pointer::array_index(last_path);
                        if (JSON_UNLIKELY(static_cast<size_type>(idx) > parent.size()))
                        {
                            // avoid undefined behavior
                            JSON_THROW(out_of_range::create(401, "array index " + Twine(idx) + " is out of range"));
                        }
                        else
                        {
                            // default case: insert add offset
                            parent.insert(parent.begin() + static_cast<difference_type>(idx), val);
                        }
                    }
                    break;
                }

                default:
                {
                    // if there exists a parent it cannot be primitive
                    assert(false);  // LCOV_EXCL_LINE
                }
            }
        }
    };

    // wrapper for "remove" operation; remove value at ptr
    const auto operation_remove = [&result](json_pointer & ptr)
    {
        // get reference to parent of JSON pointer ptr
        const auto last_path = ptr.pop_back();
        json& parent = result.at(ptr);

        // remove child
        if (parent.is_object())
        {
            // perform range check
            auto it = parent.find(last_path);
            if (JSON_LIKELY(it != parent.end()))
            {
                parent.erase(it);
            }
            else
            {
                JSON_THROW(out_of_range::create(403, "key '" + Twine(last_path) + "' not found"));
            }
        }
        else if (parent.is_array())
        {
            // note erase performs range check
            parent.erase(static_cast<size_type>(json_pointer::array_index(last_path)));
        }
    };

    // type check: top level value must be an array
    if (JSON_UNLIKELY(not json_patch.is_array()))
    {
        JSON_THROW(parse_error::create(104, 0, "JSON patch must be an array of objects"));
    }

    // iterate and apply the operations
    for (const auto& val : json_patch)
    {
        // wrapper to get a value for an operation
        const auto get_value = [&val](const std::string & op,
                                      const std::string & member,
                                      bool string_type) -> json &
        {
            // find value
            auto it = val.m_value.object->find(member);

            // context-sensitive error message
            const auto error_msg = (op == "op") ? "operation" : "operation '" + op + "'";

            // check if desired value is present
            if (JSON_UNLIKELY(it == val.m_value.object->end()))
            {
                JSON_THROW(parse_error::create(105, 0, Twine(error_msg) + " must have member '" + Twine(member) + "'"));
            }

            // check if result is of type string
            if (JSON_UNLIKELY(string_type and not it->second.is_string()))
            {
                JSON_THROW(parse_error::create(105, 0, Twine(error_msg) + " must have string member '" + Twine(member) + "'"));
            }

            // no error: return value
            return it->second;
        };

        // type check: every element of the array must be an object
        if (JSON_UNLIKELY(not val.is_object()))
        {
            JSON_THROW(parse_error::create(104, 0, "JSON patch must be an array of objects"));
        }

        // collect mandatory members
        const std::string op = get_value("op", "op", true);
        const std::string path = get_value(op, "path", true);
        json_pointer ptr(path);

        switch (get_op(op))
        {
            case patch_operations::add:
            {
                operation_add(ptr, get_value("add", "value", false));
                break;
            }

            case patch_operations::remove:
            {
                operation_remove(ptr);
                break;
            }

            case patch_operations::replace:
            {
                // the "path" location must exist - use at()
                result.at(ptr) = get_value("replace", "value", false);
                break;
            }

            case patch_operations::move:
            {
                const std::string from_path = get_value("move", "from", true);
                json_pointer from_ptr(from_path);

                // the "from" location must exist - use at()
                json v = result.at(from_ptr);

                // The move operation is functionally identical to a
                // "remove" operation on the "from" location, followed
                // immediately by an "add" operation at the target
                // location with the value that was just removed.
                operation_remove(from_ptr);
                operation_add(ptr, v);
                break;
            }

            case patch_operations::copy:
            {
                const std::string from_path = get_value("copy", "from", true);
                const json_pointer from_ptr(from_path);

                // the "from" location must exist - use at()
                json v = result.at(from_ptr);

                // The copy is functionally identical to an "add"
                // operation at the target location using the value
                // specified in the "from" member.
                operation_add(ptr, v);
                break;
            }

            case patch_operations::test:
            {
                bool success = false;
                JSON_TRY
                {
                    // check if "value" matches the one at "path"
                    // the "path" location must exist - use at()
                    success = (result.at(ptr) == get_value("test", "value", false));
                }
                JSON_CATCH (out_of_range&)
                {
                    // ignore out of range errors: success remains false
                }

                // throw an exception if test fails
                if (JSON_UNLIKELY(not success))
                {
                    JSON_THROW(other_error::create(501, "unsuccessful: " + Twine(val.dump())));
                }

                break;
            }

            case patch_operations::invalid:
            {
                // op must be "add", "remove", "replace", "move", "copy", or
                // "test"
                JSON_THROW(parse_error::create(105, 0, "operation value '" + Twine(op) + "' is invalid"));
            }
        }
    }

    return result;
}

json json::diff(const json& source, const json& target,
                       const std::string& path)
{
    // the patch
    json result(value_t::array);

    // if the values are the same, return empty patch
    if (source == target)
    {
        return result;
    }

    if (source.type() != target.type())
    {
        // different types: replace value
        result.push_back(
        {
            {"op", "replace"}, {"path", path}, {"value", target}
        });
    }
    else
    {
        switch (source.type())
        {
            case value_t::array:
            {
                // first pass: traverse common elements
                std::size_t i = 0;
                while (i < source.size() and i < target.size())
                {
                    // recursive call to compare array values at index i
                    auto temp_diff = diff(source[i], target[i], path + "/" + std::to_string(i));
                    result.insert(result.end(), temp_diff.begin(), temp_diff.end());
                    ++i;
                }

                // i now reached the end of at least one array
                // in a second pass, traverse the remaining elements

                // remove my remaining elements
                const auto end_index = static_cast<difference_type>(result.size());
                while (i < source.size())
                {
                    // add operations in reverse order to avoid invalid
                    // indices
                    result.insert(result.begin() + end_index, object(
                    {
                        {"op", "remove"},
                        {"path", path + "/" + std::to_string(i)}
                    }));
                    ++i;
                }

                // add other remaining elements
                while (i < target.size())
                {
                    result.push_back(
                    {
                        {"op", "add"},
                        {"path", path + "/" + std::to_string(i)},
                        {"value", target[i]}
                    });
                    ++i;
                }

                break;
            }

            case value_t::object:
            {
                // first pass: traverse this object's elements
                for (auto it = source.cbegin(); it != source.cend(); ++it)
                {
                    // escape the key name to be used in a JSON patch
                    const auto key = json_pointer::escape(it.key());

                    if (target.find(it.key()) != target.end())
                    {
                        // recursive call to compare object values at key it
                        auto temp_diff = diff(it.value(), target[it.key()], path + "/" + key);
                        result.insert(result.end(), temp_diff.begin(), temp_diff.end());
                    }
                    else
                    {
                        // found a key that is not in o -> remove it
                        result.push_back(object(
                        {
                            {"op", "remove"}, {"path", path + "/" + key}
                        }));
                    }
                }

                // second pass: traverse other object's elements
                for (auto it = target.cbegin(); it != target.cend(); ++it)
                {
                    if (source.find(it.key()) == source.end())
                    {
                        // found a key that is not in this -> add it
                        const auto key = json_pointer::escape(it.key());
                        result.push_back(
                        {
                            {"op", "add"}, {"path", path + "/" + key},
                            {"value", it.value()}
                        });
                    }
                }

                break;
            }

            default:
            {
                // both primitive type: replace value
                result.push_back(
                {
                    {"op", "replace"}, {"path", path}, {"value", target}
                });
                break;
            }
        }
    }

    return result;
}

void json::merge_patch(const json& patch)
{
    if (patch.is_object())
    {
        if (not is_object())
        {
            *this = object();
        }
        for (auto it = patch.begin(); it != patch.end(); ++it)
        {
            if (it.value().is_null())
            {
                erase(it.key());
            }
            else
            {
                operator[](it.key()).merge_patch(it.value());
            }
        }
    }
    else
    {
        *this = patch;
    }
}

}  // namespace wpi
