// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string_view>

#include <wpi/StringMap.h>

#include "WSBaseProvider.h"

namespace wpilibws {

class ProviderContainer {
 public:
  using ProviderPtr = std::shared_ptr<HALSimWSBaseProvider>;
  using IterFn = std::function<void(ProviderPtr)>;

  ProviderContainer() = default;

  ProviderContainer(const ProviderContainer&) = delete;
  ProviderContainer& operator=(const ProviderContainer&) = delete;

  void Add(std::string_view key,
           std::shared_ptr<HALSimWSBaseProvider> provider) {
    std::unique_lock lock(m_mutex);
    m_providers[key] = provider;
  }

  void Delete(std::string_view key) {
    std::unique_lock lock(m_mutex);
    m_providers.erase(key);
  }

  void ForEach(IterFn fn) {
    std::shared_lock lock(m_mutex);
    for (auto& kv : m_providers) {
      fn(kv.second);
    }
  }

  ProviderPtr Get(std::string_view key) {
    std::shared_lock lock(m_mutex);
    auto fiter = m_providers.find(key);
    return fiter != m_providers.end() ? fiter->second : ProviderPtr();
  }

 private:
  std::shared_mutex m_mutex;
  wpi::StringMap<std::shared_ptr<HALSimWSBaseProvider>> m_providers;
};

}  // namespace wpilibws
