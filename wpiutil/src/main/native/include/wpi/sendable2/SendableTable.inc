// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <vector>

#include "wpi/protobuf/Protobuf.h"
#include "wpi/struct/Struct.h"
#include "wpi/sendable2/SendableTable.h"
#include "wpi/sendable2/SendableWrapper.h"
#include "wpi/SmallVector.h"

namespace wpi2 {

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
void SendableTable::SetStruct(std::string_view name, const T& value,
                              I... info) {
  using S = wpi::Struct<T, I...>;
  AddStructSchema<T>(info...);
  if constexpr (sizeof...(I) == 0) {
    if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
      uint8_t buf[S::GetSize()];
      S::Pack(buf, value);
      SetRaw(name, S::GetTypeString(), buf);
      return;
    }
  }
  wpi::SmallVector<uint8_t, 128> buf;
  buf.resize_for_overwrite(S::GetSize(info...));
  S::Pack(buf, value, info...);
  SetRaw(name, S::GetTypeString(info...), buf);
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
void SendableTable::PublishStruct(std::string_view name,
                                  std::function<T()> supplier, I... info) {
  using S = wpi::Struct<T, I...>;
  AddStructSchema<T>(info...);
  PublishRawSmall(
      name, S::GetTypeString(info...),
      [supplier = std::move(supplier), info = std::tuple<I...>{info...}](
          wpi::SmallVectorImpl<uint8_t>& buf) -> std::span<uint8_t> {
        std::apply(
            [&](const I&... info) {
              buf.resize_for_overwrite(S::GetSize(info...));
              S::Pack(buf, supplier(), info...);
            },
            info);
        return buf;
      });
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
std::function<void(const T&)> SendableTable::PublishStruct(
    std::string_view name, I... info) {
  using S = wpi::Struct<T, I...>;
  AddStructSchema<T>(info...);
  return [rawSetter = PublishRaw(name, S::GetTypeString(info...)),
          info = std::tuple<I...>{info...}](const T& value) {
    if constexpr (sizeof...(I) == 0) {
      if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
        uint8_t buf[S::GetSize()];
        S::Pack(buf, value);
        rawSetter(buf);
        return;
      }
    }
    wpi::SmallVector<uint8_t, 128> buf;
    std::apply(
        [&](const I&... info) {
          buf.resize_for_overwrite(S::GetSize(info...));
          S::Pack(buf, value, info...);
        },
        info);
    rawSetter(buf);
  };
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
void SendableTable::SubscribeStruct(std::string_view name,
                                    std::function<void(T)> consumer,
                                    I... info) {
  using S = wpi::Struct<T, I...>;
  AddStructSchema<T>(info...);
  SubscribeRaw(
      name, S::GetTypeString(info...),
      [consumer = std::move(consumer),
       info = std::tuple<I...>{info...}](std::span<const uint8_t> data) {
        if (data.size() >= std::apply(S::GetSize, info)) {
          consumer(std::apply(
              [&](const I&... info) { return S::Unpack(data, info...); },
              info));
        }
      });
}

template <wpi::ProtobufSerializable T>
void SendableTable::SetProtobuf(std::string_view name, const T& value) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  wpi::SmallVector<uint8_t, 128> buf;
  msg.Pack(buf, value);
  SetRaw(name, msg.GetTypeString(), buf);
}

template <wpi::ProtobufSerializable T>
void SendableTable::PublishProtobuf(std::string_view name,
                                    std::function<T()> supplier) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  auto typeString = msg.GetTypeString();
  PublishRawSmall(
      name, typeString,
      [supplier = std::move(supplier), msg = std::move(msg)](
          wpi::SmallVectorImpl<uint8_t>& buf) mutable -> std::span<uint8_t> {
        msg.Pack(buf, supplier());
        return buf;
      });
}

template <wpi::ProtobufSerializable T>
[[nodiscard]]
std::function<void(const T&)> SendableTable::PublishProtobuf(
    std::string_view name) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  auto typeString = msg.GetTypeString();
  return [rawSetter = PublishRaw(name, typeString),
          msg = std::move(msg)](const T& value) mutable {
    wpi::SmallVector<uint8_t, 128> buf;
    msg.Pack(buf, value);
    rawSetter(buf);
  };
}

template <wpi::ProtobufSerializable T>
void SendableTable::SubscribeProtobuf(std::string_view name,
                                      std::function<void(T)> consumer) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  auto typeString = msg.GetTypeString();
  SubscribeRaw(name, typeString,
               [consumer = std::move(consumer),
                msg = std::move(msg)](std::span<const uint8_t> data) mutable {
                 if (auto val = msg.Unpack(data)) {
                   consumer(*val);
                 }
               });
}

template <typename T, typename... I>
  requires SendableSerializableMoveTracked<T, I...>
inline SendableTable SendableTable::AddSendable(std::string_view name, T* obj,
                                                I... info) {
  return CreateSendable(
      name, std::make_unique<detail::SendableWrapperMoveTracked<T, I...>>(
                obj, *this, std::move(info)...));
}

template <typename T, typename... I>
  requires SendableSerializableSharedPointer<T, I...>
inline SendableTable SendableTable::AddSendable(std::string_view name,
                                                std::shared_ptr<T> obj,
                                                I... info) {
  return CreateSendable(
      name, std::make_unique<detail::SendableWrapperSharedPtr<T, I...>>(
                std::move(obj), *this, std::move(info)...));
}

// Suppress unused-lambda-capture warning on AddSchema() call
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"
#endif

template <wpi::ProtobufSerializable T>
void SendableTable::AddProtobufSchema(wpi::ProtobufMessage<T>& msg) {
  msg.ForEachProtobufDescriptor(
      [this](auto typeString) { return HasSchema(typeString); },
      [this](auto typeString, auto schema) {
        AddSchema(typeString, "proto:FileDescriptorProto", schema);
      });
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
void SendableTable::AddStructSchema(const I&... info) {
  wpi::ForEachStructSchema<T>(
      [this](auto typeString, auto schema) {
        AddSchema(typeString, "structschema", schema);
      },
      info...);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

}  // namespace wpi2
