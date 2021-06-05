// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
template <class T>
struct NullDeleter {
  void operator()(T*) const noexcept {};
};

}  // namespace wpi
