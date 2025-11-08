// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <fmt/format.h>
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/string.h"

#include "SystemServerInternal.h"

namespace {
struct SystemServerUsageReporting {
  nt::NetworkTableInstance ntInst;
  wpi::StringMap<nt::StringPublisher> publishers;

  explicit SystemServerUsageReporting(nt::NetworkTableInstance inst)
      : ntInst{inst} {}
};

}  // namespace

static ::SystemServerUsageReporting* systemServerUsage;

extern "C" {

int32_t HAL_ReportUsage(const struct WPI_String* resource,
                        const struct WPI_String* data) {
  auto resourceStr = wpi::to_string_view(resource);
  auto& publisher = systemServerUsage->publishers[resourceStr];
  if (!publisher) {
    publisher =
        systemServerUsage->ntInst
            .GetStringTopic(fmt::format("/UsageReporting/{}", resourceStr))
            .Publish();
  }
  publisher.Set(wpi::to_string_view(data));

  return 0;
}

}  // extern "C"

namespace hal::init {
void InitializeUsageReporting() {
  systemServerUsage = new ::SystemServerUsageReporting{hal::GetSystemServer()};
}
}  // namespace hal::init
