/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
