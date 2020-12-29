// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <utility>

template <typename T>
void CreateProviders(const std::string& prefix, int numChannels,
                     WSRegisterFunc webRegisterFunc) {
  for (int32_t i = 0; i < numChannels; i++) {
    auto key = (prefix + "/" + wpi::Twine(i)).str();
    auto ptr = std::make_unique<T>(i, key, prefix);
    webRegisterFunc(key, std::move(ptr));
  }
}

template <typename T>
void CreateSingleProvider(const std::string& key,
                          WSRegisterFunc webRegisterFunc) {
  auto ptr = std::make_unique<T>(key, key);
  webRegisterFunc(key, std::move(ptr));
}
