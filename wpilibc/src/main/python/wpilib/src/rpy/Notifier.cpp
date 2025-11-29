// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "rpy/Notifier.h"

#include <utility>

#include <fmt/format.h>

#include "wpi/hal/Notifier.h"
#include "wpi/hal/Threads.h"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/util/Synchronization.h"

#include <pybind11/functional.h>
#include <gilsafe_object.h>

using namespace wpi;
using namespace pybind11::literals;

// Hang the thread since returning to the caller is going to crash when we try
// to obtain the GIL again
// - this is a daemon thread so it's fine?
// - Python 3.14 does this too
static void _hang_thread_if_finalizing() {
  if (Py_IsFinalizing()) {
    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds(1000));
    }
  }
}

PyNotifier::PyNotifier(std::function<void()> handler) {
  if (!handler) {
    throw WPILIB_MakeError(err::NullParameter, "handler");
  }
  m_handler = handler;
  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  WPILIB_CheckErrorStatus(status, "CreateNotifier");

  std::function<void()> target([this] {
    py::gil_scoped_release release;

    try {
      for (;;) {
        int32_t status = 0;
        HAL_NotifierHandle notifier = m_notifier.load();
        if (notifier == 0) {
          break;
        }
        if (WPI_WaitForObject(notifier) == 0) {
          break;
        }

        std::function<void()> handler;
        {
          std::scoped_lock lock(m_processMutex);
          handler = m_handler;
        }

        // call callback
        if (handler) {
          if (Py_IsFinalizing()) {
            break;
          }

          handler();
        }

        // Ack notifier
        HAL_AcknowledgeNotifierAlarm(notifier, &status);
        WPILIB_CheckErrorStatus(status, "AcknowledgeNotifier");
      }
    } catch (...) {
      _hang_thread_if_finalizing();
      throw;
    }

    _hang_thread_if_finalizing();
  });

  py::gil_scoped_acquire acquire;

  // create a python thread and start it
  auto Thread = py::module::import("threading").attr("Thread");
  m_thread = Thread("target"_a = target, "daemon"_a = true,
                    "name"_a = "notifier-thread");
  m_thread.attr("start")();
}

PyNotifier::~PyNotifier() {
  // atomically set handle to 0, then clean
  HAL_NotifierHandle handle = m_notifier.exchange(0);
  HAL_DestroyNotifier(handle);

  // Join the thread to ensure the handler has exited.
  if (m_thread) {
    m_thread.attr("join")();
  }
}

PyNotifier::PyNotifier(PyNotifier &&rhs)
    : m_thread(std::move(rhs.m_thread)),
      m_notifier(rhs.m_notifier.load()),
      m_handler(std::move(rhs.m_handler)) {
  rhs.m_notifier = HAL_kInvalidHandle;
}

PyNotifier &PyNotifier::operator=(PyNotifier &&rhs) {
  m_thread = std::move(rhs.m_thread);
  m_notifier = rhs.m_notifier.load();
  rhs.m_notifier = HAL_kInvalidHandle;
  m_handler = std::move(rhs.m_handler);
  return *this;
}

void PyNotifier::SetName(std::string_view name) {
  int32_t status = 0;
  HAL_SetNotifierName(m_notifier, name, &status);
}

void PyNotifier::SetCallback(std::function<void()> handler) {
  std::scoped_lock lock(m_processMutex);
  m_handler = handler;
}

void PyNotifier::StartSingle(wpi::units::second_t delay) {
  int32_t status = 0;
  HAL_SetNotifierAlarm(m_notifier, static_cast<uint64_t>(delay * 1e6), 0, false,
                       &status);
}

void PyNotifier::StartPeriodic(wpi::units::second_t period) {
  int32_t status = 0;
  HAL_SetNotifierAlarm(m_notifier, static_cast<uint64_t>(period * 1e6),
                       static_cast<uint64_t>(period * 1e6), false, &status);
}

void PyNotifier::Stop() {
  int32_t status = 0;
  HAL_CancelNotifierAlarm(m_notifier, &status);
  WPILIB_CheckErrorStatus(status, "CancelNotifierAlarm");
}

int32_t PyNotifier::GetOverrun() const {
  int32_t status = 0;
  int32_t overrun = HAL_GetNotifierOverrun(m_notifier, &status);
  WPILIB_CheckErrorStatus(status, "GetNotifierOverrun");
  return overrun;
}

bool PyNotifier::SetHALThreadPriority(bool realTime, int32_t priority) {
  int32_t status = 0;
  return HAL_SetNotifierThreadPriority(realTime, priority, &status);
}
