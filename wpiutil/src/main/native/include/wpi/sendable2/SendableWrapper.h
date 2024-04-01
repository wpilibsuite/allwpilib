// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <tuple>

#include "wpi/sendable2/Sendable.h"
#include "wpi/sendable2/SendableTable.h"
#include "wpi/MoveTracker.h"

namespace wpi2 {

class SendableTableBackend;

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

}  // namespace wpi2
