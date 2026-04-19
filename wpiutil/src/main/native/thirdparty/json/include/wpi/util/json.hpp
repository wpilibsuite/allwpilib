// -*- mode:c++;indent-tabs-mode:nil;c-basic-offset:4;coding:utf-8 -*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
//
// Copyright 2024 Mozilla Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <climits>
#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/util/StringMap.hpp"
#include "wpi/util/expected"

namespace wpi::util {

class json;
class raw_ostream;

template <typename T>
struct json_serializer;

namespace detail {
template <typename T>
concept HasToJson = requires(json& j, const T& val) {
    { to_json(j, val) };
};

template <typename T>
concept HasFromJson = requires(const json& j, T& val) {
    { from_json(j, val) };
};

template <typename T>
concept HasJsonSerializer = requires (json& j, const json& cj, const T& val) {
    typename json_serializer<typename std::remove_cvref_t<T>>;
    { json_serializer<typename std::remove_cvref_t<T>>::to(j, val) };
};

template <typename T>
concept HasJsonDeserializer = requires (json& j, const json& cj, const T& val) {
    typename json_serializer<typename std::remove_cvref_t<T>>;
    { json_serializer<typename std::remove_cvref_t<T>>::from(cj) } -> std::convertible_to<typename std::remove_cvref_t<T>>;
};

template <typename T>
concept JsonConvertible =
    HasToJson<T> ||
    HasJsonSerializer<T> ||
    std::is_same_v<T, std::nullptr_t> ||
    std::is_same_v<T, bool> ||
    std::is_floating_point_v<T> ||
    std::is_integral_v<T> ||
    std::convertible_to<T, std::string> ||
    std::convertible_to<T, std::string_view> ||
    std::is_same_v<T, std::vector<json>> ||
    std::is_same_v<T, wpi::util::StringMap<json>>;

template <typename F, typename Tuple, size_t... I>
void apply_pairs_helper(F&& f, Tuple&& t, std::index_sequence<I...>) {
    // This fold expression creates a sequence of calls to f for each pair
    (f(std::get<I * 2>(std::forward<Tuple>(t)),
       std::get<I * 2 + 1>(std::forward<Tuple>(t))), ...);
}
}  // namespace detail

class json
{
    friend bool operator==(const json& lhs, const json& rhs);
  public:
    using array_t = std::vector<json>;
    using object_t = wpi::util::StringMap<json>;

    enum class Type
    {
        Null,
        Bool,
        Int,
        Uint,
        Float,
        Double,
        String,
        Array,
        Object
    };

  private:
    enum Status
    {
        success,
        bad_double,
        absent_value,
        bad_negative,
        bad_exponent,
        missing_comma,
        missing_colon,
        malformed_utf8,
        depth_exceeded,
        stack_overflow,
        unexpected_eof,
        overlong_ascii,
        unexpected_comma,
        unexpected_colon,
        unexpected_octal,
        trailing_content,
        illegal_character,
        invalid_hex_escape,
        overlong_utf8_0x7ff,
        overlong_utf8_0xffff,
        object_missing_value,
        illegal_utf8_character,
        invalid_unicode_escape,
        utf16_surrogate_in_utf8,
        unexpected_end_of_array,
        hex_escape_not_printable,
        invalid_escape_character,
        utf8_exceeds_utf16_range,
        unexpected_end_of_string,
        unexpected_end_of_object,
        object_key_must_be_string,
        c1_control_code_in_string,
        non_del_c0_control_code_in_string,
    };

    Type type_;
    union
    {
        bool bool_value;
        float float_value;
        double double_value;
        long long long_value;
        unsigned long long ulong_value;
        std::string string_value;
        array_t array_value;
        object_t object_value;
    };

  public:
    static wpi::util::expected<json, const char*> parse(std::string_view);
    static json parse_or_throw(std::string_view);

    json(const json&);
    json(json&&);
    json(const char*);
    json(const std::string&);
    json(std::string_view);
    ~json();

    constexpr json(const std::nullptr_t = nullptr) : type_(Type::Null)
    {
    }

    constexpr json(std::same_as<bool> auto value) : type_(Type::Bool), bool_value(value)
    {
    }

    constexpr json(int value) : type_(Type::Int), long_value(value)
    {
    }

    constexpr json(float value) : type_(Type::Float), float_value(value)
    {
    }

    constexpr json(unsigned value) : type_(Type::Int), long_value(value)
    {
    }

    constexpr json(long value) : type_(Type::Int), long_value(value)
    {
    }

    constexpr json(unsigned long value)
    {
        if (value <= LLONG_MAX) {
            type_ = Type::Int;
            long_value = value;
        } else {
            type_ = Type::Uint;
            ulong_value = value;
        }
    }

    constexpr json(long long value) : type_(Type::Int), long_value(value)
    {
    }

    constexpr json(unsigned long long value)
    {
        if (value <= LLONG_MAX) {
            type_ = Type::Int;
            long_value = value;
        } else {
            type_ = Type::Uint;
            ulong_value = value;
        }
    }

    constexpr json(double value) : type_(Type::Double), double_value(value)
    {
    }

    json(std::string&& value) : type_(Type::String), string_value(std::move(value))
    {
    }

    json(array_t&& value) : type_(Type::Array), array_value(std::move(value))
    {
    }

    json(object_t&& value) : type_(Type::Object), object_value(std::move(value))
    {
    }

    template <std::ranges::input_range R>
      requires detail::JsonConvertible<std::ranges::range_value_t<R>>
    json(const R& range) : type_(Type::Array), array_value(std::ranges::cbegin(range), std::ranges::cend(range))
    {
    }

    template <detail::HasToJson T>
    json(const T& value) : type_(Type::Null)
    {
        to_json(*this, value);
    }

    template <detail::HasJsonSerializer T>
    json(const T& value) : type_(Type::Null)
    {
        json_serializer<T>::to(*this, value);
    }

    constexpr Type type() const
    {
        return type_;
    }

    constexpr bool is_null() const
    {
        return type_ == Type::Null;
    }

    constexpr bool is_bool() const
    {
        return type_ == Type::Bool;
    }

    constexpr bool is_number() const
    {
        return is_float() || is_double() || is_int() || is_uint();
    }

    constexpr bool is_int() const
    {
        return type_ == Type::Int;
    }

    constexpr bool is_uint() const
    {
        return type_ == Type::Uint;
    }

    constexpr bool is_float() const
    {
        return type_ == Type::Float;
    }

    constexpr bool is_double() const
    {
        return type_ == Type::Double;
    }

    bool is_string() const
    {
        return type_ == Type::String;
    }

    bool is_array() const
    {
        return type_ == Type::Array;
    }

    bool is_object() const
    {
        return type_ == Type::Object;
    }

    bool get_bool() const;
    float get_float() const;
    double get_double() const;
    double get_number() const;
    long long get_int() const;
    unsigned long long get_uint() const;
    std::string& get_string();
    const std::string& get_string() const;
    array_t& get_array();
    const array_t& get_array() const;
    object_t& get_object();
    const object_t& get_object() const;

    template <detail::HasFromJson T>
    T get() const {
        T value;
        from_json(*this, value);
        return value;
    }

    template <detail::HasJsonDeserializer T>
    T get() const {
        return json_serializer<T>::from(*this);
    }

    bool contains(std::string_view) const;

    json* lookup(std::string_view);
    const json* lookup(std::string_view) const;

    void set_array();
    void set_object();

    template <typename... Args>
    static json array(Args&&... args) {
        json j;
        j.set_array();
        (j.array_value.emplace_back(std::forward<Args>(args)), ...);
        return j;
    }

    template <typename... Args>
    static json object(Args&&... args) {
        json j;
        j.set_object();
        detail::apply_pairs_helper(
            [&](auto&& key, auto&& value) {
                j.object_value[std::forward<decltype(key)>(key)] = std::forward<decltype(value)>(value);
            },
            std::forward_as_tuple(args...),
            std::make_index_sequence<sizeof...(Args) / 2>{});
        return j;
    }

    std::string to_string() const;
    std::string to_string_pretty() const;

    json& operator=(const json&);
    json& operator=(json&&);

    template <detail::HasToJson T>
    json& operator=(const T& value) {
        to_json(*this, value);
        return *this;
    }

    template <detail::HasJsonSerializer T>
    json& operator=(const T& value) {
        json_serializer<T>::to(*this, value);
        return *this;
    }

    json& operator[](size_t);
    json& operator[](std::string_view);

    json& at(size_t);
    json& at(std::string_view);

    const json& at(size_t) const;
    const json& at(std::string_view) const;

    json value(size_t, json&&);
    json value(std::string_view, json&&);

    void erase(std::string_view);

    json& emplace_back(json&& value);

    bool empty() const;

    operator std::string() const
    {
        return to_string();
    }

    void marshal(wpi::util::raw_ostream& os, bool pretty = false, int indent = 0) const;

    static void stringify_null(wpi::util::raw_ostream&);
    static void stringify_string(wpi::util::raw_ostream&, std::string_view);
    static void stringify_bool(wpi::util::raw_ostream&, bool);
    static void stringify_float(wpi::util::raw_ostream&, float);
    static void stringify_double(wpi::util::raw_ostream&, double);
    static void stringify_int(wpi::util::raw_ostream&, long long);
    static void stringify_uint(wpi::util::raw_ostream&, unsigned long long);
    static void stringify_array(wpi::util::raw_ostream&, const array_t&, bool pretty = false, int indent = 0);
    static void stringify_object(wpi::util::raw_ostream&, const object_t&, bool pretty = false, int indent = 0);

  private:
    static const char* StatusToString(Status);
    void clear();
    static void serialize(wpi::util::raw_ostream&, std::string_view);
    static Status parse(json&, const char*&, const char*, int, int);
};

bool operator==(const json& lhs, const json& rhs);
inline bool operator!=(const json& lhs, const json& rhs) {
    return !(lhs == rhs);
}

} // namespace wpi::util
