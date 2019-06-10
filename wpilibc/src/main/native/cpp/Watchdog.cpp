/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"

#include <wpi/Format.h>
#include <wpi/PriorityQueue.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"

using namespace frc;

constexpr std::chrono::milliseconds Watchdog::kMinPrintPeriod;
std::condition_variable Watchdog::m_testCond;
wpi::mutex Watchdog::m_testMutex;

class Watchdog::Thread : public wpi::SafeThread {
 public:
  template <typename T>
  struct DerefGreater {
    constexpr bool operator()(const T& lhs, const T& rhs) const {
      return *lhs > *rhs;
    }
  };

  wpi::PriorityQueue<Watchdog*, std::vector<Watchdog*>, DerefGreater<Watchdog*>>
      m_watchdogs;

 private:
  void Main() override;
};

void Watchdog::Thread::Main() {
  std::unique_lock lock(m_mutex);

  while (m_active) {
    if (m_watchdogs.size() > 0) {
      if (m_cond.wait_until(lock, m_watchdogs.top()->m_expirationTime) ==
          std::cv_status::timeout) {
        std::lock_guard<wpi::mutex> testLock(m_testMutex);
        m_testCond.notify_all();
        if (m_watchdogs.size() == 0 ||
            m_watchdogs.top()->m_expirationTime > hal::fpga_clock::now()) {
          continue;
        }

        // If the condition variable timed out, that means a Watchdog timeout
        // has occurred, so call its timeout function.
        auto watchdog = m_watchdogs.top();
        m_watchdogs.pop();

        auto now = hal::fpga_clock::now();
        if (now - watchdog->m_lastTimeoutPrintTime > kMinPrintPeriod) {
          watchdog->m_lastTimeoutPrintTime = now;
          if (!watchdog->m_suppressTimeoutMessage) {
            wpi::SmallString<128> buf;
            wpi::raw_svector_ostream err(buf);
            err << "Watchdog not fed within "
                << wpi::format("%.6f", watchdog->m_timeout.count() / 1.0e9)
                << "s\n";
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
      }
      // Otherwise, a Watchdog removed itself from the queue (it notifies the
      // scheduler of this) or a spurious wakeup occurred, so just rewait with
      // the soonest watchdog timeout.
      std::lock_guard<wpi::mutex> testLock(m_testMutex);
      m_testCond.notify_all();
    } else {
      m_cond.wait(lock, [&] { return m_watchdogs.size() > 0 || !m_active; });
    }
  }
}

Watchdog::Watchdog(double timeout, std::function<void()> callback)
    : Watchdog(units::second_t{timeout}, callback) {}

Watchdog::Watchdog(units::second_t timeout, std::function<void()> callback)
    : m_timeout(timeout), m_callback(callback), m_owner(&GetThreadOwner()) {}

Watchdog::~Watchdog() { Disable(); }

double Watchdog::GetTime() const {
  return (hal::fpga_clock::now() - m_startTime).count() / 1.0e6;
}

void Watchdog::SetTimeout(double timeout) {
  SetTimeout(units::second_t{timeout});
}

void Watchdog::SetTimeout(units::second_t timeout) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  m_startTime = hal::fpga_clock::now();
  m_tracer.ClearEpochs();

  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  m_timeout = timeout;
  m_isExpired = false;

  thr->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + duration_cast<microseconds>(m_timeout);
  thr->m_watchdogs.emplace(this);
  thr->m_cond.notify_all();
}

double Watchdog::GetTimeout() const {
  // Locks mutex
  auto thr = m_owner->GetThread();

  return m_timeout.count() / 1.0e9;
}

bool Watchdog::IsExpired() const {
  // Locks mutex
  auto thr = m_owner->GetThread();

  return m_isExpired;
}

void Watchdog::AddEpoch(wpi::StringRef epochName) {
  m_tracer.AddEpoch(epochName);
}

void Watchdog::PrintEpochs() { m_tracer.PrintEpochs(); }

void Watchdog::Reset() { Enable(); }

void Watchdog::Enable() {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  m_startTime = hal::fpga_clock::now();
  m_tracer.ClearEpochs();

  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  m_isExpired = false;

  thr->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + duration_cast<microseconds>(m_timeout);
  thr->m_watchdogs.emplace(this);
  thr->m_cond.notify_all();
}

void Watchdog::Disable() {
  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  thr->m_watchdogs.remove(this);
  thr->m_cond.notify_all();
}

void Watchdog::SuppressTimeoutMessage(bool suppress) {
  m_suppressTimeoutMessage = suppress;
}

bool Watchdog::operator>(const Watchdog& rhs) {
  return m_expirationTime > rhs.m_expirationTime;
}

wpi::SafeThreadOwner<Watchdog::Thread>& Watchdog::GetThreadOwner() {
  static wpi::SafeThreadOwner<Thread> inst = [] {
    wpi::SafeThreadOwner<Watchdog::Thread> inst;
    inst.Start();
    return inst;
  }();
  return inst;
}

void Watchdog::Notify() {
  // Locks mutex
  auto thr = GetThreadOwner().GetThread();
  if (!thr) return;

  std::unique_lock<wpi::mutex> lock(m_testMutex);
  thr->m_cond.notify_all();
  thr->m_mutex.unlock();
  m_testCond.wait(lock);
}
