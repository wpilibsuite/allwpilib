// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/telemetry/Telemetry.h"

#include <functional>
#include <memory>
#include <utility>

#include <wpi/sendable2/SendableTable.h>

namespace frc {

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline void Telemetry::SetStruct(std::string_view name, const T& value,
                                 I... info) {
  GetTableHolder().SetStruct(name, value, std::forward<I>(info)...);
}

template <wpi::ProtobufSerializable T>
inline void Telemetry::SetProtobuf(std::string_view name, const T& value) {
  GetTableHolder().SetProtobuf(name, value);
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline void Telemetry::PublishStruct(std::string_view name,
                                     std::function<T()> supplier, I... info) {
  GetTableHolder().PublishStruct<T, I...>(name, std::move(supplier),
                                          std::forward<I>(info)...);
}

template <wpi::ProtobufSerializable T>
inline void Telemetry::PublishProtobuf(std::string_view name,
                                       std::function<T()> supplier) {
  GetTableHolder().PublishProtobuf<T>(name, std::move(supplier));
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline std::function<void(const T&)> Telemetry::AddStructPublisher(
    std::string_view name, I... info) {
  return GetTableHolder().AddStructPublisher<T>(name, std::forward<I>(info)...);
}

template <wpi::ProtobufSerializable T>
inline std::function<void(const T&)> Telemetry::AddProtobufPublisher(
    std::string_view name) {
  return GetTableHolder().AddProtobufPublisher<T>(name);
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline void Telemetry::SubscribeStruct(std::string_view name,
                                       std::function<void(T)> consumer,
                                       I... info) {
  GetTableHolder().SubscribeStruct<T, I...>(name, std::move(consumer),
                                            std::forward<I>(info)...);
}

template <wpi::ProtobufSerializable T>
inline void Telemetry::SubscribeProtobuf(std::string_view name,
                                         std::function<void(T)> consumer) {
  GetTableHolder().SubscribeProtobuf<T>(name, std::move(consumer));
}

template <typename T, typename... I>
  requires wpi2::SendableSerializableMoveTracked<T, I...>
inline wpi2::SendableTable Telemetry::AddSendable(std::string_view name, T* obj,
                                                  I... info) {
  return GetTableHolder().AddSendable(name, obj, std::forward<I>(info)...);
}

template <typename T, typename... I>
  requires wpi2::SendableSerializableSharedPointer<T, I...>
inline wpi2::SendableTable Telemetry::AddSendable(std::string_view name,
                                                  std::shared_ptr<T> obj,
                                                  I... info) {
  return GetTableHolder().AddSendable(name, std::move(obj),
                                      std::forward<I>(info)...);
}

}  // namespace frc
