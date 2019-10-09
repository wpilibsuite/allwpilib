/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/livewindow/LiveWindow.h"

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/mutex.h>

#include "frc/commands/Scheduler.h"
#include "frc/smartdashboard/SendableBuilderImpl.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

using wpi::Twine;

struct LiveWindow::Impl {
  Impl();

  struct Component {
    SendableBuilderImpl builder;
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
  m_impl->registry.ForeachLiveWindow(
      m_impl->dataHandle, [&](Sendable*, wpi::StringRef, wpi::StringRef,
                              Sendable*, std::shared_ptr<void>& data) {
        if (!data) data = std::make_shared<Impl::Component>();
        std::static_pointer_cast<Impl::Component>(data)->telemetryEnabled =
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
  Scheduler* scheduler = Scheduler::GetInstance();
  m_impl->startLiveWindow = enabled;
  m_impl->liveWindowEnabled = enabled;
  // Force table generation now to make sure everything is defined
  UpdateValuesUnsafe();
  if (enabled) {
    scheduler->SetEnabled(false);
    scheduler->RemoveAll();
  } else {
    m_impl->registry.ForeachLiveWindow(
        m_impl->dataHandle, [&](Sendable*, wpi::StringRef, wpi::StringRef,
                                Sendable*, std::shared_ptr<void>& data) {
          if (data)
            std::static_pointer_cast<Impl::Component>(data)
                ->builder.StopLiveWindowMode();
        });
    scheduler->SetEnabled(true);
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

  m_impl->registry.ForeachLiveWindow(
      m_impl->dataHandle,
      [&](Sendable* sendable, wpi::StringRef name, wpi::StringRef subsystem,
          Sendable* parent, std::shared_ptr<void>& data) {
        if (!sendable || parent) return;

        if (!data) data = std::make_shared<Impl::Component>();

        auto& comp = *std::static_pointer_cast<Impl::Component>(data);

        if (!m_impl->liveWindowEnabled && !comp.telemetryEnabled) return;

        if (comp.firstTime) {
          // By holding off creating the NetworkTable entries, it allows the
          // components to be redefined. This allows default sensor and actuator
          // values to be created that are replaced with the custom names from
          // users calling setName.
          if (name.empty()) return;
          auto ssTable = m_impl->liveWindowTable->GetSubTable(subsystem);
          std::shared_ptr<NetworkTable> table;
          // Treat name==subsystem as top level of subsystem
          if (name == subsystem)
            table = ssTable;
          else
            table = ssTable->GetSubTable(name);
          table->GetEntry(".name").SetString(name);
          comp.builder.SetTable(table);
          sendable->InitSendable(comp.builder);
          ssTable->GetEntry(".type").SetString("LW Subsystem");

          comp.firstTime = false;
        }

        if (m_impl->startLiveWindow) comp.builder.StartLiveWindowMode();
        comp.builder.UpdateTable();
      });

  m_impl->startLiveWindow = false;
}

LiveWindow::LiveWindow() : m_impl(new Impl) {}
