/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
  if (!mathShared) mathShared = std::make_unique<DefaultMathShared>();
  return *mathShared;
}

void MathSharedStore::SetMathShared(std::unique_ptr<MathShared> shared) {
  std::scoped_lock lock(setLock);
  mathShared = std::move(shared);
}
