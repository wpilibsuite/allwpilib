// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/internal/PeriodicPriorityQueue.hpp"

#include <utility>

#include "wpi/hal/Notifier.h"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi::internal;

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::microseconds startTime,
                                          std::chrono::microseconds period,
                                          std::chrono::microseconds offset)
    : func{std::move(func)},
      period{period},
      expirationTime(
          startTime + offset + period +
          (std::chrono::microseconds{RobotController::GetMonotonicTime()} -
           startTime) /
              period * period) {}

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::microseconds startTime,
                                          wpi::units::second_t period,
                                          wpi::units::second_t offset)
    : Callback{
          std::move(func), startTime,
          std::chrono::microseconds{static_cast<int64_t>(period.value() * 1e6)},
          std::chrono::microseconds{
              static_cast<int64_t>(offset.value() * 1e6)}} {}

PeriodicPriorityQueue::Callback::Callback(std::function<void()> func,
                                          std::chrono::microseconds startTime,
                                          wpi::units::second_t period)
    : Callback{std::move(func), startTime, period,
               std::chrono::microseconds{0}} {}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::microseconds startTime,
                                std::chrono::microseconds period) {
  Add(std::move(func), startTime, period, std::chrono::microseconds{0});
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::microseconds startTime,
                                std::chrono::microseconds period,
                                std::chrono::microseconds offset) {
  m_queue.emplace(std::move(func), startTime, period, offset);
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::microseconds startTime,
                                wpi::units::second_t period) {
  Add(std::move(func), startTime, period, wpi::units::second_t{0});
}

void PeriodicPriorityQueue::Add(std::function<void()> func,
                                std::chrono::microseconds startTime,
                                wpi::units::second_t period,
                                wpi::units::second_t offset) {
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

  const std::chrono::microseconds currentTime{
      RobotController::GetMonotonicTime()};
  m_loopStartTime = wpi::units::microsecond_t{currentTime};

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
