#include "BaseProvider.h"
#include "HALSimWS.h"

BaseProvider::BaseProvider(const std::string& key)
  : m_key(key) {}

void BaseProvider::OnEndpointValueChanged(const wpi::json& msg) {
  // no-op
}