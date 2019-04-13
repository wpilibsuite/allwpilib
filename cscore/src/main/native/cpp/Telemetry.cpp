/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Telemetry.h"

#include <chrono>
#include <limits>

#include <wpi/DenseMap.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Notifier.h"
#include "SourceImpl.h"
#include "cscore_cpp.h"

using namespace cs;

class Telemetry::Thread : public wpi::SafeThread {
 public:
  explicit Thread(Notifier& notifier) : m_notifier(notifier) {}

  void Main();

  Notifier& m_notifier;
  wpi::DenseMap<std::pair<CS_Handle, int>, int64_t> m_user;
  wpi::DenseMap<std::pair<CS_Handle, int>, int64_t> m_current;
  double m_period = 0.0;
  double m_elapsed = 0.0;
  bool m_updated = false;
  int64_t GetValue(CS_Handle handle, CS_TelemetryKind kind, CS_Status* status);
};

int64_t Telemetry::Thread::GetValue(CS_Handle handle, CS_TelemetryKind kind,
                                    CS_Status* status) {
  auto it = m_user.find(std::make_pair(handle, static_cast<int>(kind)));
  if (it == m_user.end()) {
    *status = CS_EMPTY_VALUE;
    return 0;
  }
  return it->getSecond();
}

Telemetry::~Telemetry() {}

void Telemetry::Start() { m_owner.Start(m_notifier); }

void Telemetry::Stop() { m_owner.Stop(); }

void Telemetry::Thread::Main() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  auto prevTime = std::chrono::steady_clock::now();
  while (m_active) {
    double period = m_period;
    if (period == 0) period = 1000.0;
    auto timeoutTime = prevTime + std::chrono::duration<double>(period);
    while (m_active && !m_updated) {
      if (m_cond.wait_until(lock, timeoutTime) == std::cv_status::timeout)
        break;
    }
    if (!m_active) break;
    if (m_updated) {
      m_updated = false;
      continue;
    }

    // move to user and clear current, as we don't keep around old values
    m_user = std::move(m_current);
    m_current.clear();
    auto curTime = std::chrono::steady_clock::now();
    m_elapsed = std::chrono::duration<double>(curTime - prevTime).count();
    prevTime = curTime;

    // notify
    m_notifier.NotifyTelemetryUpdated();
  }
}

void Telemetry::SetPeriod(double seconds) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  if (thr->m_period == seconds) return;  // no change
  thr->m_period = seconds;
  thr->m_updated = true;
  thr->m_cond.notify_one();
}

double Telemetry::GetElapsedTime() {
  auto thr = m_owner.GetThread();
  if (!thr) return 0;
  return thr->m_elapsed;
}

int64_t Telemetry::GetValue(CS_Handle handle, CS_TelemetryKind kind,
                            CS_Status* status) {
  auto thr = m_owner.GetThread();
  if (!thr) {
    *status = CS_TELEMETRY_NOT_ENABLED;
    return 0;
  }
  return thr->GetValue(handle, kind, status);
}

double Telemetry::GetAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                                  CS_Status* status) {
  auto thr = m_owner.GetThread();
  if (!thr) {
    *status = CS_TELEMETRY_NOT_ENABLED;
    return 0;
  }
  if (thr->m_elapsed == 0) return 0.0;
  return thr->GetValue(handle, kind, status) / thr->m_elapsed;
}

void Telemetry::RecordSourceBytes(const SourceImpl& source, int quantity) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  auto handleData = Instance::GetInstance().FindSource(source);
  thr->m_current[std::make_pair(Handle{handleData.first, Handle::kSource},
                                static_cast<int>(CS_SOURCE_BYTES_RECEIVED))] +=
      quantity;
}

void Telemetry::RecordSourceFrames(const SourceImpl& source, int quantity) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  auto handleData = Instance::GetInstance().FindSource(source);
  thr->m_current[std::make_pair(Handle{handleData.first, Handle::kSource},
                                static_cast<int>(CS_SOURCE_FRAMES_RECEIVED))] +=
      quantity;
}
