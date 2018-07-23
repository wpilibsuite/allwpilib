/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Timer.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

namespace wpi {
namespace uv {

TEST(UvTimer, StartAndStop) {
  auto loop = Loop::Create();
  auto handleNoRepeat = Timer::Create(loop);
  auto handleRepeat = Timer::Create(loop);

  bool checkTimerNoRepeatEvent = false;
  bool checkTimerRepeatEvent = false;

  handleNoRepeat->error.connect([](Error) { FAIL(); });
  handleRepeat->error.connect([](Error) { FAIL(); });

  handleNoRepeat->timeout.connect(
      [&checkTimerNoRepeatEvent, handle = handleNoRepeat.get() ] {
        ASSERT_FALSE(checkTimerNoRepeatEvent);
        checkTimerNoRepeatEvent = true;
        handle->Stop();
        handle->Close();
        ASSERT_TRUE(handle->IsClosing());
      });

  handleRepeat->timeout.connect(
      [&checkTimerRepeatEvent, handle = handleRepeat.get() ] {
        if (checkTimerRepeatEvent) {
          handle->Stop();
          handle->Close();
          ASSERT_TRUE(handle->IsClosing());
        } else {
          checkTimerRepeatEvent = true;
          ASSERT_FALSE(handle->IsClosing());
        }
      });

  handleNoRepeat->Start(Timer::Time{0}, Timer::Time{0});
  handleRepeat->Start(Timer::Time{0}, Timer::Time{1});

  ASSERT_TRUE(handleNoRepeat->IsActive());
  ASSERT_FALSE(handleNoRepeat->IsClosing());

  ASSERT_TRUE(handleRepeat->IsActive());
  ASSERT_FALSE(handleRepeat->IsClosing());

  loop->Run();

  ASSERT_TRUE(checkTimerNoRepeatEvent);
  ASSERT_TRUE(checkTimerRepeatEvent);
}

TEST(UvTimer, Repeat) {
  auto loop = Loop::Create();
  auto handle = Timer::Create(loop);

  handle->SetRepeat(Timer::Time{42});
  ASSERT_EQ(handle->GetRepeat(), Timer::Time{42});
  handle->Close();

  loop->Run();  // forces close callback to run
}

}  // namespace uv
}  // namespace wpi
