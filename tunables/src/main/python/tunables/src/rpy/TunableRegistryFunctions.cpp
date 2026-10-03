#include "TunableRegistryFunctions.h"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

#include "TunableStorage.h"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

namespace py = pybind11;

namespace wpi::tunables::python {

UpdateMutexContext::UpdateMutexContext() = default;

UpdateMutexContext::~UpdateMutexContext() {
  std::unique_lock<wpi::util::recursive_mutex> ownedLock;
  {
    std::scoped_lock stateLock{m_stateMutex};
    if (m_state != State::kEntered) {
      return;
    }

    if (m_ownerThread == std::this_thread::get_id()) {
      ownedLock = std::move(m_lock);
    } else {
      // Python entry retains the wrapper, so only defensive C++ misuse can
      // reach this path. Abandon the lock rather than unlocking a recursive
      // mutex from a nonowner thread, which would be undefined behavior.
      m_lock.release();
    }
    m_ownerThread = {};
    m_state = State::kIdle;
  }

  if (ownedLock.owns_lock()) {
    ownedLock.unlock();
  }
}

void UpdateMutexContext::Enter(py::object self) {
  {
    std::scoped_lock stateLock{m_stateMutex};
    if (m_state != State::kIdle) {
      throw std::runtime_error("update mutex context is already entered");
    }
    m_state = State::kEntering;
  }

  std::unique_lock lock{wpi::tunables::TunableRegistry::GetUpdateMutex(),
                        std::defer_lock};
  try {
    {
      py::gil_scoped_release release;
      lock.lock();
    }

    std::scoped_lock stateLock{m_stateMutex};
    m_lock = std::move(lock);
    m_ownerThread = std::this_thread::get_id();
    m_retainedSelf = std::move(self);
    m_state = State::kEntered;
  } catch (...) {
    std::scoped_lock stateLock{m_stateMutex};
    if (m_state == State::kEntering) {
      m_state = State::kIdle;
    }
    throw;
  }
}

void UpdateMutexContext::Exit() {
  std::unique_lock<wpi::util::recursive_mutex> ownedLock;
  py::object retainedSelf;
  {
    std::scoped_lock stateLock{m_stateMutex};
    if (m_state == State::kEntering) {
      throw std::runtime_error("update mutex context is being entered");
    }
    if (m_state != State::kEntered) {
      throw std::runtime_error("update mutex context is not entered");
    }
    if (m_ownerThread != std::this_thread::get_id()) {
      throw std::runtime_error(
          "update mutex context must exit on its owning thread");
    }

    ownedLock = std::move(m_lock);
    retainedSelf = std::move(m_retainedSelf);
    m_ownerThread = {};
    m_state = State::kIdle;
  }

  ownedLock.unlock();
}

void SetReportWarning(std::optional<ReportWarningCallback> callback) {
  if (!callback) {
    wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
    return;
  }

  auto callbackOwner = std::shared_ptr<ReportWarningCallback>{
      new ReportWarningCallback{std::move(*callback)},
      [](ReportWarningCallback* callback) {
        py::gil_scoped_acquire gil;
        delete callback;
      }};
  wpi::tunables::TunableRegistry::SetReportWarning(
      [callbackOwner](std::string_view message) {
        py::gil_scoped_acquire gil;
        (*callbackOwner)(message);
      });
}

void RegisterBackend(std::string_view prefix,
                     std::shared_ptr<wpi::tunables::TunableBackend> backend) {
  std::string prefixString{prefix};
  py::gil_scoped_release release;
  wpi::tunables::TunableRegistry::RegisterBackend(prefixString,
                                                  std::move(backend));
}

wpi::tunables::TunableTable GetRegistryTable(std::string_view path) {
  return wpi::tunables::GetTable(path);
}

std::string NormalizeName(std::string_view path) {
  return NormalizePath(path);
}

void Remove(RemovableValue value) {
  if (py::isinstance<py::str>(value)) {
    RemovePath(value.cast<std::string>());
  } else {
    RemoveValue(value);
  }
}

void Update() {
  py::gil_scoped_release release;
  wpi::tunables::TunableRegistry::Update();
}

std::unique_ptr<UpdateMutexContext> MakeUpdateMutexContext() {
  return std::make_unique<UpdateMutexContext>();
}

void Reset() {
  ClearValues();
  {
    py::gil_scoped_release release;
    wpi::tunables::TunableRegistry::Reset();
  }
  RegisterPreUpdateCallback();
}

}  // namespace wpi::tunables::python
