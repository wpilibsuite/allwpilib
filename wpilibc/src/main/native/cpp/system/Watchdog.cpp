// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Watchdog.hpp"

#include <atomic>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/hal/Notifier.h"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/priority_queue.hpp"

using namespace wpi;

class Watchdog::Impl {
 public:
  Impl();
  ~Impl();

  template <typename T>
  struct DerefGreater {
    constexpr bool operator()(const T& lhs, const T& rhs) const {
      return *lhs > *rhs;
    }
  };

  wpi::util::mutex m_mutex;
  std::atomic<HAL_NotifierHandle> m_notifier;
  wpi::util::priority_queue<Watchdog*, std::vector<Watchdog*>,
                      DerefGreater<Watchdog*>>
      m_watchdogs;

  void UpdateAlarm();

 private:
  void Main();

  std::thread m_thread;
};

Watchdog::Impl::Impl() {
  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  WPILIB_CheckErrorStatus(status, "starting watchdog notifier");
  HAL_SetNotifierName(m_notifier, "Watchdog", &status);

  m_thread = std::thread([=, this] { Main(); });
}

Watchdog::Impl::~Impl() {
  int32_t status = 0;
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_StopNotifier(handle, &status);
  WPILIB_ReportError(status, "stopping watchdog notifier");

  // Join the thread to ensure the handler has exited.
  if (m_thread.joinable()) {
    m_thread.join();
  }

  HAL_CleanNotifier(handle);
}

void Watchdog::Impl::UpdateAlarm() {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  auto notifier = m_notifier.load();
  if (notifier == 0) {
    return;
  }
  if (m_watchdogs.empty()) {
    HAL_CancelNotifierAlarm(notifier, &status);
  } else {
    HAL_UpdateNotifierAlarm(
        notifier,
        static_cast<uint64_t>(m_watchdogs.top()->m_expirationTime.value() *
                              1e6),
        &status);
  }
  WPILIB_CheckErrorStatus(status, "updating watchdog notifier alarm");
}

void Watchdog::Impl::Main() {
  for (;;) {
    int32_t status = 0;
    HAL_NotifierHandle notifier = m_notifier.load();
    if (notifier == 0) {
      break;
    }
    uint64_t curTime = HAL_WaitForNotifierAlarm(notifier, &status);
    if (curTime == 0 || status != 0) {
      break;
    }

    std::unique_lock lock(m_mutex);

    if (m_watchdogs.empty()) {
      continue;
    }

    // If the condition variable timed out, that means a Watchdog timeout
    // has occurred, so call its timeout function.
    auto watchdog = m_watchdogs.pop();

    wpi::units::second_t now{curTime * 1e-6};
    if (now - watchdog->m_lastTimeoutPrintTime > kMinPrintPeriod) {
      watchdog->m_lastTimeoutPrintTime = now;
      if (!watchdog->m_suppressTimeoutMessage) {
        WPILIB_ReportWarning("Watchdog not fed within {:.6f}s",
                          watchdog->m_timeout.value());
      }
    }

    // Set expiration flag before calling the callback so any manipulation
    // of the flag in the callback (e.g., calling Disable()) isn't
    // clobbered.
    watchdog->m_isExpired = true;

    lock.unlock();
    watchdog->m_callback();
    lock.lock();

    UpdateAlarm();
  }
}

Watchdog::Watchdog(wpi::units::second_t timeout, std::function<void()> callback)
    : m_timeout(timeout), m_callback(std::move(callback)), m_impl(GetImpl()) {}

Watchdog::~Watchdog() {
  try {
    Disable();
  } catch (const RuntimeError& e) {
    e.Report();
  }
}

Watchdog::Watchdog(Watchdog&& rhs) {
  *this = std::move(rhs);
}

Watchdog& Watchdog::operator=(Watchdog&& rhs) {
  m_impl = rhs.m_impl;
  std::scoped_lock lock(m_impl->m_mutex);
  m_startTime = rhs.m_startTime;
  m_timeout = rhs.m_timeout;
  m_expirationTime = rhs.m_expirationTime;
  m_callback = std::move(rhs.m_callback);
  m_lastTimeoutPrintTime = rhs.m_lastTimeoutPrintTime;
  m_suppressTimeoutMessage = rhs.m_suppressTimeoutMessage;
  m_tracer = std::move(rhs.m_tracer);
  m_isExpired = rhs.m_isExpired;
  if (m_expirationTime != 0_s) {
    m_impl->m_watchdogs.remove(&rhs);
    m_impl->m_watchdogs.emplace(this);
  }
  return *this;
}

wpi::units::second_t Watchdog::GetTime() const {
  return Timer::GetFPGATimestamp() - m_startTime;
}

void Watchdog::SetTimeout(wpi::units::second_t timeout) {
  m_startTime = Timer::GetFPGATimestamp();
  m_tracer.ClearEpochs();

  std::scoped_lock lock(m_impl->m_mutex);
  m_timeout = timeout;
  m_isExpired = false;

  m_impl->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + m_timeout;
  m_impl->m_watchdogs.emplace(this);
  m_impl->UpdateAlarm();
}

wpi::units::second_t Watchdog::GetTimeout() const {
  std::scoped_lock lock(m_impl->m_mutex);
  return m_timeout;
}

bool Watchdog::IsExpired() const {
  std::scoped_lock lock(m_impl->m_mutex);
  return m_isExpired;
}

void Watchdog::AddEpoch(std::string_view epochName) {
  m_tracer.AddEpoch(epochName);
}

void Watchdog::PrintEpochs() {
  m_tracer.PrintEpochs();
}

void Watchdog::Reset() {
  Enable();
}

void Watchdog::Enable() {
  m_startTime = Timer::GetFPGATimestamp();
  m_tracer.ClearEpochs();

  std::scoped_lock lock(m_impl->m_mutex);
  m_isExpired = false;

  m_impl->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + m_timeout;
  m_impl->m_watchdogs.emplace(this);
  m_impl->UpdateAlarm();
}

void Watchdog::Disable() {
  std::scoped_lock lock(m_impl->m_mutex);

  if (m_expirationTime != 0_s) {
    m_impl->m_watchdogs.remove(this);
    m_expirationTime = 0_s;
    m_impl->UpdateAlarm();
  }
}

void Watchdog::SuppressTimeoutMessage(bool suppress) {
  m_suppressTimeoutMessage = suppress;
}

bool Watchdog::operator>(const Watchdog& rhs) const {
  return m_expirationTime > rhs.m_expirationTime;
}

Watchdog::Impl* Watchdog::GetImpl() {
  static Impl inst;
  return &inst;
}
