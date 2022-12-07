// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Notifier.h"

#include <utility>

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Notifier.h>
#include <hal/Threads.h>

#include "frc/Errors.h"
#include "frc/Timer.h"

using namespace frc;

Notifier::Notifier(std::function<void()> handler) {
  if (!handler) {
    throw FRC_MakeError(err::NullParameter, "handler");
  }
  m_handler = handler;
  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  FRC_CheckErrorStatus(status, "InitializeNotifier");

  m_thread = std::thread([=, this] {
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

      std::function<void()> handler;
      {
        std::scoped_lock lock(m_processMutex);
        handler = m_handler;
        if (m_periodic) {
          m_expirationTime += m_period;
          UpdateAlarm();
        } else {
          // need to update the alarm to cause it to wait again
          UpdateAlarm(UINT64_MAX);
        }
      }

      // call callback
      if (handler) {
        handler();
      }
    }
  });
}

Notifier::Notifier(int priority, std::function<void()> handler) {
  if (!handler) {
    throw FRC_MakeError(err::NullParameter, "handler");
  }
  m_handler = handler;
  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  FRC_CheckErrorStatus(status, "InitializeNotifier");

  m_thread = std::thread([=, this] {
    int32_t status = 0;
    HAL_SetCurrentThreadPriority(true, priority, &status);
    for (;;) {
      HAL_NotifierHandle notifier = m_notifier.load();
      if (notifier == 0) {
        break;
      }
      uint64_t curTime = HAL_WaitForNotifierAlarm(notifier, &status);
      if (curTime == 0 || status != 0) {
        break;
      }

      std::function<void()> handler;
      {
        std::scoped_lock lock(m_processMutex);
        handler = m_handler;
        if (m_periodic) {
          m_expirationTime += m_period;
          UpdateAlarm();
        } else {
          // need to update the alarm to cause it to wait again
          UpdateAlarm(UINT64_MAX);
        }
      }

      // call callback
      if (handler) {
        handler();
      }
    }
  });
}

Notifier::~Notifier() {
  int32_t status = 0;
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_StopNotifier(handle, &status);
  FRC_ReportError(status, "StopNotifier");

  // Join the thread to ensure the handler has exited.
  if (m_thread.joinable()) {
    m_thread.join();
  }

  HAL_CleanNotifier(handle, &status);
}

Notifier::Notifier(Notifier&& rhs)
    : m_thread(std::move(rhs.m_thread)),
      m_notifier(rhs.m_notifier.load()),
      m_handler(std::move(rhs.m_handler)),
      m_expirationTime(std::move(rhs.m_expirationTime)),
      m_period(std::move(rhs.m_period)),
      m_periodic(std::move(rhs.m_periodic)) {
  rhs.m_notifier = HAL_kInvalidHandle;
}

Notifier& Notifier::operator=(Notifier&& rhs) {
  m_thread = std::move(rhs.m_thread);
  m_notifier = rhs.m_notifier.load();
  rhs.m_notifier = HAL_kInvalidHandle;
  m_handler = std::move(rhs.m_handler);
  m_expirationTime = std::move(rhs.m_expirationTime);
  m_period = std::move(rhs.m_period);
  m_periodic = std::move(rhs.m_periodic);

  return *this;
}

void Notifier::SetName(std::string_view name) {
  fmt::memory_buffer buf;
  fmt::format_to(fmt::appender{buf}, "{}", name);
  buf.push_back('\0');  // null terminate
  int32_t status = 0;
  HAL_SetNotifierName(m_notifier, buf.data(), &status);
}

void Notifier::SetHandler(std::function<void()> handler) {
  std::scoped_lock lock(m_processMutex);
  m_handler = handler;
}

void Notifier::StartSingle(units::second_t delay) {
  std::scoped_lock lock(m_processMutex);
  m_periodic = false;
  m_period = delay;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

void Notifier::StartPeriodic(units::second_t period) {
  std::scoped_lock lock(m_processMutex);
  m_periodic = true;
  m_period = period;
  m_expirationTime = Timer::GetFPGATimestamp() + m_period;
  UpdateAlarm();
}

void Notifier::Stop() {
  std::scoped_lock lock(m_processMutex);
  m_periodic = false;
  int32_t status = 0;
  HAL_CancelNotifierAlarm(m_notifier, &status);
  FRC_CheckErrorStatus(status, "CancelNotifierAlarm");
}

void Notifier::UpdateAlarm(uint64_t triggerTime) {
  int32_t status = 0;
  // Return if we are being destructed, or were not created successfully
  auto notifier = m_notifier.load();
  if (notifier == 0) {
    return;
  }
  HAL_UpdateNotifierAlarm(notifier, triggerTime, &status);
  FRC_CheckErrorStatus(status, "UpdateNotifierAlarm");
}

void Notifier::UpdateAlarm() {
  UpdateAlarm(static_cast<uint64_t>(m_expirationTime * 1e6));
}

bool Notifier::SetHALThreadPriority(bool realTime, int32_t priority) {
  int32_t status = 0;
  return HAL_SetNotifierThreadPriority(realTime, priority, &status);
}
