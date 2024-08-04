// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <utility>

#include "wpi/protobuf/Protobuf.h"
#include "wpi/struct/Struct.h"
#include "wpi/sendable2/SendableTable.h"
#include "wpi/MoveTracker.h"
#include "wpi/SmallVector.h"

namespace wpi2 {

class SendableWrapper {
 public:
  virtual ~SendableWrapper() = default;

  virtual std::string_view GetTypeString() const = 0;

  virtual void Init(SendableTable& table) const = 0;

  virtual bool Exists(SendableTable& table) = 0;
};

namespace detail {
template <wpi::MoveTracked T, typename... I>
class SendableWrapperMoveTracked final : public SendableWrapper {
  explicit SendableWrapperMoveTracked(T* obj, SendableTable& table, I... info)
      : m_last{obj},
        m_obj{obj},
        m_backend{table.GetWeak()},
        m_info{std::move(info)...} {}

  ~SendableWrapperMoveTracked() override {
    if (T* obj = m_obj.Get()) {
      if (auto backend = m_backend.lock()) {
        SendableTable table{std::move(backend)};
        std::apply([&](I... info) { CloseSendable(obj, table, info...); },
                   m_info);
      }
    }
  }

  std::string_view GetTypeString() const override {
    return std::apply([&](I... info) { GetSendableTypeString<T>(info...); },
                      m_info);
  }

  void Init(SendableTable& table) const override {
    if (T* obj = m_obj.Get()) {
      std::apply([&](I... info) { InitSendable(obj, table, info...); }, m_info);
    }
  }

  bool Exists(SendableTable& table) override {
    if (T* obj = m_obj.Get()) {
      // reinit if moved
      if (m_last != obj) {
        m_last = obj;
        std::apply([&](I... info) { InitSendable(obj, table, info...); },
                   m_info);
      }
      return true;
    } else {
      return false;
    }
  }

 private:
  T* m_last;
  wpi::MoveWeakPtr<T> m_obj;
  std::weak_ptr<SendableTableBackend> m_backend;
  [[no_unique_address]] std::tuple<I...> m_info;
};

template <typename T, typename... I>
class SendableWrapperSharedPtr final : public SendableWrapper {
  explicit SendableWrapperSharedPtr(std::shared_ptr<T> obj,
                                    SendableTable& table, I... info)
      : m_obj{std::move(obj)},
        m_backend{table.GetWeak()},
        m_info{std::move(info)...} {}

  ~SendableWrapperSharedPtr() override {
    if (auto backend = m_backend.lock()) {
      SendableTable table{std::move(backend)};
      std::apply([&](I... info) { CloseSendable(m_obj, table, info...); },
                 m_info);
    }
  }

  std::string_view GetTypeString() const override {
    return std::apply([&](I... info) { GetSendableTypeString<T>(info...); },
                      m_info);
  }

  void Init(SendableTable& table) const override {
    std::apply([&](I... info) { InitSendable(m_obj, table, info...); }, m_info);
  }

  bool Exists(SendableTable& table) override { return true; }

 private:
  std::shared_ptr<T> m_obj;
  std::weak_ptr<SendableTableBackend> m_backend;
  [[no_unique_address]] std::tuple<I...> m_info;
};
}  // namespace detail

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

template <wpi::ProtobufSerializable T>
void SendableTable::SetProtobuf(std::string_view name, const T& value) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  wpi::SmallVector<uint8_t, 128> buf;
  msg.Pack(buf, value);
  SetRaw(name, msg.GetTypeString(), buf);
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

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
std::function<void(const T&)> SendableTable::AddStructPublisher(
    std::string_view name, I... info) {
  using S = wpi::Struct<T, I...>;
  AddStructSchema<T>(info...);
  return [rawSetter = AddRawPublisher(name, S::GetTypeString(info...)),
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

template <wpi::ProtobufSerializable T>
[[nodiscard]]
std::function<void(const T&)> SendableTable::AddProtobufPublisher(
    std::string_view name) {
  wpi::ProtobufMessage<T> msg;
  AddProtobufSchema<T>(msg);
  auto typeString = msg.GetTypeString();
  return [rawSetter = AddRawPublisher(name, typeString),
          msg = std::move(msg)](const T& value) mutable {
    wpi::SmallVector<uint8_t, 128> buf;
    msg.Pack(buf, value);
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
