// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <format>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi::tunables {

/** A mock tunable backend for testing. */
class MockTunableBackend : public TunableBackend {
 public:
  /** Destroys this mock tunable backend. */
  ~MockTunableBackend() override;

  /**
   * Gets the value of a tunable.
   *
   * @tparam T value type
   * @param path normalized path
   * @return value
   */
  template <detail::TunableValueType T>
  T GetValue(std::string_view path) const;

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  bool GetBool(std::string_view path) const { return GetValue<bool>(path); }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  int32_t GetInt32(std::string_view path) const {
    return GetValue<int32_t>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  int64_t GetInt64(std::string_view path) const {
    return GetValue<int64_t>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  float GetFloat(std::string_view path) const { return GetValue<float>(path); }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  double GetDouble(std::string_view path) const {
    return GetValue<double>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::string GetString(std::string_view path) const {
    return GetValue<std::string>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<uint8_t> GetRaw(std::string_view path) const {
    return GetValue<std::vector<uint8_t>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<bool> GetBoolVector(std::string_view path) const {
    return GetValue<std::vector<bool>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<int32_t> GetInt32Vector(std::string_view path) const {
    return GetValue<std::vector<int32_t>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<int64_t> GetInt64Vector(std::string_view path) const {
    return GetValue<std::vector<int64_t>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<float> GetFloatVector(std::string_view path) const {
    return GetValue<std::vector<float>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<double> GetDoubleVector(std::string_view path) const {
    return GetValue<std::vector<double>>(path);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  std::vector<std::string> GetStringVector(std::string_view path) const {
    return GetValue<std::vector<std::string>>(path);
  }

  /**
   * Gets the type name of a struct-serialized tunable.
   *
   * @param path normalized path
   * @return struct type name
   */
  std::string GetStructTypeName(std::string_view path) const;

  /**
   * Gets the serialized bytes of a struct-serialized tunable.
   *
   * @param path normalized path
   * @return serialized bytes
   */
  std::vector<uint8_t> GetStructData(std::string_view path) const;

  /**
   * Gets the value of a struct-serialized tunable.
   *
   * @tparam T value type
   * @tparam I optional info parameters for the struct serializer
   * @param path normalized path
   * @param info optional info parameters for the struct serializer
   * @return value
   */
  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  T GetStruct(std::string_view path, I... info) const {
    std::string expected{wpi::util::GetStructTypeName<T>(info...)};
    std::string actual = GetStructTypeName(path);
    if (actual != expected) {
      throw std::invalid_argument{std::format(
          "Struct tunable at {} is a {}, not a {}", path, actual, expected)};
    }
    return wpi::util::UnpackStruct<T>(GetStructData(path), info...);
  }

  /**
   * Gets the value of a struct-serialized array tunable.
   *
   * @tparam T value type
   * @tparam I optional info parameters for the struct serializer
   * @param path normalized path
   * @param info optional info parameters for the struct serializer
   * @return value
   */
  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  std::vector<T> GetStructVector(std::string_view path, I... info) const {
    std::string expected{
        wpi::util::MakeStructArrayTypeName<T, std::dynamic_extent>(info...)};
    std::string actual = GetStructTypeName(path);
    if (actual != expected) {
      throw std::invalid_argument{std::format(
          "Struct tunable at {} is a {}, not a {}", path, actual, expected)};
    }

    std::vector<uint8_t> data = GetStructData(path);
    size_t structSize = wpi::util::GetStructSize<T>(info...);
    if (structSize == 0 || data.size() % structSize != 0) {
      throw std::runtime_error{
          std::format("Struct array tunable has invalid size: {}", path)};
    }

    std::vector<T> values;
    values.reserve(data.size() / structSize);
    for (size_t pos = 0, end = data.size(); pos < end; pos += structSize) {
      values.emplace_back(wpi::util::UnpackStruct<T>(
          std::span{data}.subspan(pos, structSize), info...));
    }
    return values;
  }

  /**
   * Gets the type string of a protobuf-serialized tunable.
   *
   * @param path normalized path
   * @return protobuf type string
   */
  std::string GetProtobufTypeString(std::string_view path) const;

  /**
   * Gets the serialized bytes of a protobuf-serialized tunable.
   *
   * @param path normalized path
   * @return serialized bytes
   */
  std::vector<uint8_t> GetProtobufData(std::string_view path) const;

  /**
   * Gets the value of a protobuf-serialized tunable.
   *
   * @tparam T value type
   * @param path normalized path
   * @return value
   */
  template <wpi::util::ProtobufSerializable T>
  T GetProtobuf(std::string_view path) const {
    wpi::util::ProtobufMessage<T> msg;
    std::string expected = msg.GetTypeString();
    std::string actual = GetProtobufTypeString(path);
    if (actual != expected) {
      throw std::invalid_argument{std::format(
          "Protobuf tunable at {} is a {}, not a {}", path, actual, expected)};
    }

    auto value = msg.Unpack(GetProtobufData(path));
    if (!value) {
      throw std::runtime_error{
          std::format("Failed to unpack protobuf tunable: {}", path)};
    }
    return *value;
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetBool(std::string_view path, bool value) {
    SetValue<bool>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetInt32(std::string_view path, int32_t value) {
    SetValue<int32_t>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetInt64(std::string_view path, int64_t value) {
    SetValue<int64_t>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetFloat(std::string_view path, float value) {
    SetValue<float>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetDouble(std::string_view path, double value) {
    SetValue<double>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetString(std::string_view path, std::string_view value) {
    SetValue<std::string>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetRaw(std::string_view path, std::span<const uint8_t> value) {
    SetValue<std::vector<uint8_t>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetBoolVector(std::string_view path, std::span<const bool> value) {
    SetValue<std::vector<bool>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetBoolVector(std::string_view path, std::vector<bool> value) {
    SetValue<std::vector<bool>>(path, std::move(value));
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetInt32Vector(std::string_view path, std::span<const int32_t> value) {
    SetValue<std::vector<int32_t>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetInt64Vector(std::string_view path, std::span<const int64_t> value) {
    SetValue<std::vector<int64_t>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetFloatVector(std::string_view path, std::span<const float> value) {
    SetValue<std::vector<float>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetDoubleVector(std::string_view path, std::span<const double> value) {
    SetValue<std::vector<double>>(path, value);
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  void SetStringVector(std::string_view path,
                       std::span<const std::string> value) {
    SetValue<std::vector<std::string>>(path, value);
  }

  /**
   * Sets the value of a struct-serialized tunable.
   *
   * @tparam T value type
   * @tparam I optional info parameters for the struct serializer
   * @param path normalized path
   * @param value value
   * @param info optional info parameters for the struct serializer
   */
  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  void SetStruct(std::string_view path, const T& value, I... info) {
    std::vector<uint8_t> buf(wpi::util::GetStructSize<T>(info...));
    wpi::util::PackStruct(buf, value, info...);
    SetStructData(path, wpi::util::GetStructTypeName<T>(info...), buf);
  }

  /**
   * Sets the value of a struct-serialized array tunable.
   *
   * @tparam T value type
   * @tparam I optional info parameters for the struct serializer
   * @param path normalized path
   * @param value value
   * @param info optional info parameters for the struct serializer
   */
  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  void SetStructVector(std::string_view path, std::span<const T> value,
                       I... info) {
    size_t structSize = wpi::util::GetStructSize<T>(info...);
    std::vector<uint8_t> buf(structSize * value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      wpi::util::PackStruct(
          std::span<uint8_t>{buf}.subspan(i * structSize, structSize), value[i],
          info...);
    }
    SetStructData(path, wpi::util::GetStructTypeName<T>(info...), buf);
  }

  /**
   * Sets the value of a protobuf-serialized tunable.
   *
   * @tparam T value type
   * @param path normalized path
   * @param value value
   */
  template <wpi::util::ProtobufSerializable T>
  void SetProtobuf(std::string_view path, const T& value) {
    std::vector<uint8_t> buf;
    util::ProtobufMessage<T> msg;
    msg.Pack(buf, value);
    SetProtobufData(path, msg.GetTypeString(), buf);
  }

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param uid the tunable uid
   * @param tunable the tunable
   * @param config the tunable config
   * @param type the tunable type
   * @return true if the tunable was published, false otherwise
   */
  bool Publish(std::string_view path, uint32_t uid,
               detail::TunableBase& tunable, const TunableConfig* config,
               detail::TunableTypeValue type) override;

  /**
   * Marks a published tunable dirty after its local value changed.
   *
   * Mock tunables read their value directly during getters, so this is a no-op.
   *
   * @param uid tunable uid
   */
  void MarkDirty(uint32_t uid) override;

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  void Remove(std::string_view path) override;

  /**
   * Removes all tunables under a prefix.
   *
   * @param prefix normalized path prefix
   * @return removed tunables
   */
  std::vector<PublishedTunable> RemovePrefix(std::string_view prefix) override;

  /**
   * Unregisters a tunable.
   *
   * @param uid tunable uid
   */
  void UnregisterTunable(uint32_t uid) override;

  /**
   * Gets the uid published at a path.
   *
   * @param path normalized path
   * @return tunable uid, or empty if no tunable exists at the path
   */
  std::optional<uint32_t> GetUid(std::string_view path) const;

  /** Updates all tunable values and calls callbacks where appropriate. */
  void Update() override;

 private:
  uint32_t GetUidOrThrow(std::string_view path) const;

  template <typename T, typename U>
  void SetValue(std::string_view path, U value);
  void SetStructData(std::string_view path, std::string_view typeString,
                     std::span<const uint8_t> data);
  void SetProtobufData(std::string_view path, std::string_view typeString,
                       std::span<const uint8_t> data);

  mutable wpi::util::mutex m_mutex;
  wpi::util::StringMap<uint32_t> m_tunables;
  wpi::util::DenseMap<uint32_t, std::vector<std::string>> m_uids;
  struct Action {
    std::string path;
    uint32_t uid;
    std::function<void(uint32_t uid)> update;
  };
  std::vector<Action> m_actions;
};

}  // namespace wpi::tunables
