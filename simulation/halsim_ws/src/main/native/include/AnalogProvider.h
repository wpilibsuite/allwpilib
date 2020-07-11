#pragma once

#include "HALProviders.h"

class AnalogInProvider : public HALChannelProvider {
  public:
    static void Initialize(WSRegisterFunc wsRegisterFunc);

    using HALChannelProvider::HALChannelProvider;

    wpi::json OnSimValueChanged(const char* name) override;
    void OnEndpointValueChanged(const wpi::json& msg) override;
};

class AnalogOutProvider : public HALChannelProvider {
  public:
    static void Initialize(WSRegisterFunc wsRegisterFunc);

    using HALChannelProvider::HALChannelProvider;

    wpi::json OnSimValueChanged(const char* name) override;
};