// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include <wpi/expected>

namespace glass::expression {

template <typename V>
wpi::expected<V, std::string> TryParseExpr(const char* expr);

extern template wpi::expected<double, std::string> TryParseExpr(const char*);
extern template wpi::expected<float, std::string> TryParseExpr(const char*);
extern template wpi::expected<int64_t, std::string> TryParseExpr(const char*);

}  // namespace glass::expression
