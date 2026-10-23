// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/SafeThread.hpp"

#include <chrono>
#include <future>
#include <memory>
#include <semaphore>
#include <utility>

#include <catch2/catch_test_macros.hpp>

using namespace std::chrono_literals;

namespace {

struct ExitState {
  std::binary_semaphore exiting{0};
  std::binary_semaphore finish{0};
};

class ExitingThread : public wpi::util::SafeThread {
 public:
  explicit ExitingThread(std::shared_ptr<ExitState> state)
      : m_state{std::move(state)} {}

  void Main() override {
    struct OnExit {
      std::shared_ptr<ExitState> state;

      ~OnExit() {
        state->exiting.release();
        state->finish.acquire();
      }
    };
    // Thread-local destruction runs after the SafeThread object is destroyed.
    thread_local OnExit onExit{m_state};
  }

 private:
  std::shared_ptr<ExitState> m_state;
};

}  // namespace

TEST_CASE("SafeThreadTest JoinAfterThreadObjectDestruction", "[wpiutil]") {
  auto state = std::make_shared<ExitState>();
  auto owner = std::make_unique<wpi::util::SafeThreadOwner<ExitingThread>>();
  owner->Start(state);
  state->exiting.acquire();
  CHECK_FALSE(*owner);

  std::future<void> joined;
  SECTION("Explicit join") {
    joined = std::async(std::launch::async, [&] { owner->Join(); });
  }
  SECTION("Owner destruction") {
    joined = std::async(std::launch::async, [&] { owner.reset(); });
  }

  // Joining must wait for thread-local destruction, even with an expired owner.
  CHECK(joined.wait_for(100ms) == std::future_status::timeout);
  state->finish.release();
  joined.get();
}
