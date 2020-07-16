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
                     HALCbRegisterIndexedFunc halRegisterFunc,
                     WSRegisterFunc webRegisterFunc) {
  for (int32_t i = 0; i < numChannels; i++) {
    auto key = (prefix + "/" + wpi::Twine(i)).str();
    auto ptr = std::make_unique<T>(i, key);
    halRegisterFunc(i, HALSimWSHalProvider::OnStaticSimCallback, ptr.get(),
                    true);
    webRegisterFunc(key, std::move(ptr));
  }
}

template <typename T>
void CreateSingleProvider(const std::string& key,
                          HALCbRegisterSingleFunc halRegisterFunc,
                          WSRegisterFunc webRegisterFunc) {
  auto ptr = std::make_unique<T>(key);
  halRegisterFunc(HALSimWSHalProvider::OnStaticSimCallback, ptr.get(), true);
  webRegisterFunc(key, std::move(ptr));
}
