// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <new>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/tunable/detail/TunableTypeTraits.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi {
class ComplexTunable;
struct TunableConfig;
}  // namespace wpi

namespace wpi::detail {

template <typename T>
class TunableMemberPointer {
 public:
  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberPointer(T Class::* member) {
    static_assert(alignof(Storage<Class>) ==
                  alignof(Storage<TunableMemberPointer>));
    static_assert(sizeof(Storage<Class>) <=
                  (2 * sizeof(Storage<TunableMemberPointer>)));
    ::new (m_storage) Storage<Class>{member};
  }

  const T& Get(const ComplexTunable* obj) const {
    const StorageBase* storage =
        std::launder(reinterpret_cast<const StorageBase*>(m_storage));
    return storage->Get(obj);
  }

  T& Get(ComplexTunable* obj) const {
    const StorageBase* storage =
        std::launder(reinterpret_cast<const StorageBase*>(m_storage));
    return storage->Get(obj);
  }

 private:
  struct StorageBase {
    virtual const T& Get(const ComplexTunable* obj) const = 0;
    virtual T& Get(ComplexTunable* obj) const = 0;
  };
  template <typename Class>
  struct Storage : public StorageBase {
    explicit Storage(T Class::* member) : m_ptr{member} {}
    const T& Get(const ComplexTunable* obj) const override {
      return static_cast<const Class*>(obj)->*m_ptr;
    }
    T& Get(ComplexTunable* obj) const override {
      return static_cast<Class*>(obj)->*m_ptr;
    }
    T Class::* m_ptr;
  };

  // 2x in case of virtual base classes or similar
  alignas(Storage<TunableMemberPointer>)
      std::byte m_storage[2 * sizeof(Storage<TunableMemberPointer>)];
};

template <TunableValueType T>
class TunableMemberValueBase : public TunableMemberBase {
 public:
  TunableMemberValueBase()
      : TunableMemberBase{GetTunableTypeValue<T, true>()} {}

  explicit TunableMemberValueBase(const TunableConfig& config)
      : TunableMemberBase{GetTunableTypeValue<T, true>(), config} {}

  virtual const T& Get(const ComplexTunable* obj) const = 0;

  virtual void Set(ComplexTunable* obj, T value) = 0;
};

template <TunableValueType T>
class TunableMemberValue : public TunableMemberValueBase<T> {
 public:
  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberValue(T Class::* member) : m_ptr{member} {}

  template <std::derived_from<ComplexTunable> Class>
  TunableMemberValue(T Class::* member, const TunableConfig& config)
      : TunableMemberValueBase<T>{config}, m_ptr{member} {}

  const T& Get(const ComplexTunable* obj) const override {
    return m_ptr.Get(obj);
  }

  void Set(ComplexTunable* obj, T value) override {
    m_ptr.Get(obj) = std::move(value);
  }

 private:
  TunableMemberPointer<T> m_ptr;
};

class TunableMemberStructBase : public TunableMemberBase {
 public:
  constexpr TunableMemberStructBase()
      : TunableMemberBase{TunableTypeValue::MEMBER_STRUCT} {}
  explicit TunableMemberStructBase(const TunableConfig& config)
      : TunableMemberBase{TunableTypeValue::MEMBER_STRUCT, config} {}

  TunableMemberStructBase(const TunableMemberStructBase&) = default;
  TunableMemberStructBase& operator=(const TunableMemberStructBase&) = default;
  TunableMemberStructBase(TunableMemberStructBase&&) = default;
  TunableMemberStructBase& operator=(TunableMemberStructBase&&) = default;

  virtual std::string GetStructTypeName() const = 0;
  // note: size may be dynamic due to struct arrays
  virtual size_t GetStructSize(ComplexTunable* obj) const = 0;
  virtual void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const = 0;
  virtual void UnpackStruct(ComplexTunable* obj,
                            std::span<const uint8_t> data) = 0;
  virtual void PackStruct(ComplexTunable* obj,
                          std::span<uint8_t> buf) const = 0;

 protected:
  virtual ~TunableMemberStructBase() = default;
};

class TunableMemberProtobufBase : public TunableMemberBase {
 public:
  constexpr TunableMemberProtobufBase()
      : TunableMemberBase{TunableTypeValue::MEMBER_PROTOBUF} {}
  explicit TunableMemberProtobufBase(const TunableConfig& config)
      : TunableMemberBase{TunableTypeValue::MEMBER_PROTOBUF, config} {}

  TunableMemberProtobufBase(const TunableMemberProtobufBase&) = default;
  TunableMemberProtobufBase& operator=(const TunableMemberProtobufBase&) =
      default;
  TunableMemberProtobufBase(TunableMemberProtobufBase&&) = default;
  TunableMemberProtobufBase& operator=(TunableMemberProtobufBase&&) = default;

  virtual std::string GetProtobufTypeString() const = 0;
  virtual bool UnpackProtobuf(ComplexTunable* obj,
                              std::span<const uint8_t> data) = 0;
  virtual bool PackProtobuf(ComplexTunable* obj,
                            std::vector<uint8_t>& buf) const = 0;
  virtual void ForEachProtobufDescriptor(
      util::function_ref<bool(std::string_view filename)> exists,
      util::function_ref<void(std::string_view filename,
                              std::string_view descriptor)>
          fn) const = 0;

 protected:
  virtual ~TunableMemberProtobufBase() = default;
};

/**
 * A tunable struct-serialized value.
 *
 * @tparam T value class
 * @tparam I optional struct type info parameters
 */
template <typename T, typename... I>
  requires wpi::util::StructSerializable<T, I...>
class TunableMemberStruct : public detail::TunableMemberStructBase {
 public:
  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberStruct(T Class::* member, I... info)
      : m_ptr{member}, m_info{std::move(info)...} {}

  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberStruct(T Class::* member, const TunableConfig& config,
                               I... info)
      : detail::TunableMemberStructBase{config},
        m_ptr{member},
        m_info{std::move(info)...} {}

  const T& Get(ComplexTunable* obj) const { return m_ptr.Get(obj); }

  void Set(ComplexTunable* obj, T value) { m_ptr.Get(obj) = std::move(value); }

  T& Mutate(ComplexTunable* obj) { return m_ptr.Get(obj); }

 protected:
  std::string GetStructTypeName() const override {
    return std::apply(
        [](const I&... info) {
          return std::string{wpi::util::GetStructTypeName<T>(info...)};
        },
        m_info);
  }

  size_t GetStructSize(ComplexTunable*) const override {
    return std::apply(
        [](const I&... info) { return wpi::util::GetStructSize<T>(info...); },
        m_info);
  }

  void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const override {
    std::apply(
        [&](const I&... info) {
          wpi::util::ForEachStructSchema<T>(fn, info...);
        },
        m_info);
  }

  void UnpackStruct(ComplexTunable* obj,
                    std::span<const uint8_t> data) override {
    std::apply(
        [&](const I&... info) {
          wpi::util::UnpackStructInto(&m_ptr.Get(obj), data, info...);
        },
        m_info);
  }

  void PackStruct(ComplexTunable* obj, std::span<uint8_t> buf) const override {
    std::apply(
        [&](const I&... info) {
          wpi::util::PackStruct(buf, m_ptr.Get(obj), info...);
        },
        m_info);
  }

 private:
  detail::TunableMemberPointer<T> m_ptr;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

template <typename T, typename... I>
  requires wpi::util::StructSerializable<T, I...>
class TunableMemberStructVector : public detail::TunableMemberStructBase {
 public:
  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberStructVector(T Class::* member, I... info)
      : m_ptr{member}, m_info{std::move(info)...} {}

  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberStructVector(T Class::* member,
                                     const TunableConfig& config, I... info)
      : detail::TunableMemberStructBase{config},
        m_ptr{member},
        m_info{std::move(info)...} {}

  const std::vector<T>& Get(ComplexTunable* obj) const {
    return m_ptr.Get(obj);
  }

  void Set(ComplexTunable* obj, std::vector<T> value) {
    m_ptr.Get(obj) = std::move(value);
  }
  void Set(ComplexTunable* obj, std::span<const T> value) {
    m_ptr.Get(obj).assign(value.begin(), value.end());
  }

  std::vector<T>& Mutate(ComplexTunable* obj) { return m_ptr.Get(obj); }

 protected:
  std::string GetStructTypeName() const override {
    return std::apply(
        [](const I&... info) {
          return std::string{
              wpi::util::MakeStructArrayTypeString<T, std::dynamic_extent>(
                  info...)};
        },
        m_info);
  }

  size_t GetStructSize(ComplexTunable* obj) const override {
    return m_ptr.Get(obj).size() *
           std::apply(
               [](const I&... info) {
                 return wpi::util::GetStructSize<T>(info...);
               },
               m_info);
  }

  void ForEachStructSchema(
      wpi::util::function_ref<void(std::string_view typeName,
                                   std::string_view schema)>
          fn) const override {
    std::apply(
        [&](const I&... info) {
          wpi::util::ForEachStructSchema<T>(fn, info...);
        },
        m_info);
  }

  void UnpackStruct(ComplexTunable* obj,
                    std::span<const uint8_t> data) override {
    auto& value = m_ptr.Get(obj);
    std::apply(
        [&](const I&... info) {
          size_t size = wpi::util::GetStructSize<T>(info...);
          value.clear();
          value.reserve(data.size() / size);
          for (size_t pos = 0, end = data.size(); pos < end; pos += size) {
            value.emplace_back(
                wpi::util::UnpackStruct<T>(data.subspan(pos, size), info...));
          }
        },
        m_info);
  }

  void PackStruct(ComplexTunable* obj, std::span<uint8_t> buf) const override {
    std::apply(
        [&](const I&... info) {
          size_t size = wpi::util::GetStructSize<T>(info...);
          for (auto&& val : m_ptr.Get(obj)) {
            if (buf.size() < size) {
              break;  // shouldn't happen, but avoid clobbering memory
            }
            wpi::util::PackStruct(buf, val, info...);
            buf = buf.subspan(size);
          }
        },
        m_info);
  }

 private:
  detail::TunableMemberPointer<std::vector<T>> m_ptr;
  [[no_unique_address]]
  std::tuple<I...> m_info;
};

/**
 * A tunable protobuf-serialized value.
 *
 * @tparam T value class
 */
template <wpi::util::ProtobufSerializable T>
class TunableMemberProtobuf : public detail::TunableMemberProtobufBase {
 public:
  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberProtobuf(T Class::* member) : m_ptr{member} {}

  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberProtobuf(T Class::* member, const TunableConfig& config)
      : detail::TunableMemberProtobufBase{config}, m_ptr{member} {}

  const T& Get(ComplexTunable* obj) const { return m_ptr.Get(obj); }

  void Set(ComplexTunable* obj, T value) { m_ptr.Get(obj) = std::move(value); }

  T& Mutate(ComplexTunable* obj) { return m_ptr.Get(obj); }

 protected:
  std::string GetProtobufTypeString() const override {
    return m_message.GetTypeString();
  }

  bool UnpackProtobuf(ComplexTunable* obj,
                      std::span<const uint8_t> data) override {
    return m_message.UnpackInto(&m_ptr.Get(obj), data);
  }

  bool PackProtobuf(ComplexTunable* obj,
                    std::vector<uint8_t>& buf) const override {
    return m_message.Pack(buf, m_ptr.Get(obj));
  }

  void ForEachProtobufDescriptor(
      wpi::util::function_ref<bool(std::string_view filename)> exists,
      wpi::util::function_ref<void(std::string_view filename,
                                   std::string_view descriptor)>
          fn) const override {
    m_message.ForEachProtobufDescriptor(
        exists,
        [&](std::string_view filename, std::span<const uint8_t> descriptor) {
          auto data = reinterpret_cast<const char*>(descriptor.data());
          fn(filename, {data, descriptor.size()});
        });
  }

 private:
  detail::TunableMemberPointer<T> m_ptr;
  [[no_unique_address]]
  mutable wpi::util::ProtobufMessage<T> m_message;
};

}  // namespace wpi::detail
