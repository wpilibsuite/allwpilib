#pragma once

#include "HALProviders.h"

class PWMProvider : public HALChannelProvider {
  public:
    static void Initialize(WSRegisterFunc wsRegisterFunc);

    using HALChannelProvider::HALChannelProvider;

    wpi::json OnSimValueChanged(const char* name) override;
};