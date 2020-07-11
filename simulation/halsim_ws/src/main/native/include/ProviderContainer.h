#pragma once

#include <functional>
#include <shared_mutex>

#include <wpi/StringMap.h>

#include "BaseProvider.h"

class ProviderContainer {
  public:
    using ProviderPtr = std::shared_ptr<BaseProvider>;
    using IterFn = std::function<void(ProviderPtr)>;

    ProviderContainer() {}

    ProviderContainer(const ProviderContainer&) = delete;
    ProviderContainer& operator=(ProviderContainer&) = delete;

    void Add(wpi::StringRef key, std::shared_ptr<BaseProvider> provider) {
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
    wpi::StringMap<std::shared_ptr<BaseProvider>> m_providers;
};