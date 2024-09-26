// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/MathShared.h"

#include <memory>
#include <utility>

#include <wpi/mutex.h>
#include <wpi/timestamp.h>

#include "units/time.h"

using namespace wpi::math;

namespace {
class DefaultMathShared : public MathShared {
 public:
  void ReportErrorV(fmt::string_view format, fmt::format_args args) override {}
  void ReportWarningV(fmt::string_view format, fmt::format_args args) override {
  }
  void ReportUsage(MathUsageId id, int count) override {}
  units::second_t GetTimestamp() override {
    return units::second_t{wpi::Now() * 1.0e-6};
  }
};
}  // namespace

static std::unique_ptr<MathShared> mathShared;
static wpi::mutex setLock;

MathShared& MathSharedStore::GetMathShared() {
  std::scoped_lock lock(setLock);
  if (!mathShared) {
    mathShared = std::make_unique<DefaultMathShared>();
  }
  return *mathShared;
}

void MathSharedStore::SetMathShared(std::unique_ptr<MathShared> shared) {
  std::scoped_lock lock(setLock);
  mathShared = std::move(shared);
}
