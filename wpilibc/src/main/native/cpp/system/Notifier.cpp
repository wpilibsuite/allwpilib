// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Notifier.hpp"

#include <utility>

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/Notifier.hpp"
#include "wpi/hal/Threads.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi;

Notifier::Notifier(int priority, std::function<void()> callback) {
  if (!callback) {
    throw WPILIB_MakeError(err::NullParameter, "callback");
  }
  m_callback = callback;
  HAL_NotifierHandle handle;
  HAL_Status status = HAL_CreateNotifier(&handle);
  m_notifier = handle;
  WPILIB_CheckErrorStatus(status, "CreateNotifier");

  m_thread = std::thread([=, this] {
    if (priority > 0 && HAL_SetCurrentThreadPriority(priority) != 0) {
      WPILIB_ReportWarning("Setting Notifier priority to {} failed\n",
                           priority);
    }
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
      HAL_Status status = HAL_AcknowledgeNotifierAlarm(notifier);
      WPILIB_CheckErrorStatus(status, "AcknowledgeNotifier");
    }
  });
}

Notifier::~Notifier() {
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(HAL_INVALID_HANDLE);
  HAL_DestroyNotifier(handle);

  // Join the thread to ensure the callback has exited.
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

Notifier::Notifier(Notifier&& rhs)
    : m_thread(std::move(rhs.m_thread)),
      m_notifier(rhs.m_notifier.exchange(HAL_INVALID_HANDLE)),
      m_callback(std::move(rhs.m_callback)) {}

Notifier& Notifier::operator=(Notifier&& rhs) {
  m_thread = std::move(rhs.m_thread);
  m_notifier = rhs.m_notifier.exchange(HAL_INVALID_HANDLE);
  m_callback = std::move(rhs.m_callback);
  return *this;
}

void Notifier::SetName(std::string_view name) {
  HAL_SetNotifierName(m_notifier, name);
}

void Notifier::SetCallback(std::function<void()> callback) {
  std::scoped_lock lock(m_processMutex);
  m_callback = callback;
}

void Notifier::StartSingle(wpi::units::second_t delay) {
  HAL_Status status = HAL_SetNotifierAlarm(
      m_notifier, static_cast<uint64_t>(delay * 1e6), 0, false, false);
  WPILIB_CheckErrorStatus(status, "SetNotifierAlarm");
}

void Notifier::StartPeriodic(wpi::units::second_t period) {
  HAL_Status status =
      HAL_SetNotifierAlarm(m_notifier, static_cast<uint64_t>(period * 1e6),
                           static_cast<uint64_t>(period * 1e6), false, false);
  WPILIB_CheckErrorStatus(status, "SetNotifierAlarm");
}

void Notifier::StartPeriodic(wpi::units::hertz_t frequency) {
  StartPeriodic(1 / frequency);
}

void Notifier::Stop() {
  HAL_Status status = HAL_CancelNotifierAlarm(m_notifier, false);
  WPILIB_CheckErrorStatus(status, "CancelNotifierAlarm");
}

int32_t Notifier::GetOverrun() const {
  int32_t overrun = 0;
  HAL_Status status = HAL_GetNotifierOverrun(m_notifier, &overrun);
  WPILIB_CheckErrorStatus(status, "GetNotifierOverrun");
  return overrun;
}
