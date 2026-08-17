// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/SmartDashboard.hpp"

#include <memory>
#include <string_view>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

namespace {

class TestSendable : public wpi::util::Sendable,
                     public wpi::util::SendableHelper<TestSendable> {
 public:
  TestSendable() {
    wpi::util::SendableRegistry::Add(this, "SmartDashboardTestSendable");
  }

  void InitSendable(wpi::util::SendableBuilder&) override { ++initCount; }

  int initCount = 0;
};

}  // namespace

TEST_CASE("SmartDashboardTest republishes sendable after UID reuse",
          "[wpilibc][smartdashboard]") {
  constexpr std::string_view key = "SendableUidReuse";

  wpi::util::SendableRegistry::UID firstUid;
  {
    TestSendable first;
    firstUid = wpi::util::SendableRegistry::GetUniqueId(&first);
    wpi::SmartDashboard::PutData(key, &first);
    REQUIRE(first.initCount == 1);
  }

  std::unique_ptr<TestSendable> replacement;
  for (int i = 0; i < 10000; ++i) {
    auto candidate = std::make_unique<TestSendable>();
    if (wpi::util::SendableRegistry::GetUniqueId(candidate.get()) == firstUid) {
      replacement = std::move(candidate);
      break;
    }
  }

  REQUIRE(replacement);
  wpi::SmartDashboard::PutData(key, replacement.get());
  REQUIRE(replacement->initCount == 1);
  wpi::SmartDashboard::PutData(key, replacement.get());
  CHECK(replacement->initCount == 1);
}
