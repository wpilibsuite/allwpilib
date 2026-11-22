// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/UsageReporting.hpp"

#include <stdint.h>

#include <atomic>
#include <format>

#include "wpi/util/string.hpp"

using namespace wpi::util;

namespace {

void DefaultReportUsage(const WPI_String*, const WPI_String*) {}

std::atomic<WPI_ReportUsageImpl> reportUsageImpl{DefaultReportUsage};

}  // namespace

void wpi::util::ReportUsage(std::string_view resource, int32_t instanceNumber,
                            std::string_view data) {
  ReportUsage(std::format("{}[{}]", resource, instanceNumber), data);
}

void wpi::util::SetReportUsageImpl(WPI_ReportUsageImpl func) {
  WPI_SetReportUsageImpl(func);
}

extern "C" {

void WPI_ReportUsage(const WPI_String* resource, const WPI_String* data) {
  reportUsageImpl.load(std::memory_order_relaxed)(resource, data);
}

void WPI_ReportUsageInstance(const WPI_String* resource, int32_t instanceNumber,
                             const WPI_String* data) {
  wpi::util::ReportUsage(wpi::util::to_string_view(resource), instanceNumber,
                         wpi::util::to_string_view(data));
}

void WPI_SetReportUsageImpl(WPI_ReportUsageImpl func) {
  reportUsageImpl.store(func ? func : DefaultReportUsage,
                        std::memory_order_relaxed);
}

}  // extern "C"
