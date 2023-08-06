// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/livewindow/LiveWindow.h"

#include <fmt/format.h>
#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>
#include <wpi/DenseMap.h>
#include <wpi/UidVector.h>
#include <wpi/mutex.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/smartdashboard/SendableBuilderImpl.h"

using namespace frc;

namespace {
struct Component {
  wpi::Sendable* sendable = nullptr;
  std::unique_ptr<wpi::SendableBuilder> builder;
  std::string name;
  std::string subsystem = "Ungrouped";
  wpi::Sendable* parent = nullptr;
  bool firstTime = true;
  bool telemetryEnabled = false;
  nt::StringPublisher namePub;
  nt::StringPublisher typePub;

  void SetName(std::string_view moduleType, int channel) {
    name = fmt::format("{}[{}]", moduleType, channel);
  }

  void SetName(std::string_view moduleType, int moduleNumber, int channel) {
    name = fmt::format("{}[{},{}]", moduleType, moduleNumber, channel);
  }
};

struct Instance {
  Instance() {
    wpi::SendableRegistry::SetLiveWindowBuilderFactory(
        [] { return std::make_unique<SendableBuilderImpl>(); });
    enabledPub.Set(false);
  }

  wpi::mutex mutex;

  int dataHandle = wpi::SendableRegistry::GetDataHandle();

  std::shared_ptr<nt::NetworkTable> liveWindowTable =
      nt::NetworkTableInstance::GetDefault().GetTable("LiveWindow");
  std::shared_ptr<nt::NetworkTable> statusTable =
      liveWindowTable->GetSubTable(".status");
  nt::BooleanPublisher enabledPub =
      statusTable->GetBooleanTopic("LW Enabled").Publish();

  wpi::UidVector<std::unique_ptr<Component>, 32> components;
  wpi::DenseMap<void*, LiveWindow::UID> componentMap;
  bool startLiveWindow = false;
  bool liveWindowEnabled = false;
  bool telemetryEnabled = false;

  std::function<void()> enabled;
  std::function<void()> disabled;

  Component& GetOrAdd(void* sendable);
};
}  // namespace

static std::unique_ptr<Instance>& GetInstanceHolder() {
  static std::unique_ptr<Instance> instance = std::make_unique<Instance>();
  return instance;
}

static Instance& GetInstance() {
  return *GetInstanceHolder();
}

#ifndef __FRC_ROBORIO__
namespace frc::impl {
void ResetLiveWindow() {
  std::make_unique<Instance>().swap(GetInstanceHolder());
}
}  // namespace frc::impl
#endif

Component& Instance::GetOrAdd(void* sendable) {
  LiveWindow::UID& compUid = componentMap[sendable];
  if (compUid == 0) {
    compUid = components.emplace_back(std::make_unique<Component>()) + 1;
  }
  return *components[compUid - 1];
}

void LiveWindow::AddLW(wpi::Sendable* sendable, std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
}

void LiveWindow::AddLW(wpi::Sendable* sendable, std::string_view moduleType,
                       int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, channel);
}

void LiveWindow::AddLW(wpi::Sendable* sendable, std::string_view moduleType,
                       int moduleNumber, int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.SetName(moduleType, moduleNumber, channel);
}

void LiveWindow::AddLW(wpi::Sendable* sendable, std::string_view subsystem,
                       std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(sendable);
  comp.sendable = sendable;
  comp.name = name;
  comp.subsystem = subsystem;
}

void LiveWindow::AddChild(wpi::Sendable* parent, wpi::Sendable* child) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(child);
  comp.parent = parent;
}

void LiveWindow::AddChild(wpi::Sendable* parent, void* child) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto& comp = inst.GetOrAdd(child);
  comp.parent = parent;
}

bool LiveWindow::Remove(wpi::Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end()) {
    return false;
  }
  UID compUid = it->getSecond();
  inst.components.erase(compUid - 1);
  inst.componentMap.erase(it);
  // update any parent pointers
  for (auto&& comp : inst.components) {
    if (comp->parent == sendable) {
      comp->parent = nullptr;
    }
  }
  return true;
}

void LiveWindow::Move(wpi::Sendable* to, wpi::Sendable* from) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(from);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  UID compUid = it->getSecond();
  inst.componentMap.erase(it);
  inst.componentMap[to] = compUid;
  auto& comp = *inst.components[compUid - 1];
  comp.sendable = to;
  if (comp.builder && comp.builder->IsPublished()) {
    // rebuild builder, as lambda captures can point to "from"
    comp.builder->ClearProperties();
    to->InitSendable(*comp.builder);
  }
  // update any parent pointers
  for (auto&& comp : inst.components) {
    if (comp->parent == from) {
      comp->parent = to;
    }
  }
}

bool LiveWindow::Contains(const wpi::Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  return inst.componentMap.count(sendable) != 0;
}

std::string LiveWindow::GetName(const wpi::Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return {};
  }
  return inst.components[it->getSecond() - 1]->name;
}

void LiveWindow::SetName(wpi::Sendable* sendable, std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->name = name;
}

void LiveWindow::SetName(wpi::Sendable* sendable, std::string_view moduleType,
                         int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->SetName(moduleType, channel);
}

void LiveWindow::SetName(wpi::Sendable* sendable, std::string_view moduleType,
                         int moduleNumber, int channel) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->SetName(moduleType, moduleNumber,
                                                channel);
}

void LiveWindow::SetName(wpi::Sendable* sendable, std::string_view subsystem,
                         std::string_view name) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  auto& comp = *inst.components[it->getSecond() - 1];
  comp.name = name;
  comp.subsystem = subsystem;
}

std::string LiveWindow::GetSubsystem(const wpi::Sendable* sendable) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return {};
  }
  return inst.components[it->getSecond() - 1]->subsystem;
}

void LiveWindow::SetSubsystem(wpi::Sendable* sendable,
                              std::string_view subsystem) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.mutex);
  auto it = inst.componentMap.find(sendable);
  if (it == inst.componentMap.end() || !inst.components[it->getSecond() - 1]) {
    return;
  }
  inst.components[it->getSecond() - 1]->subsystem = subsystem;
}

void LiveWindow::SetEnabledCallback(std::function<void()> func) {
  ::GetInstance().enabled = func;
}

void LiveWindow::SetDisabledCallback(std::function<void()> func) {
  ::GetInstance().disabled = func;
}

void LiveWindow::EnableTelemetry(wpi::Sendable* sendable) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  // Re-enable global setting in case DisableAllTelemetry() was called.
  inst.telemetryEnabled = true;
  inst.GetOrAdd(sendable).telemetryEnabled = true;
}

void LiveWindow::DisableTelemetry(wpi::Sendable* sendable) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.GetOrAdd(sendable).telemetryEnabled = false;
}

void LiveWindow::DisableAllTelemetry() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.telemetryEnabled = false;
  for (auto& comp : inst.components) {
    comp->telemetryEnabled = false;
  }
}

void LiveWindow::EnableAllTelemetry() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.telemetryEnabled = true;
  for (auto& comp : inst.components) {
    comp->telemetryEnabled = true;
  }
}

bool LiveWindow::IsEnabled() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  return inst.liveWindowEnabled;
}

void LiveWindow::SetEnabled(bool enabled) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  if (inst.liveWindowEnabled == enabled) {
    return;
  }
  inst.startLiveWindow = enabled;
  inst.liveWindowEnabled = enabled;
  // Force table generation now to make sure everything is defined
  UpdateValuesUnsafe();
  if (enabled) {
    if (inst.enabled) {
      inst.enabled();
    }
  } else {
    for (auto& comp : inst.components) {
      static_cast<SendableBuilderImpl*>(comp->builder.get())
          ->StopLiveWindowMode();
    }
    if (inst.disabled) {
      inst.disabled();
    }
  }
  inst.enabledPub.Set(enabled);
}

void LiveWindow::UpdateValues() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  UpdateValuesUnsafe();
}

void LiveWindow::UpdateValuesUnsafe() {
  auto& inst = ::GetInstance();
  // Only do this if either LiveWindow mode or telemetry is enabled.
  if (!inst.liveWindowEnabled && !inst.telemetryEnabled) {
    return;
  }

  for (auto& comp : inst.components) {
    if (!comp->sendable || comp->parent) {
      return;
    }
    if (!inst.liveWindowEnabled && !comp->telemetryEnabled) {
      return;
    }

    if (comp->firstTime) {
      // By holding off creating the NetworkTable entries, it allows the
      // components to be redefined. This allows default sensor and actuator
      // values to be created that are replaced with the custom names from
      // users calling setName.
      if (comp->name.empty()) {
        return;
      }
      auto ssTable = inst.liveWindowTable->GetSubTable(comp->subsystem);
      std::shared_ptr<nt::NetworkTable> table;
      // Treat name==subsystem as top level of subsystem
      if (comp->name == comp->subsystem) {
        table = ssTable;
      } else {
        table = ssTable->GetSubTable(comp->name);
      }
      comp->namePub = nt::StringTopic{table->GetTopic(".name")}.Publish();
      comp->namePub.Set(comp->name);
      static_cast<SendableBuilderImpl*>(comp->builder.get())->SetTable(table);
      comp->sendable->InitSendable(*comp->builder);
      comp->typePub = nt::StringTopic{ssTable->GetTopic(".type")}.Publish();
      comp->typePub.Set("LW Subsystem");

      comp->firstTime = false;
    }

    if (inst.startLiveWindow) {
      static_cast<SendableBuilderImpl*>(comp->builder.get())
          ->StartLiveWindowMode();
    }
    comp->builder->Update();
  }

  inst.startLiveWindow = false;
}
