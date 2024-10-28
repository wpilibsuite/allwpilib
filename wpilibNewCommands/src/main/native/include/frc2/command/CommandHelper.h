// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <utility>

#include "frc2/command/Command.h"
#include "frc2/command/CommandPtr.h"

namespace frc2 {

/**
 * CRTP implementation to allow polymorphic decorator functions in Command.
 *
 * <p>Note: ALWAYS create a subclass by extending CommandHelper<Base, Subclass>,
 * or decorators will not function!
 *
 * This class is provided by the NewCommands VendorDep
 */
template <std::derived_from<Command> Base, typename CRTP>
class CommandHelper : public Base {
  using Base::Base;

 public:
  CommandHelper() = default;

  CommandPtr ToPtr() && override {
    return CommandPtr(
        std::make_unique<CRTP>(std::move(*static_cast<CRTP*>(this))));
  }
};
}  // namespace frc2
