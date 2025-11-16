// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <fmt/format.h>

#include "SystemServerInternal.h"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/string.h"

namespace {
struct SystemServerUsageReporting {
  wpi::nt::NetworkTableInstance ntInst;
  wpi::util::StringMap<wpi::nt::StringPublisher> publishers;

  explicit SystemServerUsageReporting(wpi::nt::NetworkTableInstance inst)
      : ntInst{inst} {}
};

}  // namespace

static ::SystemServerUsageReporting* systemServerUsage;

extern "C" {

int32_t HAL_ReportUsage(const struct WPI_String* resource,
                        const struct WPI_String* data) {
  auto resourceStr = wpi::util::to_string_view(resource);
  auto& publisher = systemServerUsage->publishers[resourceStr];
  if (!publisher) {
    publisher =
        systemServerUsage->ntInst
            .GetStringTopic(fmt::format("/UsageReporting/{}", resourceStr))
            .Publish();
  }
  publisher.Set(wpi::util::to_string_view(data));

  return 0;
}

}  // extern "C"

namespace wpi::hal::init {
void InitializeUsageReporting() {
  systemServerUsage =
      new ::SystemServerUsageReporting{wpi::hal::GetSystemServer()};
}
}  // namespace wpi::hal::init
