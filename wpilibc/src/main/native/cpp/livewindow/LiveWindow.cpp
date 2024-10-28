// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/livewindow/LiveWindow.h"

#include <memory>

#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>
#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/smartdashboard/SendableBuilderImpl.h"

using namespace frc;

static constexpr std::string_view kSmartDashboardType = "LW Subsystem";

namespace {
struct Component {
  bool firstTime = true;
  bool telemetryEnabled = false;
  nt::StringPublisher namePub;
  nt::StringPublisher typePub;
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

  bool startLiveWindow = false;
  bool liveWindowEnabled = false;
  bool telemetryEnabled = false;

  std::function<void()> enabled;
  std::function<void()> disabled;

  std::shared_ptr<Component> GetOrAdd(wpi::Sendable* sendable);
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

std::shared_ptr<Component> Instance::GetOrAdd(wpi::Sendable* sendable) {
  auto data = std::static_pointer_cast<Component>(
      wpi::SendableRegistry::GetData(sendable, dataHandle));
  if (!data) {
    data = std::make_shared<Component>();
    wpi::SendableRegistry::SetData(sendable, dataHandle, data);
  }
  return data;
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
  inst.GetOrAdd(sendable)->telemetryEnabled = true;
}

void LiveWindow::DisableTelemetry(wpi::Sendable* sendable) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.GetOrAdd(sendable)->telemetryEnabled = false;
}

void LiveWindow::DisableAllTelemetry() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.telemetryEnabled = false;
  wpi::SendableRegistry::ForeachLiveWindow(inst.dataHandle, [&](auto& cbdata) {
    if (!cbdata.data) {
      cbdata.data = std::make_shared<Component>();
    }
    std::static_pointer_cast<Component>(cbdata.data)->telemetryEnabled = false;
  });
}

void LiveWindow::EnableAllTelemetry() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.mutex);
  inst.telemetryEnabled = true;
  wpi::SendableRegistry::ForeachLiveWindow(inst.dataHandle, [&](auto& cbdata) {
    if (!cbdata.data) {
      cbdata.data = std::make_shared<Component>();
    }
    std::static_pointer_cast<Component>(cbdata.data)->telemetryEnabled = true;
  });
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
    wpi::SendableRegistry::ForeachLiveWindow(
        inst.dataHandle, [&](auto& cbdata) {
          static_cast<SendableBuilderImpl&>(cbdata.builder)
              .StopLiveWindowMode();
        });
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

  wpi::SendableRegistry::ForeachLiveWindow(inst.dataHandle, [&](auto& cbdata) {
    if (!cbdata.sendable || cbdata.parent) {
      return;
    }

    if (!cbdata.data) {
      cbdata.data = std::make_shared<Component>();
    }

    auto& comp = *std::static_pointer_cast<Component>(cbdata.data);

    if (!inst.liveWindowEnabled && !comp.telemetryEnabled) {
      return;
    }

    if (comp.firstTime) {
      // By holding off creating the NetworkTable entries, it allows the
      // components to be redefined. This allows default sensor and actuator
      // values to be created that are replaced with the custom names from
      // users calling setName.
      if (cbdata.name.empty()) {
        return;
      }
      auto ssTable = inst.liveWindowTable->GetSubTable(cbdata.subsystem);
      std::shared_ptr<nt::NetworkTable> table;
      // Treat name==subsystem as top level of subsystem
      if (cbdata.name == cbdata.subsystem) {
        table = ssTable;
      } else {
        table = ssTable->GetSubTable(cbdata.name);
      }
      comp.namePub = nt::StringTopic{table->GetTopic(".name")}.Publish();
      comp.namePub.Set(cbdata.name);
      static_cast<SendableBuilderImpl&>(cbdata.builder).SetTable(table);
      cbdata.sendable->InitSendable(cbdata.builder);
      comp.typePub = nt::StringTopic{ssTable->GetTopic(".type")}.PublishEx(
          nt::StringTopic::kTypeString,
          {{"SmartDashboard", kSmartDashboardType}});
      comp.typePub.Set(kSmartDashboardType);

      comp.firstTime = false;
    }

    if (inst.startLiveWindow) {
      static_cast<SendableBuilderImpl&>(cbdata.builder).StartLiveWindowMode();
    }
    cbdata.builder.Update();
  });

  inst.startLiveWindow = false;
}
