// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

namespace wpi::fields {

struct Field {
  const char* name;
  std::string_view (*getJson)();
  std::string_view (*getImage)();
};

std::span<const Field> GetFields();

}  // namespace wpi::fields
