// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableBackend.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi {

class MockTunableBackend : public TunableBackend {
 public:
  ~MockTunableBackend() override;

  void SetBool(std::string_view path, bool value) {
    SetValue<bool>(path, value);
  }

  void SetInt32(std::string_view path, int32_t value) {
    SetValue<int32_t>(path, value);
  }

  void SetInt64(std::string_view path, int64_t value) {
    SetValue<int64_t>(path, value);
  }

  void SetFloat(std::string_view path, float value) {
    SetValue<float>(path, value);
  }

  void SetDouble(std::string_view path, double value) {
    SetValue<double>(path, value);
  }

  void SetString(std::string_view path, std::string_view value) {
    SetValue<std::string>(path, value);
  }

  void SetRaw(std::string_view path, std::span<const uint8_t> value) {
    SetValue<std::vector<uint8_t>>(path, value);
  }

  void SetBoolVector(std::string_view path, std::span<const bool> value) {
    SetValue<std::vector<bool>>(path, value);
  }

  void SetBoolVector(std::string_view path, std::vector<bool> value) {
    SetValue<std::vector<bool>>(path, std::move(value));
  }

  void SetInt32Vector(std::string_view path, std::span<const int32_t> value) {
    SetValue<std::vector<int32_t>>(path, value);
  }

  void SetInt64Vector(std::string_view path, std::span<const int64_t> value) {
    SetValue<std::vector<int64_t>>(path, value);
  }

  void SetFloatVector(std::string_view path, std::span<const float> value) {
    SetValue<std::vector<float>>(path, value);
  }

  void SetDoubleVector(std::string_view path, std::span<const double> value) {
    SetValue<std::vector<double>>(path, value);
  }

  void SetStringVector(std::string_view path,
                       std::span<const std::string> value) {
    SetValue<std::vector<std::string>>(path, value);
  }

  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  void SetStruct(std::string_view path, const T& value, I... info) {
    using S = wpi::util::Struct<T, I...>;
    std::vector<uint8_t> buf(S::GetSize());
    wpi::util::PackStruct(buf, value, info...);
    SetStructData(path, S::GetTypeName(), buf);
  }

  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  void SetStructVector(std::string_view path, std::span<const T> value,
                       I... info) {
    using S = wpi::util::Struct<T, I...>;
    size_t structSize = S::GetSize();
    std::vector<uint8_t> buf(structSize * value.size());
    for (size_t i = 0; i < value.size(); ++i) {
      wpi::util::PackStruct(
          std::span<uint8_t>{buf}.subspan(i * structSize, structSize), value[i],
          info...);
    }
    SetStructData(path, S::GetTypeName(), buf);
  }

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
   */
  void Publish(std::string_view path, uint32_t uid,
               detail::TunableBase& tunable, const TunableConfig* config,
               detail::TunableTypeValue type) override;

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  void Remove(std::string_view path) override;

  std::vector<PublishedTunable> RemovePrefix(std::string_view prefix) override;

  /**
   * Unregisters a tunable.
   *
   * @param uid tunable uid
   */
  void UnregisterTunable(uint32_t uid) override;

  /** Updates all tunable values and calls callbacks where appropriate. */
  void Update() override;

 private:
  template <typename T, typename U>
  void SetValue(std::string_view path, U value);
  void SetStructData(std::string_view path, std::string_view typeString,
                     std::span<const uint8_t> data);
  void SetProtobufData(std::string_view path, std::string_view typeString,
                       std::span<const uint8_t> data);

  wpi::util::mutex m_mutex;
  wpi::util::StringMap<uint32_t> m_tunables;
  wpi::util::DenseMap<uint32_t, std::vector<std::string>> m_uids;
  struct Action {
    uint32_t uid;
    std::function<void(uint32_t uid)> update;
  };
  std::vector<Action> m_actions;
};

}  // namespace wpi
