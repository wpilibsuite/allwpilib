// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include "WSHalProviders.h"

namespace wpilibws {

template <typename T>
void CreateProviders(std::string_view prefix, int numChannels,
                     WSRegisterFunc webRegisterFunc) {
  for (int32_t i = 0; i < numChannels; i++) {
    auto key = fmt::format("{}/{}", prefix, i);
    auto ptr = std::make_unique<T>(i, key, prefix);
    webRegisterFunc(key, std::move(ptr));
  }
}

template <typename T>
void CreateSingleProvider(std::string_view key,
                          WSRegisterFunc webRegisterFunc) {
  auto ptr = std::make_unique<T>(key, key);
  webRegisterFunc(key, std::move(ptr));
}

}  // namespace wpilibws
