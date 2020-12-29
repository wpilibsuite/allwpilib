// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "frc2/command/Command.h"

namespace frc2 {

/**
 * CRTP implementation to allow polymorphic decorator functions in Command.
 *
 * <p>Note: ALWAYS create a subclass by extending CommandHelper<Base, Subclass>,
 * or decorators will not function!
 */
template <typename Base, typename CRTP,
          typename = std::enable_if_t<std::is_base_of_v<Command, Base>>>
class CommandHelper : public Base {
  using Base::Base;

 public:
  CommandHelper() = default;

 protected:
  std::unique_ptr<Command> TransferOwnership() && override {
    return std::make_unique<CRTP>(std::move(*static_cast<CRTP*>(this)));
  }
};
}  // namespace frc2
