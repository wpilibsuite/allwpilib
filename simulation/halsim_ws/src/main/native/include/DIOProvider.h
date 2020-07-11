#pragma once

#include <memory>

#include "HALProviders.h"

class DIOProvider : public HALChannelProvider {
  public:
    static void Initialize(WSRegisterFunc wsRegisterFunc);

    using HALChannelProvider::HALChannelProvider;

    wpi::json OnSimValueChanged(const char* name) override;
    void OnEndpointValueChanged(const wpi::json& msg) override;
};