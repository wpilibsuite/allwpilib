#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::tunables::python {

using ReportWarningCallback =
    pybind11::typing::Callable<void(std::string_view)>;
using RemovableValue =
    pybind11::typing::Union<pybind11::str, pybind11::object>;

class UpdateMutexContext final {
 public:
  UpdateMutexContext();
  ~UpdateMutexContext();

  UpdateMutexContext(const UpdateMutexContext&) = delete;
  UpdateMutexContext& operator=(const UpdateMutexContext&) = delete;
  UpdateMutexContext(UpdateMutexContext&&) = delete;
  UpdateMutexContext& operator=(UpdateMutexContext&&) = delete;

  void Enter(pybind11::object self);
  void Exit();

 private:
  enum class State { kIdle, kEntering, kEntered };

  std::mutex m_stateMutex;
  State m_state = State::kIdle;
  std::unique_lock<wpi::util::recursive_mutex> m_lock;
  std::thread::id m_ownerThread;
  pybind11::object m_retainedSelf;
};

void SetReportWarning(std::optional<ReportWarningCallback> callback);

void RegisterBackend(std::string_view prefix,
                     std::shared_ptr<wpi::tunables::TunableBackend> backend);

wpi::tunables::TunableTable GetRegistryTable(std::string_view path);

std::string NormalizeName(std::string_view path);

void Remove(RemovableValue value);

void Update();

std::unique_ptr<UpdateMutexContext> MakeUpdateMutexContext();

void Reset();

}  // namespace wpi::tunables::python
