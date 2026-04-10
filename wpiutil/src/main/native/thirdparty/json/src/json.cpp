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

#include "wpi/util/json.hpp"
#include "jtckdint.h"

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "wpi/double-conversion/double-to-string.h"
#include "wpi/double-conversion/string-to-double.h"
#include "wpi/util/raw_ostream.hpp"

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#define KEY 1
#define COMMA 2
#define COLON 4
#define ARRAY 8
#define OBJECT 16
#define DEPTH 20

#define ASCII 0
#define C0 1
#define DQUOTE 2
#define BACKSLASH 3
#define UTF8_2 4
#define UTF8_3 5
#define UTF8_4 6
#define C1 7
#define UTF8_3_E0 8
#define UTF8_3_ED 9
#define UTF8_4_F0 10
#define BADUTF8 11
#define EVILUTF8 12

#define UTF16_MASK 0xfc00
#define UTF16_MOAR 0xd800 // 0xD800..0xDBFF
#define UTF16_CONT 0xdc00 // 0xDC00..0xDFFF

#define READ32LE(S) \
    ((uint_least32_t)(255 & (S)[3]) << 030 | \
     (uint_least32_t)(255 & (S)[2]) << 020 | \
     (uint_least32_t)(255 & (S)[1]) << 010 | \
     (uint_least32_t)(255 & (S)[0]) << 000)

#define ThomPikeCont(x) (0200 == (0300 & (x)))
#define ThomPikeByte(x) ((x) & (((1 << ThomPikeMsb(x)) - 1) | 3))
#define ThomPikeLen(x) (7 - ThomPikeMsb(x))
#define ThomPikeMsb(x) ((255 & (x)) < 252 ? Bsr(255 & ~(x)) : 1)
#define ThomPikeMerge(x, y) ((x) << 6 | (077 & (y)))

#define IsSurrogate(wc) ((0xf800 & (wc)) == 0xd800)
#define IsHighSurrogate(wc) (((wc) & UTF16_MASK) == UTF16_MOAR)
#define IsLowSurrogate(wc) (((wc) & UTF16_MASK) == UTF16_CONT)
#define MergeUtf16(hi, lo) ((((hi) - 0xD800) << 10) + ((lo) - 0xDC00) + 0x10000)
#define EncodeUtf16(wc) \
    ((0x0000 <= (wc) && (wc) <= 0xFFFF) || (0xE000 <= (wc) && (wc) <= 0xFFFF) \
       ? (wc) \
     : 0x10000 <= (wc) && (wc) <= 0x10FFFF \
       ? (((((wc) - 0x10000) >> 10) + 0xD800) | \
          (unsigned)((((wc) - 0x10000) & 1023) + 0xDC00) << 16) \
       : 0xFFFD)

#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#define ON_LOGIC_ERROR(s) throw std::logic_error(s)
#else
#define ON_LOGIC_ERROR(s) abort()
#endif

namespace wpi::util {

static const char kJsonStr[256] = {
    1,  1,  1,  1,  1,  1,  1,  1, // 0000 ascii (0)
    1,  1,  1,  1,  1,  1,  1,  1, // 0010
    1,  1,  1,  1,  1,  1,  1,  1, // 0020 c0 (1)
    1,  1,  1,  1,  1,  1,  1,  1, // 0030
    0,  0,  2,  0,  0,  0,  0,  0, // 0040 dquote (2)
    0,  0,  0,  0,  0,  0,  0,  0, // 0050
    0,  0,  0,  0,  0,  0,  0,  0, // 0060
    0,  0,  0,  0,  0,  0,  0,  0, // 0070
    0,  0,  0,  0,  0,  0,  0,  0, // 0100
    0,  0,  0,  0,  0,  0,  0,  0, // 0110
    0,  0,  0,  0,  0,  0,  0,  0, // 0120
    0,  0,  0,  0,  3,  0,  0,  0, // 0130 backslash (3)
    0,  0,  0,  0,  0,  0,  0,  0, // 0140
    0,  0,  0,  0,  0,  0,  0,  0, // 0150
    0,  0,  0,  0,  0,  0,  0,  0, // 0160
    0,  0,  0,  0,  0,  0,  0,  0, // 0170
    7,  7,  7,  7,  7,  7,  7,  7, // 0200 c1 (8)
    7,  7,  7,  7,  7,  7,  7,  7, // 0210
    7,  7,  7,  7,  7,  7,  7,  7, // 0220
    7,  7,  7,  7,  7,  7,  7,  7, // 0230
    11, 11, 11, 11, 11, 11, 11, 11, // 0240 latin1 (4)
    11, 11, 11, 11, 11, 11, 11, 11, // 0250
    11, 11, 11, 11, 11, 11, 11, 11, // 0260
    11, 11, 11, 11, 11, 11, 11, 11, // 0270
    12, 12, 4,  4,  4,  4,  4,  4, // 0300 utf8-2 (5)
    4,  4,  4,  4,  4,  4,  4,  4, // 0310
    4,  4,  4,  4,  4,  4,  4,  4, // 0320 utf8-2
    4,  4,  4,  4,  4,  4,  4,  4, // 0330
    8,  5,  5,  5,  5,  5,  5,  5, // 0340 utf8-3 (6)
    5,  5,  5,  5,  5,  9,  5,  5, // 0350
    10, 6,  6,  6,  6,  11, 11, 11, // 0360 utf8-4 (7)
    11, 11, 11, 11, 11, 11, 11, 11, // 0370
};

static const char kEscapeLiteral[128] = {
    9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 2, 9, 4, 3, 9, 9, // 0x00
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 0x10
    0, 0, 7, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 6, // 0x20
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0, // 0x30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, // 0x50
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x60
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, // 0x70
};

alignas(signed char) static const signed char kHexToInt[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x00
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x10
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x20
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, // 0x30
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x40
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x50
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x60
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x90
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xa0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xb0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xc0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xd0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xe0
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0xf0
};

static const double_conversion::DoubleToStringConverter kDoubleToJson(
  double_conversion::DoubleToStringConverter::UNIQUE_ZERO |
    double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN,
  "1e5000",
  "null",
  'e',
  -6,
  21,
  6,
  0);

static const double_conversion::StringToDoubleConverter kJsonToDouble(
  double_conversion::StringToDoubleConverter::ALLOW_CASE_INSENSITIVITY |
    double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
    double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK |
    double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES,
  0.0,
  1.0,
  "Infinity",
  "NaN");

#if defined(__GNUC__) || defined(__clang__)
#define Bsr(x) (__builtin_clz(x) ^ (sizeof(int) * CHAR_BIT - 1))
#else
static int
Bsr(int x)
{
    int r = 0;
    if (x & 0xFFFF0000u) {
        x >>= 16;
        r |= 16;
    }
    if (x & 0xFF00) {
        x >>= 8;
        r |= 8;
    }
    if (x & 0xF0) {
        x >>= 4;
        r |= 4;
    }
    if (x & 0xC) {
        x >>= 2;
        r |= 2;
    }
    if (x & 0x2) {
        r |= 1;
    }
    return r;
}
#endif

static double
StringToDouble(const char* s, size_t n, int* out_processed)
{
    if (n == (size_t)-1)
        n = strlen(s);
    int processed;
    double res = kJsonToDouble.StringToDouble(s, n, &processed);
    if (out_processed)
        *out_processed = processed;
    return res;
}

static char*
UlongToString(char* p, unsigned long long x)
{
    char t;
    size_t i, a, b;
    i = 0;
    do {
        p[i++] = x % 10 + '0';
        x = x / 10;
    } while (x > 0);
    p[i] = '\0';
    if (i) {
        for (a = 0, b = i - 1; a < b; ++a, --b) {
            t = p[a];
            p[a] = p[b];
            p[b] = t;
        }
    }
    return p + i;
}

static char*
LongToString(char* p, long long x)
{
    if (x < 0)
        *p++ = '-', x = 0 - (unsigned long long)x;
    return UlongToString(p, x);
}

json::json(const char* value)
{
    if (value) {
        type_ = Type::String;
        new (&string_value) std::string(value);
    } else {
        type_ = Type::Null;
    }
}

json::json(const std::string& value) : type_(Type::String), string_value(value)
{
}

json::json(std::string_view value) : type_(Type::String), string_value(value)
{
}

json::~json()
{
    if (type_ >= Type::String)
        clear();
}

void
json::clear()
{
    switch (type_) {
        case Type::String:
            string_value.~basic_string();
            break;
        case Type::Array:
            array_value.~array_t();
            break;
        case Type::Object:
            object_value.~object_t();
            break;
        default:
            break;
    }
    type_ = Type::Null;
}

json::json(const json& other) : type_(other.type_)
{
    switch (type_) {
        case Type::Null:
            break;
        case Type::Bool:
            bool_value = other.bool_value;
            break;
        case Type::Int:
            long_value = other.long_value;
            break;
        case Type::Uint:
            ulong_value = other.ulong_value;
            break;
        case Type::Float:
            float_value = other.float_value;
            break;
        case Type::Double:
            double_value = other.double_value;
            break;
        case Type::String:
            new (&string_value) std::string(other.string_value);
            break;
        case Type::Array:
            new (&array_value) array_t(other.array_value);
            break;
        case Type::Object:
            new (&object_value) object_t(other.object_value);
            break;
        default:
            ON_LOGIC_ERROR("Unhandled JSON type.");
    }
}

json&
json::operator=(const json& other)
{
    if (this != &other) {
        if (type_ >= Type::String)
            clear();
        type_ = other.type_;
        switch (type_) {
            case Type::Null:
                break;
            case Type::Bool:
                bool_value = other.bool_value;
                break;
            case Type::Int:
                long_value = other.long_value;
                break;
            case Type::Uint:
                ulong_value = other.ulong_value;
                break;
            case Type::Float:
                float_value = other.float_value;
                break;
            case Type::Double:
                double_value = other.double_value;
                break;
            case Type::String:
                new (&string_value) std::string(other.string_value);
                break;
            case Type::Array:
                new (&array_value) array_t(other.array_value);
                break;
            case Type::Object:
                new (&object_value) object_t(other.object_value);
                break;
            default:
                ON_LOGIC_ERROR("Unhandled JSON type.");
        }
    }
    return *this;
}

json::json(json&& other) : type_(other.type_)
{
    switch (type_) {
        case Type::Null:
            break;
        case Type::Bool:
            bool_value = other.bool_value;
            break;
        case Type::Int:
            long_value = other.long_value;
            break;
        case Type::Uint:
            ulong_value = other.ulong_value;
            break;
        case Type::Float:
            float_value = other.float_value;
            break;
        case Type::Double:
            double_value = other.double_value;
            break;
        case Type::String:
            new (&string_value) std::string(std::move(other.string_value));
            break;
        case Type::Array:
            new (&array_value) array_t(std::move(other.array_value));
            break;
        case Type::Object:
            new (&object_value) object_t(std::move(other.object_value));
            break;
        default:
            ON_LOGIC_ERROR("Unhandled JSON type.");
    }
    other.type_ = Type::Null;
}

json&
json::operator=(json&& other)
{
    if (this != &other) {
        if (type_ >= Type::String)
            clear();
        type_ = other.type_;
        switch (type_) {
            case Type::Null:
                break;
            case Type::Bool:
                bool_value = other.bool_value;
                break;
            case Type::Int:
                long_value = other.long_value;
                break;
            case Type::Uint:
                ulong_value = other.ulong_value;
                break;
            case Type::Float:
                float_value = other.float_value;
                break;
            case Type::Double:
                double_value = other.double_value;
                break;
            case Type::String:
                new (&string_value) std::string(std::move(other.string_value));
                break;
            case Type::Array:
                new (&array_value) array_t(std::move(other.array_value));
                break;
            case Type::Object:
                new (&object_value) object_t(std::move(other.object_value));
                break;
            default:
                ON_LOGIC_ERROR("Unhandled JSON type.");;
        }
        other.type_ = Type::Null;
    }
    return *this;
}

double
json::get_number() const
{
    switch (type_) {
        case Type::Int:
            return long_value;
        case Type::Uint:
            return ulong_value;
        case Type::Float:
            return float_value;
        case Type::Double:
            return double_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a number.");
    }
}

long long
json::get_int() const
{
    switch (type_) {
        case Type::Int:
            return long_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a long.");
    }
}

unsigned long long
json::get_uint() const
{
    switch (type_) {
        case Type::Uint:
            return ulong_value;
        default:
            ON_LOGIC_ERROR("JSON value is not an unsigned long.");
    }
}

bool
json::get_bool() const
{
    switch (type_) {
        case Type::Bool:
            return bool_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a bool.");
    }
}

float
json::get_float() const
{
    switch (type_) {
        case Type::Float:
            return float_value;
        case Type::Double:
            return double_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a floating-point number.");
    }
}

double
json::get_double() const
{
    switch (type_) {
        case Type::Float:
            return float_value;
        case Type::Double:
            return double_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a floating-point number.");
    }
}

std::string&
json::get_string()
{
    switch (type_) {
        case Type::String:
            return string_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a string.");
    }
}

const std::string&
json::get_string() const
{
    switch (type_) {
        case Type::String:
            return string_value;
        default:
            ON_LOGIC_ERROR("JSON value is not a string.");
    }
}

json::array_t&
json::get_array()
{
    switch (type_) {
        case Type::Array:
            return array_value;
        default:
            ON_LOGIC_ERROR("JSON value is not an array.");
    }
}

const json::array_t&
json::get_array() const
{
    switch (type_) {
        case Type::Array:
            return array_value;
        default:
            ON_LOGIC_ERROR("JSON value is not an array.");
    }
}

json::object_t&
json::get_object()
{
    switch (type_) {
        case Type::Object:
            return object_value;
        default:
            ON_LOGIC_ERROR("JSON value is not an object.");
    }
}

const json::object_t&
json::get_object() const
{
    switch (type_) {
        case Type::Object:
            return object_value;
        default:
            ON_LOGIC_ERROR("JSON value is not an object.");
    }
}

void
json::set_array()
{
    if (type_ >= Type::String)
        clear();
    type_ = Type::Array;
    new (&array_value) array_t();
}

void
json::set_object()
{
    if (type_ >= Type::String)
        clear();
    type_ = Type::Object;
    new (&object_value) object_t();
}

bool
json::contains(std::string_view key) const
{
    if (!is_object())
        return false;
    return object_value.find(key) != object_value.end();
}

json*
json::lookup(std::string_view key)
{
    if (!is_object())
        return nullptr;
    auto i = object_value.find(key);
    if (i == object_value.end())
        return nullptr;
    return &i->second;
}

const json*
json::lookup(std::string_view key) const
{
    if (!is_object())
        return nullptr;
    auto i = object_value.find(key);
    if (i == object_value.end())
        return nullptr;
    return &i->second;
}

json&
json::operator[](size_t index)
{
    if (!is_array())
        set_array();
    if (index >= array_value.size()) {
        array_value.resize(index + 1);
    }
    return array_value[index];
}

json&
json::operator[](std::string_view key)
{
    if (!is_object())
        set_object();
    return object_value[key];
}

json&
json::at(size_t index) {
    if (!is_array())
        ON_LOGIC_ERROR("JSON value is not an array.");
    return array_value.at(index);
}

json&
json::at(std::string_view key) {
    if (!is_object())
        ON_LOGIC_ERROR("JSON value is not an object.");
    return object_value.at(key);
}

const json&
json::at(size_t index) const {
    if (!is_array())
        ON_LOGIC_ERROR("JSON value is not an array.");
    return array_value.at(index);
}

const json&
json::at(std::string_view key) const {
    if (!is_object())
        ON_LOGIC_ERROR("JSON value is not an object.");
    return object_value.at(key);
}

json
json::value(size_t index, json&& default_value) {
    if (!is_array())
        ON_LOGIC_ERROR("JSON value is not an array.");
    if (index < array_value.size())
        return array_value[index];
    return default_value;
}

json
json::value(std::string_view key, json&& default_value)
{
    if (!is_object())
        ON_LOGIC_ERROR("JSON value is not an object.");
    auto i = object_value.find(key);
    if (i != object_value.end())
        return i->second;
    return default_value;
}

void
json::erase(std::string_view key)
{
    if (!is_object())
        ON_LOGIC_ERROR("JSON value is not an object.");
    object_value.erase(key);
}

json&
json::emplace_back(json&& value)
{
    if (!is_array())
        set_array();
    array_value.emplace_back(std::move(value));
    return array_value.back();
}

bool
json::empty() const
{
    switch (type_) {
        case Type::Null:
            return true;
        case Type::Array:
            return array_value.empty();
        case Type::Object:
            return object_value.empty();
        default:
            return false;
    }
}

std::string
json::to_string() const
{
    std::string b;
    wpi::util::raw_string_ostream os(b);
    marshal(os, false, 0);
    return b;
}

std::string
json::to_string_pretty() const
{
    std::string b;
    wpi::util::raw_string_ostream os(b);
    marshal(os, true, 0);
    return b;
}

void
json::marshal(wpi::util::raw_ostream& os, bool pretty, int indent) const
{
    switch (type_) {
        case Type::Null:
            stringify_null(os);
            break;
        case Type::String:
            stringify_string(os, string_value);
            break;
        case Type::Bool:
            stringify_bool(os, bool_value);
            break;
        case Type::Int:
            stringify_int(os, long_value);
            break;
        case Type::Uint:
            stringify_uint(os, ulong_value);
            break;
        case Type::Float:
            stringify_float(os, float_value);
            break;
        case Type::Double:
            stringify_double(os, double_value);
            break;
        case Type::Array:
            stringify_array(os, array_value, pretty, indent);
            break;
        case Type::Object:
            stringify_object(os, object_value, pretty, indent);
            break;
        default:
            ON_LOGIC_ERROR("Unhandled JSON type.");
    }
}

void
json::stringify_null(wpi::util::raw_ostream& os)
{
    os << "null";
}

void
json::stringify_string(wpi::util::raw_ostream& os, std::string_view s)
{
    os << '"';
    serialize(os, s);
    os << '"';
}

void
json::stringify_bool(wpi::util::raw_ostream& os, bool value) {
    os << (value ? "true" : "false");
}

void
json::stringify_float(wpi::util::raw_ostream& os, float value) {
    char buf[128];
    double_conversion::StringBuilder db(buf, 128);
    kDoubleToJson.ToShortestSingle(value, &db);
    db.Finalize();
    os << buf;
}

void
json::stringify_double(wpi::util::raw_ostream& os, double value) {
    char buf[128];
    double_conversion::StringBuilder db(buf, 128);
    kDoubleToJson.ToShortest(value, &db);
    db.Finalize();
    os << buf;
}

void
json::stringify_int(wpi::util::raw_ostream& os, long long value) {
    char buf[64];
    os.write(buf, LongToString(buf, value) - buf);
}

void
json::stringify_uint(wpi::util::raw_ostream& os, unsigned long long value) {
    char buf[64];
    os.write(buf, UlongToString(buf, value) - buf);
}

void
json::stringify_array(wpi::util::raw_ostream& os, const array_t& value, bool pretty, int indent) {
    bool once = false;
    os << '[';
    for (auto i = value.begin(); i != value.end(); ++i) {
        if (once) {
            os << ',';
            if (pretty)
                os << ' ';
        } else {
            once = true;
        }
        i->marshal(os, pretty, indent);
    }
    os << ']';
}

void
json::stringify_object(wpi::util::raw_ostream& os, const object_t& value, bool pretty, int indent) {
    bool once = false;
    os << '{';
    for (auto i = value.begin(); i != value.end(); ++i) {
        if (once) {
            os << ',';
        } else {
            once = true;
        }
        if (pretty && value.size() > 1) {
            os << '\n';
            ++indent;
            for (int j = 0; j < indent; ++j)
                os << "  ";
        }
        stringify_string(os, i->first);
        os << ':';
        if (pretty)
            os << ' ';
        i->second.marshal(os, pretty, indent);
        if (pretty && value.size() > 1)
            --indent;
    }
    if (pretty && value.size() > 1) {
        os << '\n';
        for (int j = 0; j < indent; ++j)
            os << "  ";
        ++indent;
    }
    os << '}';
}

void
json::serialize(wpi::util::raw_ostream& os, std::string_view s)
{
    size_t i, j, m;
    wint_t x, a, b;
    unsigned long long w;
    for (i = 0; i < s.size();) {
        x = s[i++] & 255;
        if (x >= 0300) {
            a = ThomPikeByte(x);
            m = ThomPikeLen(x) - 1;
            if (i + m <= s.size()) {
                for (j = 0;;) {
                    b = s[i + j] & 0xff;
                    if (!ThomPikeCont(b))
                        break;
                    a = ThomPikeMerge(a, b);
                    if (++j == m) {
                        x = a;
                        i += j;
                        break;
                    }
                }
            }
        }
        switch (0 <= x && x <= 127 ? kEscapeLiteral[x] : 9) {
            case 0:
                os << static_cast<char>(x);
                break;
            case 1:
                os << "\\t";
                break;
            case 2:
                os << "\\n";
                break;
            case 3:
                os << "\\r";
                break;
            case 4:
                os << "\\f";
                break;
            case 5:
                os << "\\\\";
                break;
            case 6:
                os << "\\/";
                break;
            case 7:
                os << "\\\"";
                break;
            case 9:
                w = EncodeUtf16(x);
                do {
                    char esc[6];
                    esc[0] = '\\';
                    esc[1] = 'u';
                    esc[2] = "0123456789abcdef"[(w & 0xF000) >> 014];
                    esc[3] = "0123456789abcdef"[(w & 0x0F00) >> 010];
                    esc[4] = "0123456789abcdef"[(w & 0x00F0) >> 004];
                    esc[5] = "0123456789abcdef"[(w & 0x000F) >> 000];
                    os.write(esc, 6);
                } while ((w >>= 16));
                break;
            default:
                ON_LOGIC_ERROR("Unhandled character escape code during string serialization.");
        }
    }
}

json::Status
json::parse(json& j, const char*& p, const char* e, int context, int depth)
{
    char w[4];
    long long x;
    unsigned long long ux;
    const char* a;
    int A, B, C, D, c, d, i, u;
    if (!depth)
        return depth_exceeded;
    for (a = p, d = +1; p < e;) {
        switch ((c = *p++ & 255)) {
            case ' ': // spaces
            case '\n':
            case '\r':
            case '\t':
                a = p;
                break;

            case ',': // present in list and object
                if (context & COMMA) {
                    context = 0;
                    a = p;
                    break;
                } else {
                    return unexpected_comma;
                }

            case ':': // present only in object after key
                if (context & COLON) {
                    context = 0;
                    a = p;
                    break;
                } else {
                    return unexpected_colon;
                }

            case 'n': // null
                if (context & (KEY | COLON | COMMA))
                    goto OnColonCommaKey;
                if (p + 3 <= e && READ32LE(p - 1) == READ32LE("null")) {
                    p += 3;
                    return success;
                } else {
                    return illegal_character;
                }

            case 'f': // false
                if (context & (KEY | COLON | COMMA))
                    goto OnColonCommaKey;
                if (p + 4 <= e && READ32LE(p) == READ32LE("alse")) {
                    j.type_ = Type::Bool;
                    j.bool_value = false;
                    p += 4;
                    return success;
                } else {
                    return illegal_character;
                }

            case 't': // true
                if (context & (KEY | COLON | COMMA))
                    goto OnColonCommaKey;
                if (p + 3 <= e && READ32LE(p - 1) == READ32LE("true")) {
                    j.type_ = Type::Bool;
                    j.bool_value = true;
                    p += 3;
                    return success;
                } else {
                    return illegal_character;
                }

            default:
                return illegal_character;

            OnColonCommaKey:
                if (context & KEY)
                    return object_key_must_be_string;
            OnColonComma:
                if (context & COLON)
                    return missing_colon;
                return missing_comma;

            case '-': // negative
                if (context & (COLON | COMMA | KEY))
                    goto OnColonCommaKey;
                if (p < e && isdigit(*p)) {
                    d = -1;
                    break;
                } else {
                    return bad_negative;
                }

            case '0': // zero or number
                if (context & (COLON | COMMA | KEY))
                    goto OnColonCommaKey;
                if (p < e) {
                    if (*p == '.') {
                        if (p + 1 == e || !isdigit(p[1]))
                            return bad_double;
                        goto UseDubble;
                    } else if (*p == 'e' || *p == 'E') {
                        goto UseDubble;
                    } else if (isdigit(*p)) {
                        return unexpected_octal;
                    }
                }
                j.type_ = Type::Int;
                j.long_value = 0;
                return success;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': // integer
                if (context & (COLON | COMMA | KEY))
                    goto OnColonCommaKey;
                if (d > 0) {
                    for (ux = c - '0'; p < e; ++p) {
                        c = *p & 255;
                        if (isdigit(c)) {
                            if (ckd_mul(&ux, ux, 10u) ||
                                ckd_add(&ux, ux,
                                        static_cast<unsigned long long>(c - '0'))) {
                                goto UseDubble;
                            }
                        } else if (c == '.') {
                            if (p + 1 == e || !isdigit(p[1]))
                                return bad_double;
                            goto UseDubble;
                        } else if (c == 'e' || c == 'E') {
                            goto UseDubble;
                        } else {
                            break;
                        }
                    }
                    if (ux <= static_cast<unsigned long long>(LLONG_MAX)) {
                        j.type_ = Type::Int;
                        j.long_value = ux;
                    } else {
                        j.type_ = Type::Uint;
                        j.ulong_value = ux;
                    }
                } else {
                    for (x = (c - '0') * d; p < e; ++p) {
                        c = *p & 255;
                        if (isdigit(c)) {
                            if (ckd_mul(&x, x, 10) ||
                                ckd_add(&x, x, (c - '0') * d)) {
                                goto UseDubble;
                            }
                        } else if (c == '.') {
                            if (p + 1 == e || !isdigit(p[1]))
                                return bad_double;
                            goto UseDubble;
                        } else if (c == 'e' || c == 'E') {
                            goto UseDubble;
                        } else {
                            break;
                        }
                    }
                    j.type_ = Type::Int;
                    j.long_value = x;
                }
                return success;

            UseDubble: // number
                j.type_ = Type::Double;
                j.double_value = StringToDouble(a, e - a, &c);
                if (c <= 0)
                    return bad_double;
                if (a + c < e && (a[c] == 'e' || a[c] == 'E'))
                    return bad_exponent;
                p = a + c;
                return success;

            case '[': { // Array
                if (context & (COLON | COMMA | KEY))
                    goto OnColonCommaKey;
                j.set_array();
                json value;
                for (context = ARRAY, i = 0;;) {
                    Status status = parse(value, p, e, context, depth - 1);
                    if (status == absent_value)
                        return success;
                    if (status != success)
                        return status;
                    j.array_value.emplace_back(std::move(value));
                    context = ARRAY | COMMA;
                }
            }

            case ']':
                if (context & ARRAY)
                    return absent_value;
                return unexpected_end_of_array;

            case '}':
                if (context & OBJECT)
                    return absent_value;
                return unexpected_end_of_object;

            case '{': { // Object
                if (context & (COLON | COMMA | KEY))
                    goto OnColonCommaKey;
                j.set_object();
                context = KEY | OBJECT;
                json key, value;
                for (;;) {
                    Status status = parse(key, p, e, context, depth - 1);
                    if (status == absent_value)
                        return success;
                    if (status != success)
                        return status;
                    if (!key.is_string())
                        return object_key_must_be_string;
                    status = parse(value, p, e, COLON, depth - 1);
                    if (status == absent_value)
                        return object_missing_value;
                    if (status != success)
                        return status;
                    j.object_value.emplace(std::move(key.string_value),
                                              std::move(value));
                    context = KEY | COMMA | OBJECT;
                    key.clear();
                }
            }

            case '"': { // string
                std::string b;
                if (context & (COLON | COMMA))
                    goto OnColonComma;
                for (;;) {
                    if (p >= e)
                        return unexpected_end_of_string;
                    switch (kJsonStr[(c = *p++ & 255)]) {

                        case ASCII:
                            b += c;
                            break;

                        case DQUOTE:
                            j.type_ = Type::String;
                            new (&j.string_value) std::string(std::move(b));
                            return success;

                        case BACKSLASH:
                            if (p >= e)
                                return unexpected_end_of_string;
                            switch ((c = *p++ & 255)) {
                                case '"':
                                case '/':
                                case '\\':
                                    b += c;
                                    break;
                                case 'b':
                                    b += '\b';
                                    break;
                                case 'f':
                                    b += '\f';
                                    break;
                                case 'n':
                                    b += '\n';
                                    break;
                                case 'r':
                                    b += '\r';
                                    break;
                                case 't':
                                    b += '\t';
                                    break;
                                case 'x':
                                    if (p + 2 <= e && //
                                        (A = kHexToInt[p[0] & 255]) !=
                                          -1 && // HEX
                                        (B = kHexToInt[p[1] & 255]) != -1) { //
                                        c = A << 4 | B;
                                        if (!(0x20 <= c && c <= 0x7E))
                                            return hex_escape_not_printable;
                                        p += 2;
                                        b += c;
                                        break;
                                    } else {
                                        return invalid_hex_escape;
                                    }
                                case 'u':
                                    if (p + 4 <= e && //
                                        (A = kHexToInt[p[0] & 255]) != -1 && //
                                        (B = kHexToInt[p[1] & 255]) !=
                                          -1 && // UCS-2
                                        (C = kHexToInt[p[2] & 255]) != -1 && //
                                        (D = kHexToInt[p[3] & 255]) != -1) { //
                                        c = A << 12 | B << 8 | C << 4 | D;
                                        if (!IsSurrogate(c)) {
                                            p += 4;
                                        } else if (IsHighSurrogate(c)) {
                                            if (p + 4 + 6 <= e && //
                                                p[4] == '\\' && //
                                                p[5] == 'u' && //
                                                (A = kHexToInt[p[6] & 255]) !=
                                                  -1 && // UTF-16
                                                (B = kHexToInt[p[7] & 255]) !=
                                                  -1 && //
                                                (C = kHexToInt[p[8] & 255]) !=
                                                  -1 && //
                                                (D = kHexToInt[p[9] & 255]) !=
                                                  -1) { //
                                                u =
                                                  A << 12 | B << 8 | C << 4 | D;
                                                if (IsLowSurrogate(u)) {
                                                    p += 4 + 6;
                                                    c = MergeUtf16(c, u);
                                                } else {
                                                    goto BadUnicode;
                                                }
                                            } else {
                                                goto BadUnicode;
                                            }
                                        } else {
                                            goto BadUnicode;
                                        }
                                        // UTF-8
                                    EncodeUtf8:
                                        if (c <= 0x7f) {
                                            w[0] = c;
                                            i = 1;
                                        } else if (c <= 0x7ff) {
                                            w[0] = 0300 | (c >> 6);
                                            w[1] = 0200 | (c & 077);
                                            i = 2;
                                        } else if (c <= 0xffff) {
                                            if (IsSurrogate(c)) {
                                            ReplacementCharacter:
                                                c = 0xfffd;
                                            }
                                            w[0] = 0340 | (c >> 12);
                                            w[1] = 0200 | ((c >> 6) & 077);
                                            w[2] = 0200 | (c & 077);
                                            i = 3;
                                        } else if (~(c >> 18) & 007) {
                                            w[0] = 0360 | (c >> 18);
                                            w[1] = 0200 | ((c >> 12) & 077);
                                            w[2] = 0200 | ((c >> 6) & 077);
                                            w[3] = 0200 | (c & 077);
                                            i = 4;
                                        } else {
                                            goto ReplacementCharacter;
                                        }
                                        b.append(w, i);
                                    } else {
                                        return invalid_unicode_escape;
                                    BadUnicode:
                                        // Echo invalid \uXXXX sequences
                                        // Rather than corrupting UTF-8!
                                        b += "\\u";
                                    }
                                    break;
                                default:
                                    return invalid_escape_character;
                            }
                            break;

                        case UTF8_2:
                            if (p < e && //
                                (p[0] & 0300) == 0200) { //
                                c = (c & 037) << 6 | //
                                    (p[0] & 077); //
                                p += 1;
                                goto EncodeUtf8;
                            } else {
                                return malformed_utf8;
                            }

                        case UTF8_3_E0:
                            if (p + 2 <= e && //
                                (p[0] & 0377) < 0240 && //
                                (p[0] & 0300) == 0200 && //
                                (p[1] & 0300) == 0200) {
                                return overlong_utf8_0x7ff;
                            }
                            // fallthrough

                        case UTF8_3:
                        ThreeUtf8:
                            if (p + 2 <= e && //
                                (p[0] & 0300) == 0200 && //
                                (p[1] & 0300) == 0200) { //
                                c = (c & 017) << 12 | //
                                    (p[0] & 077) << 6 | //
                                    (p[1] & 077); //
                                p += 2;
                                goto EncodeUtf8;
                            } else {
                                return malformed_utf8;
                            }

                        case UTF8_3_ED:
                            if (p + 2 <= e && //
                                (p[0] & 0377) >= 0240) { //
                                if (p + 5 <= e && //
                                    (p[0] & 0377) >= 0256 && //
                                    (p[1] & 0300) == 0200 && //
                                    (p[2] & 0377) == 0355 && //
                                    (p[3] & 0377) >= 0260 && //
                                    (p[4] & 0300) == 0200) { //
                                    A = (0355 & 017) << 12 | // CESU-8
                                        (p[0] & 077) << 6 | //
                                        (p[1] & 077); //
                                    B = (0355 & 017) << 12 | //
                                        (p[3] & 077) << 6 | //
                                        (p[4] & 077); //
                                    c = ((A - 0xDB80) << 10) + //
                                        ((B - 0xDC00) + 0x10000); //
                                    goto EncodeUtf8;
                                } else if ((p[0] & 0300) == 0200 && //
                                           (p[1] & 0300) == 0200) { //
                                    return utf16_surrogate_in_utf8;
                                } else {
                                    return malformed_utf8;
                                }
                            }
                            goto ThreeUtf8;

                        case UTF8_4_F0:
                            if (p + 3 <= e && (p[0] & 0377) < 0220 &&
                                (((uint_least32_t)(p[+2] & 0377) << 030 |
                                  (uint_least32_t)(p[+1] & 0377) << 020 |
                                  (uint_least32_t)(p[+0] & 0377) << 010 |
                                  (uint_least32_t)(p[-1] & 0377) << 000) &
                                 0xC0C0C000) == 0x80808000) {
                                return overlong_utf8_0xffff;
                            }
                            // fallthrough
                        case UTF8_4:
                            if (p + 3 <= e && //
                                ((A =
                                    ((uint_least32_t)(p[+2] & 0377) << 030 | //
                                     (uint_least32_t)(p[+1] & 0377) << 020 | //
                                     (uint_least32_t)(p[+0] & 0377) << 010 | //
                                     (uint_least32_t)(p[-1] & 0377)
                                       << 000)) & //
                                 0xC0C0C000) == 0x80808000) { //
                                A = (A & 7) << 18 | //
                                    (A & (077 << 010)) << (12 - 010) | //
                                    (A & (077 << 020)) >> -(6 - 020) | //
                                    (A & (077 << 030)) >> 030; //
                                if (A <= 0x10FFFF) {
                                    c = A;
                                    p += 3;
                                    goto EncodeUtf8;
                                } else {
                                    return utf8_exceeds_utf16_range;
                                }
                            } else {
                                return malformed_utf8;
                            }

                        case EVILUTF8:
                            if (p < e && (p[0] & 0300) == 0200)
                                return overlong_ascii;
                            // fallthrough
                        case BADUTF8:
                            return illegal_utf8_character;
                        case C0:
                            return non_del_c0_control_code_in_string;
                        case C1:
                            return c1_control_code_in_string;
                        default:
                            ON_LOGIC_ERROR("Unhandled character category during string parsing.");
                    }
                }
            }
        }
    }
    if (depth == DEPTH)
        return absent_value;
    return unexpected_eof;
}

wpi::util::expected<json, const char*>
json::parse(std::string_view s)
{
    const char* p = s.data();
    const char* e = s.data() + s.size();
    json j;
    Status result = parse(j, p, e, 0, DEPTH);
    if (result != success) {
        return wpi::util::unexpected(StatusToString(result));
    }
    json j2;
    Status s2 = parse(j2, p, e, 0, DEPTH);
    if (s2 != absent_value) {
        return wpi::util::unexpected(StatusToString(trailing_content));
    }
    return j;
}

json
json::parse_or_throw(std::string_view s)
{
    auto result = parse(s);
    if (!result) {
        ON_LOGIC_ERROR(result.error());
    }
    return *result;
}

const char*
json::StatusToString(json::Status status)
{
    switch (status) {
        case success:
            return "success";
        case bad_double:
            return "bad_double";
        case absent_value:
            return "absent_value";
        case bad_negative:
            return "bad_negative";
        case bad_exponent:
            return "bad_exponent";
        case missing_comma:
            return "missing_comma";
        case missing_colon:
            return "missing_colon";
        case malformed_utf8:
            return "malformed_utf8";
        case depth_exceeded:
            return "depth_exceeded";
        case stack_overflow:
            return "stack_overflow";
        case unexpected_eof:
            return "unexpected_eof";
        case overlong_ascii:
            return "overlong_ascii";
        case unexpected_comma:
            return "unexpected_comma";
        case unexpected_colon:
            return "unexpected_colon";
        case unexpected_octal:
            return "unexpected_octal";
        case trailing_content:
            return "trailing_content";
        case illegal_character:
            return "illegal_character";
        case invalid_hex_escape:
            return "invalid_hex_escape";
        case overlong_utf8_0x7ff:
            return "overlong_utf8_0x7ff";
        case overlong_utf8_0xffff:
            return "overlong_utf8_0xffff";
        case object_missing_value:
            return "object_missing_value";
        case illegal_utf8_character:
            return "illegal_utf8_character";
        case invalid_unicode_escape:
            return "invalid_unicode_escape";
        case utf16_surrogate_in_utf8:
            return "utf16_surrogate_in_utf8";
        case unexpected_end_of_array:
            return "unexpected_end_of_array";
        case hex_escape_not_printable:
            return "hex_escape_not_printable";
        case invalid_escape_character:
            return "invalid_escape_character";
        case utf8_exceeds_utf16_range:
            return "utf8_exceeds_utf16_range";
        case unexpected_end_of_string:
            return "unexpected_end_of_string";
        case unexpected_end_of_object:
            return "unexpected_end_of_object";
        case object_key_must_be_string:
            return "object_key_must_be_string";
        case c1_control_code_in_string:
            return "c1_control_code_in_string";
        case non_del_c0_control_code_in_string:
            return "non_del_c0_control_code_in_string";
        default:
            ON_LOGIC_ERROR("Unhandled Json status value.");
    }
}

bool
operator==(const json& lhs, const json& rhs) {
    if (lhs.type_ != rhs.type_)
        return false;
    switch (lhs.type_) {
        case json::Type::Null:
            return true;
        case json::Type::String:
            return lhs.string_value == rhs.string_value;
        case json::Type::Bool:
            return lhs.bool_value == rhs.bool_value;
        case json::Type::Int:
            return lhs.long_value == rhs.long_value;
        case json::Type::Uint:
            return lhs.ulong_value == rhs.ulong_value;
        case json::Type::Float:
            return lhs.float_value == rhs.float_value;
        case json::Type::Double:
            return lhs.double_value == rhs.double_value;
        case json::Type::Array:
            return lhs.array_value == rhs.array_value;
        case json::Type::Object:
            return lhs.object_value == rhs.object_value;
        default:
            ON_LOGIC_ERROR("Unhandled JSON type during equality comparison.");
    }
}

} // namespace wpi::util
