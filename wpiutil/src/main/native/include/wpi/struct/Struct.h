// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/Endian.h"
#include "wpi/MathExtras.h"
#include "wpi/bit.h"
#include "wpi/ct_string.h"
#include "wpi/function_ref.h"
#include "wpi/mutex.h"

namespace wpi {

/**
 * Struct serialization template. Unspecialized class has no members; only
 * specializations of this class are useful, and only if they meet the
 * StructSerializable concept.
 *
 * @tparam T type to serialize/deserialize
 */
template <typename T>
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
 * - std::string_view kTypeString: the type string
 * - size_t kSize: the structure size in bytes
 * - std::string_view kSchema: the struct schema
 * - T Unpack(std::span<const uint8_t, kSize>): function for deserialization
 * - void Pack(std::span<uint8_t, kSize>, T&& value): function for
 *   serialization
 *
 * If the struct has nested structs, implementations should also meet the
 * requirements of HasNestedStruct<T>.
 */
template <typename T>
concept StructSerializable =
    requires(std::span<const uint8_t> in, std::span<uint8_t> out, T&& value) {
      typename Struct<typename std::remove_cvref_t<T>>;
      {
        Struct<typename std::remove_cvref_t<T>>::kTypeString
      } -> std::convertible_to<std::string_view>;
      {
        Struct<typename std::remove_cvref_t<T>>::kSize
      } -> std::convertible_to<size_t>;
      {
        Struct<typename std::remove_cvref_t<T>>::kSchema
      } -> std::convertible_to<std::string_view>;
      {
        Struct<typename std::remove_cvref_t<T>>::Unpack(
            in.subspan<0, Struct<typename std::remove_cvref_t<T>>::kSize>())
      } -> std::same_as<typename std::remove_cvref_t<T>>;
      Struct<typename std::remove_cvref_t<T>>::Pack(
          out.subspan<0, Struct<typename std::remove_cvref_t<T>>::kSize>(),
          std::forward<T>(value));
    };

/**
 * Specifies that a type is capable of in-place raw struct deserialization.
 *
 * In addition to meeting StructSerializable, implementations must define a
 * wpi::Struct<T> static member `void UnpackInto(T*, std::span<const uint8_t>)`
 * to update the pointed-to T with the contents of the span.
 */
template <typename T>
concept MutableStructSerializable =
    StructSerializable<T> && requires(T* out, std::span<const uint8_t> in) {
      Struct<typename std::remove_cvref_t<T>>::UnpackInto(
          out, in.subspan<0, Struct<typename std::remove_cvref_t<T>>::kSize>());
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
template <typename T>
concept HasNestedStruct =
    StructSerializable<T> &&
    requires(function_ref<void(std::string_view, std::string_view)> fn) {
      Struct<typename std::remove_cvref_t<T>>::ForEachNested(fn);
    };

/**
 * Unpack a serialized struct.
 *
 * @tparam T object type
 * @param data raw struct data
 * @return Deserialized object
 */
template <StructSerializable T>
inline T UnpackStruct(std::span<const uint8_t, Struct<T>::kSize> data) {
  return Struct<T>::Unpack(data);
}

/**
 * Unpack a serialized struct starting at a given offset within the data.
 * This is primarily useful in unpack implementations to unpack nested structs.
 *
 * @tparam T object type
 * @tparam Offset starting offset
 * @param data raw struct data
 * @return Deserialized object
 */
template <StructSerializable T, size_t Offset>
inline T UnpackStruct(std::span<const uint8_t> data) {
  return Struct<T>::Unpack(data.template subspan<Offset, Struct<T>::kSize>());
}

/**
 * Pack a serialized struct.
 *
 * @param data struct storage (mutable, output)
 * @param value object
 */
template <StructSerializable T>
inline void PackStruct(
    std::span<uint8_t, Struct<typename std::remove_cvref_t<T>>::kSize> data,
    T&& value) {
  Struct<typename std::remove_cvref_t<T>>::Pack(data, std::forward<T>(value));
}

/**
 * Pack a serialized struct starting at a given offset within the data. This is
 * primarily useful in pack implementations to pack nested structs.
 *
 * @tparam Offset starting offset
 * @param data struct storage (mutable, output)
 * @param value object
 */
template <size_t Offset, StructSerializable T>
inline void PackStruct(std::span<uint8_t> data, T&& value) {
  Struct<typename std::remove_cvref_t<T>>::Pack(
      data.template subspan<Offset,
                            Struct<typename std::remove_cvref_t<T>>::kSize>(),
      std::forward<T>(value));
}

/**
 * Unpack a serialized struct into an existing object, overwriting its contents.
 *
 * @param out object (output)
 * @param data raw struct data
 */
template <StructSerializable T>
inline void UnpackStructInto(T* out,
                             std::span<const uint8_t, Struct<T>::kSize> data) {
  if constexpr (MutableStructSerializable<T>) {
    Struct<T>::UnpackInto(out, data);
  } else {
    *out = UnpackStruct<T>(data);
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
 */
template <size_t Offset, StructSerializable T>
inline void UnpackStructInto(T* out, std::span<const uint8_t> data) {
  if constexpr (MutableStructSerializable<T>) {
    Struct<T>::UnpackInto(out,
                          data.template subspan<Offset, Struct<T>::kSize>());
  } else {
    *out = UnpackStruct<T, Offset>(data);
  }
}

/**
 * Get the type string for a raw struct serializable type
 *
 * @tparam T serializable type
 * @return type string
 */
template <StructSerializable T>
constexpr auto GetStructTypeString() {
  return Struct<T>::kTypeString;
}

template <StructSerializable T, size_t N>
consteval auto MakeStructArrayTypeString() {
  using namespace literals;
  if constexpr (N == std::dynamic_extent) {
    return Concat(
        ct_string<char, std::char_traits<char>, Struct<T>::kTypeString.size()>{
            Struct<T>::kTypeString},
        "[]"_ct_string);
  } else {
    return Concat(
        ct_string<char, std::char_traits<char>, Struct<T>::kTypeString.size()>{
            Struct<T>::kTypeString},
        "["_ct_string, NumToCtString<N>(), "]"_ct_string);
  }
}

template <StructSerializable T, size_t N>
consteval auto MakeStructArraySchema() {
  using namespace literals;
  if constexpr (N == std::dynamic_extent) {
    return Concat(
        ct_string<char, std::char_traits<char>, Struct<T>::kSchema.size()>{
            Struct<T>::kSchema},
        "[]"_ct_string);
  } else {
    return Concat(
        ct_string<char, std::char_traits<char>, Struct<T>::kSchema.size()>{
            Struct<T>::kSchema},
        "["_ct_string, NumToCtString<N>(), "]"_ct_string);
  }
}

template <StructSerializable T>
constexpr std::string_view GetStructSchema() {
  return Struct<T>::kSchema;
}

template <StructSerializable T>
constexpr std::span<const uint8_t> GetStructSchemaBytes() {
  return {reinterpret_cast<const uint8_t*>(Struct<T>::kSchema.data()),
          Struct<T>::kSchema.size()};
}

template <StructSerializable T>
void ForEachStructSchema(
    std::invocable<std::string_view, std::string_view> auto fn) {
  if constexpr (HasNestedStruct<T>) {
    Struct<T>::ForEachNested(fn);
  }
  fn(Struct<T>::kTypeString, Struct<T>::kSchema);
}

template <StructSerializable T>
class StructArrayBuffer {
  using S = Struct<T>;

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
    void Write(U&& data, F&& func) {
    if ((std::size(data) * S::kSize) < 256) {
      // use the stack
      uint8_t buf[256];
      auto out = buf;
      for (auto&& val : data) {
        S::Pack(std::span<uint8_t, S::kSize>{out, out + S::kSize},
                std::forward<decltype(val)>(val));
        out += S::kSize;
      }
      func(std::span<uint8_t>{buf, out});
    } else {
      std::scoped_lock lock{m_mutex};
      m_buf.resize(std::size(data) * S::kSize);
      auto out = m_buf.begin();
      for (auto&& val : data) {
        S::Pack(std::span<uint8_t, S::kSize>{out, out + S::kSize},
                std::forward<decltype(val)>(val));
        out += S::kSize;
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
template <StructSerializable T, size_t N>
struct Struct<std::array<T, N>> {
  static constexpr auto kTypeString = MakeStructArrayTypeString<T, N>();
  static constexpr size_t kSize = N * Struct<T>::kSize;
  static constexpr auto kSchema = MakeStructArraySchema<T, N>();
  static std::array<T, N> Unpack(std::span<const uint8_t, kSize> data) {
    std::array<T, N> result;
    for (size_t i = 0; i < N; ++i) {
      result[i] = UnpackStruct<T, 0>(data);
      data = data.subspan(Struct<T>::kSize);
    }
    return result;
  }
  static void Pack(std::span<uint8_t, kSize> data,
                   std::span<const T, N> values) {
    std::span<uint8_t> unsizedData = data;
    for (auto&& val : values) {
      PackStruct<0>(unsizedData, val);
      unsizedData = unsizedData.subspan(Struct<T>::kSize);
    }
  }
  static void UnpackInto(std::array<T, N>* out,
                         std::span<const uint8_t, kSize> data) {
    UnpackInto(std::span{*out}, data);
  }
  // alternate span-based function
  static void UnpackInto(std::span<T, N> out,
                         std::span<const uint8_t, kSize> data) {
    std::span<const uint8_t> unsizedData = data;
    for (size_t i = 0; i < N; ++i) {
      UnpackStructInto<0, T>(&out[i], unsizedData);
      unsizedData = unsizedData.subspan(Struct<T>::kSize);
    }
  }
};

/**
 * Raw struct support for boolean values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<bool> {
  static constexpr std::string_view kTypeString = "struct:bool";
  static constexpr size_t kSize = 1;
  static constexpr std::string_view kSchema = "bool value";
  static bool Unpack(std::span<const uint8_t, 1> data) { return data[0]; }
  static void Pack(std::span<uint8_t, 1> data, bool value) {
    data[0] = static_cast<char>(value ? 1 : 0);
  }
};

/**
 * Raw struct support for uint8_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint8_t> {
  static constexpr std::string_view kTypeString = "struct:uint8";
  static constexpr size_t kSize = 1;
  static constexpr std::string_view kSchema = "uint8 value";
  static uint8_t Unpack(std::span<const uint8_t, 1> data) { return data[0]; }
  static void Pack(std::span<uint8_t, 1> data, uint8_t value) {
    data[0] = value;
  }
};

/**
 * Raw struct support for int8_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int8_t> {
  static constexpr std::string_view kTypeString = "struct:int8";
  static constexpr size_t kSize = 1;
  static constexpr std::string_view kSchema = "int8 value";
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
  static constexpr std::string_view kTypeString = "struct:uint16";
  static constexpr size_t kSize = 2;
  static constexpr std::string_view kSchema = "uint16 value";
  static uint16_t Unpack(std::span<const uint8_t, 2> data) {
    return support::endian::read16le(data.data());
  }
  static void Pack(std::span<uint8_t, 2> data, uint16_t value) {
    support::endian::write16le(data.data(), value);
  }
};

/**
 * Raw struct support for int16_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int16_t> {
  static constexpr std::string_view kTypeString = "struct:int16";
  static constexpr size_t kSize = 2;
  static constexpr std::string_view kSchema = "int16 value";
  static int16_t Unpack(std::span<const uint8_t, 2> data) {
    return support::endian::read16le(data.data());
  }
  static void Pack(std::span<uint8_t, 2> data, int16_t value) {
    support::endian::write16le(data.data(), value);
  }
};

/**
 * Raw struct support for uint32_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint32_t> {
  static constexpr std::string_view kTypeString = "struct:uint32";
  static constexpr size_t kSize = 4;
  static constexpr std::string_view kSchema = "uint32 value";
  static uint32_t Unpack(std::span<const uint8_t, 4> data) {
    return support::endian::read32le(data.data());
  }
  static void Pack(std::span<uint8_t, 4> data, uint32_t value) {
    support::endian::write32le(data.data(), value);
  }
};

/**
 * Raw struct support for int32_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int32_t> {
  static constexpr std::string_view kTypeString = "struct:int32";
  static constexpr size_t kSize = 4;
  static constexpr std::string_view kSchema = "int32 value";
  static int32_t Unpack(std::span<const uint8_t, 4> data) {
    return support::endian::read32le(data.data());
  }
  static void Pack(std::span<uint8_t, 4> data, int32_t value) {
    support::endian::write32le(data.data(), value);
  }
};

/**
 * Raw struct support for uint64_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<uint64_t> {
  static constexpr std::string_view kTypeString = "struct:uint64";
  static constexpr size_t kSize = 8;
  static constexpr std::string_view kSchema = "uint64 value";
  static uint64_t Unpack(std::span<const uint8_t, 8> data) {
    return support::endian::read64le(data.data());
  }
  static void Pack(std::span<uint8_t, 8> data, uint64_t value) {
    support::endian::write64le(data.data(), value);
  }
};

/**
 * Raw struct support for int64_t values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<int64_t> {
  static constexpr std::string_view kTypeString = "struct:int64";
  static constexpr size_t kSize = 8;
  static constexpr std::string_view kSchema = "int64 value";
  static int64_t Unpack(std::span<const uint8_t, 8> data) {
    return support::endian::read64le(data.data());
  }
  static void Pack(std::span<uint8_t, 8> data, int64_t value) {
    support::endian::write64le(data.data(), value);
  }
};

/**
 * Raw struct support for float values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<float> {
  static constexpr std::string_view kTypeString = "struct:float";
  static constexpr size_t kSize = 4;
  static constexpr std::string_view kSchema = "float value";
  static float Unpack(std::span<const uint8_t, 4> data) {
    return bit_cast<float>(support::endian::read32le(data.data()));
  }
  static void Pack(std::span<uint8_t, 4> data, float value) {
    support::endian::write32le(data.data(), bit_cast<uint32_t>(value));
  }
};

/**
 * Raw struct support for double values.
 * Primarily useful for higher level struct implementations.
 */
template <>
struct Struct<double> {
  static constexpr std::string_view kTypeString = "struct:double";
  static constexpr size_t kSize = 8;
  static constexpr std::string_view kSchema = "double value";
  static double Unpack(std::span<const uint8_t, 8> data) {
    return bit_cast<double>(support::endian::read64le(data.data()));
  }
  static void Pack(std::span<uint8_t, 8> data, double value) {
    support::endian::write64le(data.data(), bit_cast<uint64_t>(value));
  }
};

}  // namespace wpi
