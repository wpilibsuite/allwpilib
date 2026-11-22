// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/UsageReporting.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/UsageReporting.hpp"
#include "wpi/util/string.hpp"

namespace {

std::string resource;
std::string data;

void MockReportUsage(const WPI_String* resourceStr, const WPI_String* dataStr) {
  resource = wpi::util::to_string_view(resourceStr);
  data = wpi::util::to_string_view(dataStr);
}

class ResetReportUsageImpl {
 public:
  ~ResetReportUsageImpl() { wpi::util::SetReportUsageImpl(nullptr); }
};

}  // namespace

TEST_CASE("UsageReportingTest DefaultNoOp", "[wpiutil]") {
  wpi::util::ReportUsage("Resource", "Data");
  WPI_ReportUsage(nullptr, nullptr);
}

TEST_CASE("UsageReportingTest CppReportUsageCallsImpl", "[wpiutil]") {
  ResetReportUsageImpl reset;
  wpi::util::SetReportUsageImpl(MockReportUsage);

  wpi::util::ReportUsage("Resource", "Data");

  CHECK("Resource" == resource);
  CHECK("Data" == data);
}

TEST_CASE("UsageReportingTest CppReportUsageInstanceFormatsResource",
          "[wpiutil]") {
  ResetReportUsageImpl reset;
  wpi::util::SetReportUsageImpl(MockReportUsage);

  wpi::util::ReportUsage("Resource", 4, "Data");

  CHECK("Resource[4]" == resource);
  CHECK("Data" == data);
}

TEST_CASE("UsageReportingTest CReportUsageInstanceFormatsResource",
          "[wpiutil]") {
  ResetReportUsageImpl reset;
  WPI_SetReportUsageImpl(MockReportUsage);

  WPI_String resourceStr = wpi::util::make_string("Resource");
  WPI_String dataStr = wpi::util::make_string("Data");
  WPI_ReportUsageInstance(&resourceStr, 5, &dataStr);

  CHECK("Resource[5]" == resource);
  CHECK("Data" == data);
}
