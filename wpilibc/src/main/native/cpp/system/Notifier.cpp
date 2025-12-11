// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Notifier.hpp"

#include <utility>

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/Notifier.h"
#include "wpi/hal/Threads.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi;

Notifier::Notifier(std::function<void()> callback) {
  if (!callback) {
    throw WPILIB_MakeError(err::NullParameter, "callback");
  }
  m_callback = callback;
  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  WPILIB_CheckErrorStatus(status, "CreateNotifier");

  m_thread = std::thread([=, this] {
    for (;;) {
      int32_t status = 0;
      HAL_NotifierHandle notifier = m_notifier.load();
      if (notifier == 0) {
        break;
      }
      if (WPI_WaitForObject(notifier) == 0) {
        break;
      }

      std::function<void()> callback;
      {
        std::scoped_lock lock(m_processMutex);
        callback = m_callback;
      }

      // Call callback
      if (callback) {
        callback();
      }

      // Ack notifier
      HAL_AcknowledgeNotifierAlarm(notifier, false, 0, 0, false, &status);
      WPILIB_CheckErrorStatus(status, "AcknowledgeNotifier");
    }
  });
}

Notifier::Notifier(int priority, std::function<void()> callback) {
  if (!callback) {
    throw WPILIB_MakeError(err::NullParameter, "callback");
  }
  m_callback = callback;
  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  WPILIB_CheckErrorStatus(status, "InitializeNotifier");

  m_thread = std::thread([=, this] {
    int32_t status = 0;
    HAL_SetCurrentThreadPriority(true, priority, &status);
    for (;;) {
      HAL_NotifierHandle notifier = m_notifier.load();
      if (notifier == 0) {
        break;
      }
      if (WPI_WaitForObject(notifier) == 0) {
        break;
      }

      std::function<void()> callback;
      {
        std::scoped_lock lock(m_processMutex);
        callback = m_callback;
      }

      // call callback
      if (callback) {
        try {
          callback();
        } catch (const wpi::RuntimeError& e) {
          e.Report();
          WPILIB_ReportError(
              err::Error,
              "Error in Notifier thread."
              "  The above stacktrace can help determine where the error "
              "occurred.\n"
              "  See https://wpilib.org/stacktrace for more information.\n");
          throw;
        } catch (const std::exception& e) {
          HAL_SendError(1, err::Error, 0, e.what(), "", "", 1);
          throw;
        }
      }

      // Ack notifier
      HAL_AcknowledgeNotifierAlarm(notifier, false, 0, 0, false, &status);
      WPILIB_CheckErrorStatus(status, "AcknowledgeNotifier");
    }
  });
}

Notifier::~Notifier() {
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_DestroyNotifier(handle);

  // Join the thread to ensure the callback has exited.
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

Notifier::Notifier(Notifier&& rhs)
    : m_thread(std::move(rhs.m_thread)),
      m_notifier(rhs.m_notifier.load()),
      m_callback(std::move(rhs.m_callback)) {
  rhs.m_notifier = HAL_kInvalidHandle;
}

Notifier& Notifier::operator=(Notifier&& rhs) {
  m_thread = std::move(rhs.m_thread);
  m_notifier = rhs.m_notifier.load();
  rhs.m_notifier = HAL_kInvalidHandle;
  m_callback = std::move(rhs.m_callback);
  return *this;
}

void Notifier::SetName(std::string_view name) {
  int32_t status = 0;
  HAL_SetNotifierName(m_notifier, name, &status);
}

void Notifier::SetCallback(std::function<void()> callback) {
  std::scoped_lock lock(m_processMutex);
  m_callback = callback;
}

void Notifier::StartSingle(wpi::units::second_t delay) {
  int32_t status = 0;
  HAL_SetNotifierAlarm(m_notifier, static_cast<uint64_t>(delay * 1e6), 0, false,
                       &status);
}

void Notifier::StartPeriodic(wpi::units::second_t period) {
  int32_t status = 0;
  HAL_SetNotifierAlarm(m_notifier, static_cast<uint64_t>(period * 1e6),
                       static_cast<uint64_t>(period * 1e6), false, &status);
}

void Notifier::StartPeriodic(wpi::units::hertz_t frequency) {
  StartPeriodic(1 / frequency);
}

void Notifier::Stop() {
  int32_t status = 0;
  HAL_CancelNotifierAlarm(m_notifier, &status);
  WPILIB_CheckErrorStatus(status, "CancelNotifierAlarm");
}

int32_t Notifier::GetOverrun() const {
  int32_t status = 0;
  int32_t overrun = HAL_GetNotifierOverrun(m_notifier, &status);
  WPILIB_CheckErrorStatus(status, "GetNotifierOverrun");
  return overrun;
}

bool Notifier::SetHALThreadPriority(bool realTime, int32_t priority) {
  int32_t status = 0;
  return HAL_SetNotifierThreadPriority(realTime, priority, &status);
}
