// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/MathShared.h"

#include <wpi/mutex.h>

using namespace wpi::math;

namespace {
class DefaultMathShared : public MathShared {
 public:
  void ReportError(const wpi::Twine& error) override {}
  void ReportUsage(MathUsageId id, int count) override {}
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
