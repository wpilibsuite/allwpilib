/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"

#include <atomic>

#include <hal/Notifier.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/priority_queue.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc2/Timer.h"

using namespace frc;

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

  wpi::mutex m_mutex;
  std::atomic<HAL_NotifierHandle> m_notifier;
  wpi::priority_queue<Watchdog*, std::vector<Watchdog*>,
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
  wpi_setGlobalHALError(status);
  HAL_SetNotifierName(m_notifier, "Watchdog", &status);

  m_thread = std::thread([=] { Main(); });
}

Watchdog::Impl::~Impl() {
  int32_t status = 0;
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_StopNotifier(handle, &status);
  wpi_setGlobalHALError(status);

  // Join the thread to ensure the handler has exited.
  if (m_thread.joinable()) m_thread.join();

  HAL_CleanNotifier(handle, &status);
}

void Watchdog::Impl::UpdateAlarm() {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  auto notifier = m_notifier.load();
  if (notifier == 0) return;
  if (m_watchdogs.empty())
    HAL_CancelNotifierAlarm(notifier, &status);
  else
    HAL_UpdateNotifierAlarm(
        notifier,
        static_cast<uint64_t>(m_watchdogs.top()->m_expirationTime.to<double>() *
                              1e6),
        &status);
  wpi_setGlobalHALError(status);
}

void Watchdog::Impl::Main() {
  for (;;) {
    int32_t status = 0;
    HAL_NotifierHandle notifier = m_notifier.load();
    if (notifier == 0) break;
    uint64_t curTime = HAL_WaitForNotifierAlarm(notifier, &status);
    if (curTime == 0 || status != 0) break;

    std::unique_lock lock(m_mutex);

    if (m_watchdogs.empty()) continue;

    // If the condition variable timed out, that means a Watchdog timeout
    // has occurred, so call its timeout function.
    auto watchdog = m_watchdogs.pop();

    units::second_t now{curTime * 1e-6};
    if (now - watchdog->m_lastTimeoutPrintTime > kMinPrintPeriod) {
      watchdog->m_lastTimeoutPrintTime = now;
      if (!watchdog->m_suppressTimeoutMessage) {
        wpi::SmallString<128> buf;
        wpi::raw_svector_ostream err(buf);
        err << "Watchdog not fed within "
            << wpi::format("%.6f", watchdog->m_timeout.to<double>()) << "s\n";
        frc::DriverStation::ReportWarning(err.str());
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

Watchdog::Watchdog(double timeout, std::function<void()> callback)
    : Watchdog(units::second_t{timeout}, callback) {}

Watchdog::Watchdog(units::second_t timeout, std::function<void()> callback)
    : m_timeout(timeout), m_callback(callback), m_impl(GetImpl()) {}

Watchdog::~Watchdog() { Disable(); }

Watchdog::Watchdog(Watchdog&& rhs) { *this = std::move(rhs); }

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

double Watchdog::GetTime() const {
  return (frc2::Timer::GetFPGATimestamp() - m_startTime).to<double>();
}

void Watchdog::SetTimeout(double timeout) {
  SetTimeout(units::second_t{timeout});
}

void Watchdog::SetTimeout(units::second_t timeout) {
  m_startTime = frc2::Timer::GetFPGATimestamp();
  m_tracer.ClearEpochs();

  std::scoped_lock lock(m_impl->m_mutex);
  m_timeout = timeout;
  m_isExpired = false;

  m_impl->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + m_timeout;
  m_impl->m_watchdogs.emplace(this);
  m_impl->UpdateAlarm();
}

double Watchdog::GetTimeout() const {
  std::scoped_lock lock(m_impl->m_mutex);
  return m_timeout.to<double>();
}

bool Watchdog::IsExpired() const {
  std::scoped_lock lock(m_impl->m_mutex);
  return m_isExpired;
}

void Watchdog::AddEpoch(wpi::StringRef epochName) {
  m_tracer.AddEpoch(epochName);
}

void Watchdog::PrintEpochs() { m_tracer.PrintEpochs(); }

void Watchdog::Reset() { Enable(); }

void Watchdog::Enable() {
  m_startTime = frc2::Timer::GetFPGATimestamp();
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
