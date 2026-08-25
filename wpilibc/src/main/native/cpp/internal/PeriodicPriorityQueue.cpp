// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/internal/PeriodicPriorityQueue.hpp"

#include <atomic>
#include <cstdint>
#include <utility>

#include "wpi/hal/Notifier.h"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi::internal;

namespace {
// Monotonic source of stable callback identities. Shared across all callbacks
// so each fresh construction gets a unique id; copies preserve their id.
std::atomic<uint64_t> gNextCallbackId{1};
}  // namespace

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::nanoseconds startTime,
                                          std::chrono::nanoseconds period,
                                          std::chrono::nanoseconds offset)
    : func{std::move(func)},
      period{period},
      expirationTime(
          startTime + offset + period +
          (std::chrono::nanoseconds{RobotController::GetMonotonicTime()} -
           startTime) /
              period * period),
      id{gNextCallbackId.fetch_add(1, std::memory_order_relaxed)} {}

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::nanoseconds startTime,
                                          wpi::units::seconds<> period,
                                          wpi::units::seconds<> offset)
    : Callback{
          std::move(func), startTime,
          std::chrono::nanoseconds{static_cast<int64_t>(period.value() * 1e9)},
          std::chrono::nanoseconds{
              static_cast<int64_t>(offset.value() * 1e9)}} {}

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::nanoseconds startTime,
                                          wpi::units::seconds<> period)
    : Callback{std::move(func), startTime, period, 0_s} {}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::nanoseconds startTime,
                                std::chrono::nanoseconds period) {
  Add(std::move(func), startTime, period, std::chrono::nanoseconds{0});
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::nanoseconds startTime,
                                std::chrono::nanoseconds period,
                                std::chrono::nanoseconds offset) {
  m_queue.emplace(std::move(func), startTime, period, offset);
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::nanoseconds startTime,
                                wpi::units::seconds<> period) {
  Add(std::move(func), startTime, period, wpi::units::seconds<>{0});
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::nanoseconds startTime,
                                wpi::units::seconds<> period,
                                wpi::units::seconds<> offset) {
  m_queue.emplace(std::move(func), startTime, period, offset);
}

void PeriodicPriorityQueue::Add(Callback callback) {
  m_queue.push(std::move(callback));
}

bool PeriodicPriorityQueue::Remove(const Callback& callback) {
  return m_queue.remove(callback);
}

void PeriodicPriorityQueue::Clear() {
  while (!m_queue.empty()) {
    m_queue.pop();
  }
}

bool PeriodicPriorityQueue::RunCallbacks(HAL_NotifierHandle notifier) {
  // We don't have to check there's an element in the queue first because
  // there's always at least one (the constructor adds one). It's reenqueued
  // at the end of the loop.
  auto callback = m_queue.pop();

  int32_t status = 0;
  HAL_SetNotifierAlarm(notifier, callback.expirationTime.count(), 0, true, true,
                       &status);
  WPILIB_CheckErrorStatus(status, "SetNotifierAlarm");

  if (WPI_WaitForObject(notifier) == 0) {
    return false;
  }

  const std::chrono::nanoseconds currentTime{
      RobotController::GetMonotonicTime()};
  m_loopStartTime = wpi::units::nanoseconds<>{currentTime};

  callback.func();

  // Increment the expiration time by the number of full periods it's behind
  // plus one to avoid rapid repeat fires from a large loop overrun. We assume
  // currentTime >= expirationTime rather than checking for it since the
  // callback wouldn't be running otherwise.
  callback.expirationTime +=
      callback.period + (currentTime - callback.expirationTime) /
                            callback.period * callback.period;
  m_queue.push(std::move(callback));

  // Process all other callbacks that are ready to run
  while (m_queue.top().expirationTime <= currentTime) {
    callback = m_queue.pop();

    callback.func();

    callback.expirationTime +=
        callback.period + (currentTime - callback.expirationTime) /
                              callback.period * callback.period;
    m_queue.push(std::move(callback));
  }

  return true;
}
