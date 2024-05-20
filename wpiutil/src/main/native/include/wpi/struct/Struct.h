// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/Endian.h"
#include "wpi/bit.h"
#include "wpi/ct_string.h"
#include "wpi/function_ref.h"
#include "wpi/mutex.h"
#include "wpi/type_traits.h"

namespace wpi {

/**
 * Struct serialization template. Unspecialized class has no members; only
 * specializations of this class are useful, and only if they meet the
 * StructSerializable concept.
 *
 * @tparam T type to serialize/deserialize
 * @tparam I optional struct type info
 */
template <typename T, typename... I>
struct Struct {};

/**
 * Specifies that a type is capable of raw struct serialization and
 * deserialization.
 *
 * This is designed for serializing small fixed-size data structures in the
 * fastest and most compact means possible. Serialization consists of writing
 * values into a mutable std::span and deserialization consists of reading
 * values from an immutable std::span.
 *
 * Implementations must define a template specialization for wpi::Struct with
 * T being the type that is being serialized/deserialized, with the following
 * static members (as enforced by this concept):
 * - std::string_view GetTypeString(): function that returns the type string
 * - size_t GetSize(): function that returns the structure size in bytes
 * - std::string_view GetSchema(): function that returns the struct schema
 * - T Unpack(std::span<const uint8_t>): function for deserialization
 * - void Pack(std::span<uint8_t>, T&& value): function for
 *   serialization
 *
 * If possible, the GetTypeString(), GetSize(), and GetSchema() functions should
 * be marked constexpr. GetTypeString() and GetSchema() may return types other
 * than std::string_view, as long as the return value is convertible to
 * std::string_view.
 *
 * If the struct has nested structs, implementations should also meet the
 * requirements of HasNestedStruct<T>.
 */
template <typename T, typename... I>
concept StructSerializable = requires(std::span<const uint8_t> in,
                                      std::span<uint8_t> out, T&& value,
                                      const I&... info) {
  typename Struct<typename std::remove_cvref_t<T>,
                  typename std::remove_cvref_t<I>...>;
  {
    Struct<typename std::remove_cvref_t<T>,
           typename std::remove_cvref_t<I>...>::GetTypeString(info...)
  } -> std::convertible_to<std::string_view>;
  {
    Struct<typename std::remove_cvref_t<T>,
           typename std::remove_cvref_t<I>...>::GetSize(info...)
  } -> std::convertible_to<size_t>;
  {
    Struct<typename std::remove_cvref_t<T>,
           typename std::remove_cvref_t<I>...>::GetSchema(info...)
  } -> std::convertible_to<std::string_view>;
  {
    Struct<typename std::remove_cvref_t<T>,
           typename std::remove_cvref_t<I>...>::Unpack(in, info...)
  } -> std::same_as<typename std::remove_cvref_t<T>>;
  Struct<typename std::remove_cvref_t<T>,
         typename std::remove_cvref_t<I>...>::Pack(out, std::forward<T>(value),
                                                   info...);
};

/**
 * Specifies that a type is capable of in-place raw struct deserialization.
 *
 * In addition to meeting StructSerializable, implementations must define a
 * wpi::Struct<T> static member `void UnpackInto(T*, std::span<const uint8_t>)`
 * to update the pointed-to T with the contents of the span.
 */
template <typename T, typename... I>
concept MutableStructSerializable =
    StructSerializable<T, I...> &&
    requires(T* out, std::span<const uint8_t> in, const I&... info) {
      Struct<typename std::remove_cvref_t<T>,
             typename std::remove_cvref_t<I>...>::UnpackInto(out, in, info...);
    };

/**
 * Specifies that a struct type has nested struct declarations.
 *
 * In addition to meeting StructSerializable, implementations must define a
 * wpi::Struct<T> static member
 * `void ForEachNested(std::invocable<std::string_view, std::string_view) auto
 * fn)` (or equivalent) and call ForEachNestedStruct<Type> on each nested struct
 * type.
 */
template <typename T, typename... I>
concept HasNestedStruct =
    StructSerializable<T, I...> &&
    requires(function_ref<void(std::string_view, std::string_view)> fn,
             const I&... info) {
      Struct<typename std::remove_cvref_t<T>,
             typename std::remove_cvref_t<I>...>::ForEachNested(fn, info...);
    };

/**
 * Unpack a serialized struct.
 *
 * @tparam T object type
 * @param data raw struct data
 * @param info optional struct type info
 * @return Deserialized object
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
inline T UnpackStruct(std::span<const uint8_t> data, const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  return S::Unpack(data, info...);
}

/**
 * Unpack a serialized struct starting at a given offset within the data.
 * This is primarily useful in unpack implementations to unpack nested structs.
 *
 * @tparam T object type
 * @tparam Offset starting offset
 * @param data raw struct data
 * @param info optional struct type info
 * @return Deserialized object
 */
template <typename T, size_t Offset, typename... I>
  requires StructSerializable<T, I...>
inline T UnpackStruct(std::span<const uint8_t> data, const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  return S::Unpack(data.subspan(Offset), info...);
}

/**
 * Pack a serialized struct.
 *
 * @param data struct storage (mutable, output)
 * @param value object
 * @param info optional struct type info
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
inline void PackStruct(std::span<uint8_t> data, T&& value, const I&... info) {
  using S = Struct<typename std::remove_cvref_t<T>,
                   typename std::remove_cvref_t<I>...>;
  S::Pack(data, std::forward<T>(value), info...);
}

/**
 * Pack a serialized struct starting at a given offset within the data. This is
 * primarily useful in pack implementations to pack nested structs.
 *
 * @tparam Offset starting offset
 * @param data struct storage (mutable, output)
 * @param value object
 * @param info optional struct type info
 */
template <size_t Offset, typename T, typename... I>
  requires StructSerializable<T, I...>
inline void PackStruct(std::span<uint8_t> data, T&& value, const I&... info) {
  using S = Struct<typename std::remove_cvref_t<T>,
                   typename std::remove_cvref_t<I>...>;
  S::Pack(data.subspan(Offset), std::forward<T>(value), info...);
}

/**
 * Unpack a serialized struct into an existing object, overwriting its contents.
 *
 * @param out object (output)
 * @param data raw struct data
 * @param info optional struct type info
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
inline void UnpackStructInto(T* out, std::span<const uint8_t> data,
                             const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  if constexpr (MutableStructSerializable<T, I...>) {
    S::UnpackInto(out, data, info...);
  } else {
    *out = UnpackStruct<T>(data, info...);
  }
}

/**
 * Unpack a serialized struct into an existing object, overwriting its contents,
 * and starting at a given offset within the data.
 * This is primarily useful in unpack implementations to unpack nested structs.
 *
 * @tparam Offset starting offset
 * @param out object (output)
 * @param data raw struct data
 * @param info optional struct type info
 */
template <size_t Offset, typename T, typename... I>
  requires StructSerializable<T, I...>
inline void UnpackStructInto(T* out, std::span<const uint8_t> data,
                             const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  if constexpr (MutableStructSerializable<T, I...>) {
    S::UnpackInto(out, data.subspan(Offset), info...);
  } else {
    *out = UnpackStruct<T, Offset>(data, info...);
  }
}

/**
 * Get the type string for a raw struct serializable type
 *
 * @tparam T serializable type
 * @param info optional struct type info
 * @return type string
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
constexpr auto GetStructTypeString(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  return S::GetTypeString(info...);
}

/**
 * Get the size for a raw struct serializable type
 *
 * @tparam T serializable type
 * @param info optional struct type info
 * @return size
 */
template <typename T, typename... I>
  requires StructSerializable<T, I...>
constexpr size_t GetStructSize(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  return S::GetSize(info...);
}

template <typename T, size_t N, typename... I>
  requires StructSerializable<T, I...>
constexpr auto MakeStructArrayTypeString(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  if constexpr (sizeof...(I) == 0 &&
                is_constexpr([&] { S::GetTypeString(info...); })) {
    constexpr auto typeString = S::GetTypeString(info...);
    using namespace literals;
    if constexpr (N == std::dynamic_extent) {
      return Concat(
          ct_string<char, std::char_traits<char>, typeString.size()>{
              typeString},
          "[]"_ct_string);
    } else {
      return Concat(
          ct_string<char, std::char_traits<char>, typeString.size()>{
              typeString},
          "["_ct_string, NumToCtString<N>(), "]"_ct_string);
    }
  } else {
    if constexpr (N == std::dynamic_extent) {
      return fmt::format("{}[]", S::GetTypeString(info...));
    } else {
      return fmt::format("{}[{}]", S::GetTypeString(info...), N);
    }
  }
}

template <typename T, size_t N, typename... I>
  requires StructSerializable<T, I...>
constexpr auto MakeStructArraySchema(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  if constexpr (sizeof...(I) == 0 &&
                is_constexpr([&] { S::GetSchema(info...); })) {
    constexpr auto schema = S::GetSchema(info...);
    using namespace literals;
    if constexpr (N == std::dynamic_extent) {
      return Concat(
          ct_string<char, std::char_traits<char>, schema.size()>{schema},
          "[]"_ct_string);
    } else {
      return Concat(
          ct_string<char, std::char_traits<char>, schema.size()>{schema},
          "["_ct_string, NumToCtString<N>(), "]"_ct_string);
    }
  } else {
    if constexpr (N == std::dynamic_extent) {
      return fmt::format("{}[]", S::GetSchema(info...));
    } else {
      return fmt::format("{}[{}]", S::GetSchema(info...), N);
    }
  }
}

template <typename T, typename... I>
  requires StructSerializable<T, I...>
constexpr std::string_view GetStructSchema(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  return S::GetSchema(info...);
}

template <typename T, typename... I>
  requires StructSerializable<T, I...>
constexpr std::span<const uint8_t> GetStructSchemaBytes(const I&... info) {
  using S = Struct<T, typename std::remove_cvref_t<I>...>;
  auto schema = S::GetSchema(info...);
  return {reinterpret_cast<const uint8_t*>(schema.data()), schema.size()};
}

template <typename T, typename... I>
  requires StructSerializable<T, I...>
void ForEachStructSchema(
    std::invocable<std::string_view, std::string_view> auto fn,
    const I&... info) {
  using S = Struct<typename std::remove_cvref_t<T>,
                   typename std::remove_cvref_t<I>...>;
  if constexpr (HasNestedStruct<T, I...>) {
    S::ForEachNested(fn, info...);
  }
  fn(S::GetTypeString(info...), S::GetSchema(info...));
}

template <typename T, typename... I>
  requires StructSerializable<T, I...>
class StructArrayBuffer {
  using S = Struct<T, I...>;

 public:
  StructArrayBuffer() = default;
  StructArrayBuffer(const StructArrayBuffer&) = delete;
  StructArrayBuffer& operator=(const StructArrayBuffer&) = delete;
  StructArrayBuffer(StructArrayBuffer&& rhs) : m_buf{std::move(rhs.m_buf)} {}
  StructArrayBuffer& operator=(StructArrayBuffer&& rhs) {
    m_buf = std::move(rhs.m_buf);
    return *this;
  }

  template <typename U, typename F>
    requires
#if __cpp_lib_ranges >= 201911L
      std::ranges::range<U> &&
      std::convertible_to<std::ranges::range_value_t<U>, T> &&
#endif
      std::invocable<F, std::span<const uint8_t>>
    void Write(U&& data, F&& func, const I&... info) {
    auto size = S::GetSize(info...);
    if ((std::size(data) * size) < 256) {
      // use the stack
      uint8_t buf[256];
      auto out = buf;
      for (auto&& val : data) {
        S::Pack(std::span<uint8_t>{std::to_address(out), size},
                std::forward<decltype(val)>(val), info...);
        out += size;
      }
      func(std::span<uint8_t>{buf, out});
    } else {
      std::scoped_lock lock{m_mutex};
      m_buf.resize(std::size(data) * size);
      auto out = m_buf.begin();
      for (auto&& val : data) {
        S::Pack(std::span<uint8_t>{std::to_address(out), size},
                std::forward<decltype(val)>(val), info...);
        out += size;
      }
      func(m_buf);
    }
  }

 private:
  wpi::mutex m_mutex;
  std::vector<uint8_t> m_buf;
};

/**
 * Raw struct support for fixed-size arrays of other structs.
 */
template <typename T, size_t N, typename... I>
  requires StructSerializable<T, I...>
struct Struct<std::array<T, N>, I...> {
  static constexpr auto GetTypeString(const I&... info) {
    return MakeStructArrayTypeString<T, N>(info...);
  }
  static constexpr size_t GetSize(const I&... info) {
    return N * GetStructSize<T>(info...);
  }
  static constexpr auto GetSchema(const I&... info) {
    return MakeStructArraySchema<T, N>(info...);
  }
  static std::array<T, N> Unpack(std::span<const uint8_t> data,
                                 const I&... info) {
    auto size = GetStructSize<T>(info...);
    std::array<T, N> result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = UnpackStruct<T, 0>(data, info...);
      data = data.subspan(size);
    }
    return result;
  }
  static void Pack(std::span<uint8_t> data, std::span<const T, N> values,
                   const I&... info) {
    auto size = GetStructSize<T>(info...);
    std::span<uint8_t> unsizedData = data;
    for (auto&& val : values) {
      PackStruct(unsizedData, val, info...);
      unsizedData = unsizedData.subspan(size);
    }
  }
  static void UnpackInto(std::array<T, N>* out, std::span<const uint8_t> data,
                         const I&... info) {
    UnpackInto(std::span{*out}, data, info...);
  }
  // alternate span-based function
  static void UnpackInto(std::span<T, N> out, std::span<const uint8_t> data,
                         const I&... info) {
    auto size = GetStructSize<T>(info...);
    std::span<const uint8_t> unsizedData = data;
    for (size_t i = 0; i < N; ++i) {
      UnpackStructInto(&out[i], unsizedData, info...);
      unsizedData = unsizedData.subspan(size);
    }
  }
};

/**
 * Raw struct support for boolean values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<bool> {
  static constexpr std::string_view GetTypeString() { return "struct:bool"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "bool value"; }
  static bool Unpack(std::span<const uint8_t> data) { return data[0]; }
  static void Pack(std::span<uint8_t> data, bool value) {
    data[0] = static_cast<char>(value ? 1 : 0);
  }
};

/**
 * Raw struct support for uint8_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint8_t> {
  static constexpr std::string_view GetTypeString() { return "struct:uint8"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "uint8 value"; }
  static uint8_t Unpack(std::span<const uint8_t> data) { return data[0]; }
  static void Pack(std::span<uint8_t> data, uint8_t value) { data[0] = value; }
};

/**
 * Raw struct support for int8_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int8_t> {
  static constexpr std::string_view GetTypeString() { return "struct:int8"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "int8 value"; }
  static int8_t Unpack(std::span<const uint8_t, 1> data) { return data[0]; }
  static void Pack(std::span<uint8_t, 1> data, int8_t value) {
    data[0] = value;
  }
};

/**
 * Raw struct support for uint16_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint16_t> {
  static constexpr std::string_view GetTypeString() { return "struct:uint16"; }
  static constexpr size_t GetSize() { return 2; }
  static constexpr std::string_view GetSchema() { return "uint16 value"; }
  static uint16_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read16le(data.data());
  }
  static void Pack(std::span<uint8_t> data, uint16_t value) {
    support::endian::write16le(data.data(), value);
  }
};

/**
 * Raw struct support for int16_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int16_t> {
  static constexpr std::string_view GetTypeString() { return "struct:int16"; }
  static constexpr size_t GetSize() { return 2; }
  static constexpr std::string_view GetSchema() { return "int16 value"; }
  static int16_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read16le(data.data());
  }
  static void Pack(std::span<uint8_t> data, int16_t value) {
    support::endian::write16le(data.data(), value);
  }
};

/**
 * Raw struct support for uint32_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint32_t> {
  static constexpr std::string_view GetTypeString() { return "struct:uint32"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "uint32 value"; }
  static uint32_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read32le(data.data());
  }
  static void Pack(std::span<uint8_t> data, uint32_t value) {
    support::endian::write32le(data.data(), value);
  }
};

/**
 * Raw struct support for int32_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int32_t> {
  static constexpr std::string_view GetTypeString() { return "struct:int32"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "int32 value"; }
  static int32_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read32le(data.data());
  }
  static void Pack(std::span<uint8_t> data, int32_t value) {
    support::endian::write32le(data.data(), value);
  }
};

/**
 * Raw struct support for uint64_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint64_t> {
  static constexpr std::string_view GetTypeString() { return "struct:uint64"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "uint64 value"; }
  static uint64_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read64le(data.data());
  }
  static void Pack(std::span<uint8_t> data, uint64_t value) {
    support::endian::write64le(data.data(), value);
  }
};

/**
 * Raw struct support for int64_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int64_t> {
  static constexpr std::string_view GetTypeString() { return "struct:int64"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "int64 value"; }
  static int64_t Unpack(std::span<const uint8_t> data) {
    return support::endian::read64le(data.data());
  }
  static void Pack(std::span<uint8_t> data, int64_t value) {
    support::endian::write64le(data.data(), value);
  }
};

/**
 * Raw struct support for float values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<float> {
  static constexpr std::string_view GetTypeString() { return "struct:float"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "float value"; }
  static float Unpack(std::span<const uint8_t> data) {
    return bit_cast<float>(support::endian::read32le(data.data()));
  }
  static void Pack(std::span<uint8_t> data, float value) {
    support::endian::write32le(data.data(), bit_cast<uint32_t>(value));
  }
};

/**
 * Raw struct support for double values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<double> {
  static constexpr std::string_view GetTypeString() { return "struct:double"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "double value"; }
  static double Unpack(std::span<const uint8_t> data) {
    return bit_cast<double>(support::endian::read64le(data.data()));
  }
  static void Pack(std::span<uint8_t> data, double value) {
    support::endian::write64le(data.data(), bit_cast<uint64_t>(value));
  }
};

}  // namespace wpi
