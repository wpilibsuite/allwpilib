#include "HALProviders.h"

#include <iostream>

void HALProvider::OnNetworkConnected(std::shared_ptr<HALSimWebsocketConnection> ws) {
  {
    std::lock_guard lock(m_mutex);

    m_ws = ws;
  }

  OnSimCallback("");
}

void HALProvider::OnStaticSimCallback(const char* name, void* param, const struct HAL_Value* value) {
  static_cast<HALProvider*>(param)->OnSimCallback(name);
}

void HALProvider::OnSimCallback(const char* name) {
  std::lock_guard lock(m_mutex);

  auto result = OnSimValueChanged(name);

  auto ws = m_ws.lock();
  if (ws) {
    // Just send the raw JSON
    ws->OnSimValueChanged(result);
  }
}

HALChannelProvider::HALChannelProvider(int32_t channel, const std::string& key)
  : HALProvider(key), m_channel(channel) {}