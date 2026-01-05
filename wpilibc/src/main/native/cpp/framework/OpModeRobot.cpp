// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Notifier.h"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi;

namespace {
class MonitorThread : public wpi::util::SafeThreadEvent {
 public:
  MonitorThread(int64_t modeId, wpi::util::Event& dsEvent,
                HAL_NotifierHandle notifier, std::weak_ptr<OpMode> activeOpMode)
      : m_modeId{modeId},
        m_dsEvent{dsEvent.GetHandle()},
        m_notifier{notifier},
        m_activeOpMode{std::move(activeOpMode)} {}

 private:
  void Main() override {
    // Wait for DS to disable or change modes
    WPI_EventHandle events[] = {m_dsEvent, m_stopEvent.GetHandle()};
    WPI_Handle signaledBuf[2];
    for (;;) {
      auto signaled = wpi::util::WaitForObjects(events, signaledBuf);
      if (signaled.empty()) {
        return;  // handles destroyed
      }
      for (auto signal : signaled) {
        if ((signal & 0x80000000) != 0 || signal == m_stopEvent.GetHandle()) {
          return;  // handle destroyed or transitioned
        }
      }

      // did the opmode or enable state change?
      HAL_ControlWord word;
      HAL_GetUncachedControlWord(&word);
      if (!HAL_ControlWord_IsEnabled(word) ||
          HAL_ControlWord_GetOpModeId(word) != m_modeId) {
        break;
      }
    }

    // call opmode stop
    auto opMode = m_activeOpMode.lock();
    if (opMode) {
      opMode->OpModeStop();
    }

    events[0] = m_notifier;
    int32_t status = 0;
    HAL_SetNotifierAlarm(m_notifier, 1000000, 0, false, true, &status);  // 1s
    auto signaled = wpi::util::WaitForObjects(events, signaledBuf);
    if (signaled.empty()) {
      return;  // handles destroyed
    }
    for (auto signal : signaled) {
      if ((signal & 0x80000000) != 0 || signal == m_stopEvent.GetHandle()) {
        return;  // handle destroyed or transitioned
      }
    }

    // if it hasn't transitioned after 1 second, terminate the program
    WPILIB_ReportError(err::Error, "OpMode did not exit, terminating program");
    HAL_Shutdown();
    std::exit(0);
  }

  int64_t m_modeId;
  WPI_EventHandle m_dsEvent;
  HAL_NotifierHandle m_notifier;
  std::weak_ptr<OpMode> m_activeOpMode;
};
}  // namespace

void OpModeRobotBase::StartCompetition() {
  fmt::print("********** Robot program startup complete **********\n");

  wpi::util::Event event;
  struct DSListener {
    wpi::util::Event& event;
    explicit DSListener(wpi::util::Event& event) : event{event} {
      HAL_ProvideNewDataEventHandle(event.GetHandle());
    }
    ~DSListener() { HAL_RemoveNewDataEventHandle(event.GetHandle()); }
  } listener{event};

  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  HAL_SetNotifierName(m_notifier, "OpModeRobot", &status);

  HAL_ObserveUserProgramStarting();

  int64_t lastModeId = -1;
  bool calledDriverStationConnected = false;
  std::shared_ptr<OpMode> opMode;
  WPI_EventHandle events[] = {event.GetHandle(),
                              static_cast<WPI_EventHandle>(m_notifier)};
  WPI_Handle signaledBuf[2];
  for (;;) {
    // Wait for new data from the driver station, with 50 ms timeout
    HAL_SetNotifierAlarm(m_notifier, 50000, 0, false, true, &status);
    auto signaled = wpi::util::WaitForObjects(events, signaledBuf);
    if (signaled.empty()) {
      return;  // handles destroyed
    }
    for (auto signal : signaled) {
      if ((signal & 0x80000000) != 0) {
        return;  // handle destroyed
      }
    }

    // Get the latest control word and opmode
    DriverStation::RefreshData();
    hal::ControlWord ctlWord = DriverStation::GetControlWord();

    if (!calledDriverStationConnected && ctlWord.IsDSAttached()) {
      calledDriverStationConnected = true;
      DriverStationConnected();
    }

    int64_t modeId;
    if (!ctlWord.IsDSAttached()) {
      modeId = 0;
    } else {
      modeId = ctlWord.GetOpModeId();
    }

    if (!opMode || modeId != lastModeId) {
      if (opMode) {
        // no or different opmode selected
        opMode.reset();
      }

      if (modeId == 0) {
        // no opmode selected
        NonePeriodic();
        HAL_ObserveUserProgram(ctlWord.GetValue());
        continue;
      }

      auto data = m_opModes.lookup(modeId);
      if (!data.factory) {
        WPILIB_ReportError(err::Error, "No OpMode found for mode {}", modeId);
        ctlWord.SetOpModeId(0);
        HAL_ObserveUserProgram(ctlWord.GetValue());
        continue;
      }

      // Instantiate the opmode
      fmt::print("********** Starting OpMode {} **********\n", data.name);
      opMode = data.factory();
      if (!opMode) {
        // could not construct
        ctlWord.SetOpModeId(0);
        HAL_ObserveUserProgram(ctlWord.GetValue());
        continue;
      }
      {
        std::scoped_lock lock(m_opModeMutex);
        m_activeOpMode = opMode;
      }
      lastModeId = modeId;
      // Ensure disabledPeriodic is called at least once
      opMode->DisabledPeriodic();
    }

    HAL_ObserveUserProgram(ctlWord.GetValue());

    if (ctlWord.IsEnabled()) {
      // When enabled, call the opmode run function, then close and clear
      wpi::util::SafeThreadOwner<MonitorThread> monitor;
      monitor.Start(modeId, event, static_cast<HAL_NotifierHandle>(m_notifier),
                    opMode);
      opMode->OpModeRun(modeId);
      opMode.reset();
    } else {
      // When disabled, call the DisabledPeriodic function
      opMode->DisabledPeriodic();
    }
  }
}

void OpModeRobotBase::EndCompetition() {
  m_notifier = {};
  std::shared_ptr<OpMode> opMode;
  {
    std::scoped_lock lock(m_opModeMutex);
    opMode = m_activeOpMode.lock();
  }
  if (opMode) {
    opMode->OpModeStop();
  }
}

void OpModeRobotBase::AddOpModeFactory(
    OpModeFactory factory, RobotMode mode, std::string_view name,
    std::string_view group, std::string_view description,
    const wpi::util::Color& textColor,
    const wpi::util::Color& backgroundColor) {
  int64_t id = DriverStation::AddOpMode(mode, name, group, description,
                                        textColor, backgroundColor);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description) {
  int64_t id = DriverStation::AddOpMode(mode, name, group, description);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::RemoveOpMode(RobotMode mode, std::string_view name) {
  int64_t id = DriverStation::RemoveOpMode(mode, name);
  if (id != 0) {
    m_opModes.erase(id);
  }
}

void OpModeRobotBase::PublishOpModes() {
  DriverStation::PublishOpModes();
}

void OpModeRobotBase::ClearOpModes() {
  DriverStation::ClearOpModes();
  m_opModes.clear();
}
