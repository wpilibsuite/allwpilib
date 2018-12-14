/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Watchdog.h"

#include <wpi/Format.h>
#include <wpi/PriorityQueue.h>
#include <wpi/raw_ostream.h>

using namespace frc;

constexpr std::chrono::milliseconds Watchdog::kMinPrintPeriod;

class Watchdog::Thread : public wpi::SafeThread {
 public:
  template <typename T>
  struct DerefGreater : public std::binary_function<T, T, bool> {
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
  std::unique_lock<wpi::mutex> lock(m_mutex);

  while (m_active) {
    if (m_watchdogs.size() > 0) {
      if (m_cond.wait_until(lock, m_watchdogs.top()->m_expirationTime) ==
          std::cv_status::timeout) {
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
            wpi::outs() << "Watchdog not fed within "
                        << wpi::format("%.6f",
                                       watchdog->m_timeout.count() / 1.0e6)
                        << "s\n";
          }
        }
        lock.unlock();
        watchdog->m_callback();
        lock.lock();
        watchdog->m_isExpired = true;
      }
      // Otherwise, a Watchdog removed itself from the queue (it notifies the
      // scheduler of this) or a spurious wakeup occurred, so just rewait with
      // the soonest watchdog timeout.
    } else {
      m_cond.wait(lock, [&] { return m_watchdogs.size() > 0 || !m_active; });
    }
  }
}

Watchdog::Watchdog(double timeout, std::function<void()> callback)
    : m_timeout(static_cast<int64_t>(timeout * 1.0e6)),
      m_callback(callback),
      m_owner(&GetThreadOwner()) {}

Watchdog::~Watchdog() { Disable(); }

double Watchdog::GetTime() const {
  return (hal::fpga_clock::now() - m_startTime).count() / 1.0e6;
}

void Watchdog::SetTimeout(double timeout) {
  m_startTime = hal::fpga_clock::now();
  m_epochs.clear();

  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  m_timeout = std::chrono::microseconds(static_cast<int64_t>(timeout * 1.0e6));
  m_isExpired = false;

  thr->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + m_timeout;
  thr->m_watchdogs.emplace(this);
  thr->m_cond.notify_all();
}

double Watchdog::GetTimeout() const {
  // Locks mutex
  auto thr = m_owner->GetThread();

  return m_timeout.count() / 1.0e6;
}

bool Watchdog::IsExpired() const {
  // Locks mutex
  auto thr = m_owner->GetThread();

  return m_isExpired;
}

void Watchdog::AddEpoch(wpi::StringRef epochName) {
  auto currentTime = hal::fpga_clock::now();
  m_epochs[epochName] = currentTime - m_startTime;
  m_startTime = currentTime;
}

void Watchdog::PrintEpochs() {
  auto now = hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      wpi::outs() << '\t' << epoch.getKey() << ": "
                  << wpi::format("%.6f", epoch.getValue().count() / 1.0e6)
                  << "s\n";
    }
  }
}

void Watchdog::Reset() { Enable(); }

void Watchdog::Enable() {
  m_startTime = hal::fpga_clock::now();
  m_epochs.clear();

  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  m_isExpired = false;

  thr->m_watchdogs.remove(this);
  m_expirationTime = m_startTime + m_timeout;
  thr->m_watchdogs.emplace(this);
  thr->m_cond.notify_all();
}

void Watchdog::Disable() {
  // Locks mutex
  auto thr = m_owner->GetThread();
  if (!thr) return;

  m_isExpired = false;

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
