// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

#include "wpi/sendable2/SendableTable.h"

namespace wpi2 {

class SendableSet final {
 public:
  std::vector<SendableTable> GetAll() const;

  template <std::invocable<SendableTable> F>
  void ForEach(F&& func) const {
    for (auto&& backendWeak : m_tables) {
      if (auto backend = backendWeak.lock()) {
        std::invoke(std::forward<F>(func), SendableTable{std::move(backend)});
      }
    }
  }

  void Add(const SendableTable& table);

  void Remove(const SendableTable& table);

 private:
  std::vector<std::weak_ptr<SendableTableBackend>> m_tables;
};

}  // namespace wpi2
