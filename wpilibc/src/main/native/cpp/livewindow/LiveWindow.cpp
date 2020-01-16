/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/livewindow/LiveWindow.h"

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/mutex.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableBuilderImpl.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

using wpi::Twine;

struct LiveWindow::Impl {
  Impl();

  struct Component {
    bool firstTime = true;
    bool telemetryEnabled = true;
  };

  wpi::mutex mutex;

  SendableRegistry& registry;
  int dataHandle;

  std::shared_ptr<nt::NetworkTable> liveWindowTable;
  std::shared_ptr<nt::NetworkTable> statusTable;
  nt::NetworkTableEntry enabledEntry;

  bool startLiveWindow = false;
  bool liveWindowEnabled = false;
  bool telemetryEnabled = true;

  std::shared_ptr<Component> GetOrAdd(Sendable* sendable);
};

LiveWindow::Impl::Impl()
    : registry(SendableRegistry::GetInstance()),
      dataHandle(registry.GetDataHandle()),
      liveWindowTable(
          nt::NetworkTableInstance::GetDefault().GetTable("LiveWindow")) {
  statusTable = liveWindowTable->GetSubTable(".status");
  enabledEntry = statusTable->GetEntry("LW Enabled");
}

std::shared_ptr<LiveWindow::Impl::Component> LiveWindow::Impl::GetOrAdd(
    Sendable* sendable) {
  auto data = std::static_pointer_cast<Component>(
      registry.GetData(sendable, dataHandle));
  if (!data) {
    data = std::make_shared<Component>();
    registry.SetData(sendable, dataHandle, data);
  }
  return data;
}

LiveWindow* LiveWindow::GetInstance() {
  static LiveWindow instance;
  return &instance;
}

void LiveWindow::EnableTelemetry(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  // Re-enable global setting in case DisableAllTelemetry() was called.
  m_impl->telemetryEnabled = true;
  m_impl->GetOrAdd(sendable)->telemetryEnabled = true;
}

void LiveWindow::DisableTelemetry(Sendable* sendable) {
  std::scoped_lock lock(m_impl->mutex);
  m_impl->GetOrAdd(sendable)->telemetryEnabled = false;
}

void LiveWindow::DisableAllTelemetry() {
  std::scoped_lock lock(m_impl->mutex);
  m_impl->telemetryEnabled = false;
  m_impl->registry.ForeachLiveWindow(m_impl->dataHandle, [&](auto& cbdata) {
    if (!cbdata.data) cbdata.data = std::make_shared<Impl::Component>();
    std::static_pointer_cast<Impl::Component>(cbdata.data)->telemetryEnabled =
        false;
  });
}

bool LiveWindow::IsEnabled() const {
  std::scoped_lock lock(m_impl->mutex);
  return m_impl->liveWindowEnabled;
}

void LiveWindow::SetEnabled(bool enabled) {
  std::scoped_lock lock(m_impl->mutex);
  if (m_impl->liveWindowEnabled == enabled) return;
  m_impl->startLiveWindow = enabled;
  m_impl->liveWindowEnabled = enabled;
  // Force table generation now to make sure everything is defined
  UpdateValuesUnsafe();
  if (enabled) {
    if (this->enabled) this->enabled();
  } else {
    m_impl->registry.ForeachLiveWindow(m_impl->dataHandle, [&](auto& cbdata) {
      cbdata.builder.StopLiveWindowMode();
    });
    if (this->disabled) this->disabled();
  }
  m_impl->enabledEntry.SetBoolean(enabled);
}

void LiveWindow::UpdateValues() {
  std::scoped_lock lock(m_impl->mutex);
  UpdateValuesUnsafe();
}

void LiveWindow::UpdateValuesUnsafe() {
  // Only do this if either LiveWindow mode or telemetry is enabled.
  if (!m_impl->liveWindowEnabled && !m_impl->telemetryEnabled) return;

  m_impl->registry.ForeachLiveWindow(m_impl->dataHandle, [&](auto& cbdata) {
    if (!cbdata.sendable || cbdata.parent) return;

    if (!cbdata.data) cbdata.data = std::make_shared<Impl::Component>();

    auto& comp = *std::static_pointer_cast<Impl::Component>(cbdata.data);

    if (!m_impl->liveWindowEnabled && !comp.telemetryEnabled) return;

    if (comp.firstTime) {
      // By holding off creating the NetworkTable entries, it allows the
      // components to be redefined. This allows default sensor and actuator
      // values to be created that are replaced with the custom names from
      // users calling setName.
      if (cbdata.name.empty()) return;
      auto ssTable = m_impl->liveWindowTable->GetSubTable(cbdata.subsystem);
      std::shared_ptr<NetworkTable> table;
      // Treat name==subsystem as top level of subsystem
      if (cbdata.name == cbdata.subsystem)
        table = ssTable;
      else
        table = ssTable->GetSubTable(cbdata.name);
      table->GetEntry(".name").SetString(cbdata.name);
      cbdata.builder.SetTable(table);
      cbdata.sendable->InitSendable(cbdata.builder);
      ssTable->GetEntry(".type").SetString("LW Subsystem");

      comp.firstTime = false;
    }

    if (m_impl->startLiveWindow) cbdata.builder.StartLiveWindowMode();
    cbdata.builder.UpdateTable();
  });

  m_impl->startLiveWindow = false;
}

LiveWindow::LiveWindow() : m_impl(new Impl) {}
