// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <expected>
#include <string>

namespace wpi::glass::expression {

template <typename V>
std::expected<V, std::string> TryParseExpr(const char* expr);

extern template std::expected<double, std::string> TryParseExpr(const char*);
extern template std::expected<float, std::string> TryParseExpr(const char*);
extern template std::expected<int64_t, std::string> TryParseExpr(const char*);

}  // namespace wpi::glass::expression
