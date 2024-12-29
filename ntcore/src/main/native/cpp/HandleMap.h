// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <utility>

#include <wpi/UidVector.h>

#include "Handle.h"

namespace nt {

template <typename T>
concept HandleType = requires {
  { T::kType } -> std::convertible_to<NT_Handle>;
};

// Utility wrapper class for our UidVectors
template <HandleType T, size_t Size>
class HandleMap : public wpi::UidVector<std::unique_ptr<T>, Size> {
 public:
  template <typename... Args>
  T* Add(int inst, Args&&... args) {
    auto i = this->emplace_back();
    auto& it = (*this)[i];
    it = std::make_unique<T>(Handle(inst, i, T::kType),
                             std::forward<Args>(args)...);
    return it.get();
  }

  std::unique_ptr<T> Remove(NT_Handle handle) {
    Handle h{handle};
    if (!h.IsType(T::kType)) {
      return {};
    }
    unsigned int i = h.GetIndex();
    if (i >= this->size()) {
      return {};
    }
    return this->erase(i);
  }

  T* Get(NT_Handle handle) {
    Handle h{handle};
    if (!h.IsType(T::kType)) {
      return {};
    }
    unsigned int i = h.GetIndex();
    if (i >= this->size()) {
      return {};
    }
    return (*this)[i].get();
  }
};

}  // namespace nt
