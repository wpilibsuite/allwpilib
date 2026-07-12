// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/net/uv/Timer.hpp"
// clang-format on

#include <catch2/catch_test_macros.hpp>

namespace wpi::net::uv {

TEST_CASE("UvTimerTest StartAndStop", "[uv][timer]") {
  auto loop = Loop::Create();
  auto handleNoRepeat = Timer::Create(loop);
  auto handleRepeat = Timer::Create(loop);

  bool checkTimerNoRepeatEvent = false;
  bool checkTimerRepeatEvent = false;

  handleNoRepeat->error.connect([](Error) { FAIL(); });
  handleRepeat->error.connect([](Error) { FAIL(); });

  handleNoRepeat->timeout.connect(
      [&checkTimerNoRepeatEvent, handle = handleNoRepeat.get()] {
        REQUIRE_FALSE(checkTimerNoRepeatEvent);
        checkTimerNoRepeatEvent = true;
        handle->Stop();
        handle->Close();
        REQUIRE(handle->IsClosing());
      });

  handleRepeat->timeout.connect(
      [&checkTimerRepeatEvent, handle = handleRepeat.get()] {
        if (checkTimerRepeatEvent) {
          handle->Stop();
          handle->Close();
          REQUIRE(handle->IsClosing());
        } else {
          checkTimerRepeatEvent = true;
          REQUIRE_FALSE(handle->IsClosing());
        }
      });

  handleNoRepeat->Start(Timer::Time{0}, Timer::Time{0});
  handleRepeat->Start(Timer::Time{0}, Timer::Time{1});

  REQUIRE(handleNoRepeat->IsActive());
  REQUIRE_FALSE(handleNoRepeat->IsClosing());

  REQUIRE(handleRepeat->IsActive());
  REQUIRE_FALSE(handleRepeat->IsClosing());

  loop->Run();

  REQUIRE(checkTimerNoRepeatEvent);
  REQUIRE(checkTimerRepeatEvent);
}

TEST_CASE("UvTimerTest Repeat", "[uv][timer]") {
  auto loop = Loop::Create();
  auto handle = Timer::Create(loop);

  handle->SetRepeat(Timer::Time{42});
  REQUIRE(handle->GetRepeat() == Timer::Time{42});
  handle->Close();

  loop->Run();  // forces close callback to run
}

}  // namespace wpi::net::uv
