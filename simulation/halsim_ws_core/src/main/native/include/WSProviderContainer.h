/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

#include <wpi/StringMap.h>

#include "WSBaseProvider.h"

namespace wpilibws {

class ProviderContainer {
 public:
  using ProviderPtr = std::shared_ptr<HALSimWSBaseProvider>;
  using IterFn = std::function<void(ProviderPtr)>;

  ProviderContainer() {}

  ProviderContainer(const ProviderContainer&) = delete;
  ProviderContainer& operator=(const ProviderContainer&) = delete;

  void Add(wpi::StringRef key, std::shared_ptr<HALSimWSBaseProvider> provider) {
    std::unique_lock lock(m_mutex);
    m_providers[key] = provider;
  }

  void Delete(wpi::StringRef key) {
    std::unique_lock lock(m_mutex);
    m_providers.erase(key);
  }

  void ForEach(IterFn fn) {
    std::shared_lock lock(m_mutex);
    for (auto& kv : m_providers) {
      fn(kv.getValue());
    }
  }

  ProviderPtr Get(wpi::StringRef key) {
    std::shared_lock lock(m_mutex);
    auto fiter = m_providers.find(key);
    return fiter != m_providers.end() ? fiter->second : ProviderPtr();
  }

 private:
  std::shared_mutex m_mutex;
  wpi::StringMap<std::shared_ptr<HALSimWSBaseProvider>> m_providers;
};

}  // namespace wpilibws
