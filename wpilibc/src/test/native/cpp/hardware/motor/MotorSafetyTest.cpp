// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/motor/MotorSafety.hpp"

#include <stdint.h>

#include <chrono>
#include <condition_variable>
#include <string>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/cpp/MrcLibDs.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::impl {
void ResetMotorSafety();
}  // namespace wpi::impl

namespace {

class MockMotorSafety : public wpi::MotorSafety {
 public:
  void StopMotor() override {}
  std::string GetDescription() const override { return "MockMotorSafety"; }
};

// Forwards to the default sim driver station, recording when the motor safety
// thread registers and removes its new data event handle. Removal is delayed
// so that it can't finish before ResetMotorSafety() returns unless
// ResetMotorSafety() waits for the thread to exit.
class RecordingDs : public wpi::hal::MrcLibDs {
 public:
  explicit RecordingDs(wpi::hal::MrcLibDs* ds) : m_ds{ds} {}

  bool WaitForProvided() {
    std::unique_lock lock{m_mutex};
    return m_cond.wait_for(lock, std::chrono::seconds{5},
                           [this] { return m_provided; });
  }

  bool IsRemoved() {
    std::scoped_lock lock{m_mutex};
    return m_removed;
  }

  bool WaitForRemoved() {
    std::unique_lock lock{m_mutex};
    return m_cond.wait_for(lock, std::chrono::seconds{5},
                           [this] { return m_removed; });
  }

  int32_t sendError(bool isError, int32_t errorCode,
                    const struct WPI_String* details,
                    const struct WPI_String* location,
                    const struct WPI_String* callStack,
                    bool printMsg) override {
    return m_ds->sendError(isError, errorCode, details, location, callStack,
                           printMsg);
  }
  int32_t sendConsoleLine(const struct WPI_String* line) override {
    return m_ds->sendConsoleLine(line);
  }
  int32_t sendProgramCrash(const struct WPI_String* details,
                           const struct WPI_String* location,
                           const struct WPI_String* callStack) override {
    return m_ds->sendProgramCrash(details, location, callStack);
  }
  int32_t getControlWord(HAL_ControlWord* controlWord) override {
    return m_ds->getControlWord(controlWord);
  }
  int32_t getUncachedControlWord(HAL_ControlWord* controlWord) override {
    return m_ds->getUncachedControlWord(controlWord);
  }
  int32_t setOpModeOptions(const struct HAL_OpModeOption* options,
                           int32_t count) override {
    return m_ds->setOpModeOptions(options, count);
  }
  int32_t getAllianceStation(HAL_AllianceStationID* allianceStation) override {
    return m_ds->getAllianceStation(allianceStation);
  }
  int32_t getJoystickAxes(int32_t joystickNum,
                          HAL_JoystickAxes* axes) override {
    return m_ds->getJoystickAxes(joystickNum, axes);
  }
  int32_t getJoystickPOVs(int32_t joystickNum,
                          HAL_JoystickPOVs* povs) override {
    return m_ds->getJoystickPOVs(joystickNum, povs);
  }
  int32_t getJoystickButtons(int32_t joystickNum,
                             HAL_JoystickButtons* buttons) override {
    return m_ds->getJoystickButtons(joystickNum, buttons);
  }
  int32_t getJoystickTouchpads(int32_t joystickNum,
                               HAL_JoystickTouchpads* touchpads) override {
    return m_ds->getJoystickTouchpads(joystickNum, touchpads);
  }
  int32_t getAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                             HAL_JoystickPOVs* povs,
                             HAL_JoystickButtons* buttons,
                             HAL_JoystickTouchpads* touchpads) override {
    return m_ds->getAllJoystickData(joystickNum, axes, povs, buttons,
                                    touchpads);
  }
  int32_t getJoystickDescriptor(int32_t joystickNum,
                                HAL_JoystickDescriptor* desc) override {
    return m_ds->getJoystickDescriptor(joystickNum, desc);
  }
  int32_t getGameData(HAL_GameData* gameData) override {
    return m_ds->getGameData(gameData);
  }
  int32_t setJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                            int32_t rightRumble, int32_t leftTriggerRumble,
                            int32_t rightTriggerRumble) override {
    return m_ds->setJoystickRumble(joystickNum, leftRumble, rightRumble,
                                   leftTriggerRumble, rightTriggerRumble);
  }
  int32_t setJoystickLeds(int32_t joystickNum, int32_t leds) override {
    return m_ds->setJoystickLeds(joystickNum, leds);
  }
  int32_t getMatchTime(double* matchTime) override {
    return m_ds->getMatchTime(matchTime);
  }
  int32_t getMatchInfo(HAL_MatchInfo* info) override {
    return m_ds->getMatchInfo(info);
  }
  int32_t getOutputsEnabled(bool* outputsEnabled) override {
    return m_ds->getOutputsEnabled(outputsEnabled);
  }
  int32_t refreshDSData(bool* wasRefreshed) override {
    return m_ds->refreshDSData(wasRefreshed);
  }
  void provideNewDataEventHandle(WPI_EventHandle handle) override {
    m_ds->provideNewDataEventHandle(handle);
    std::scoped_lock lock{m_mutex};
    m_provided = true;
    m_cond.notify_all();
  }
  void removeNewDataEventHandle(WPI_EventHandle handle) override {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    m_ds->removeNewDataEventHandle(handle);
    std::scoped_lock lock{m_mutex};
    m_removed = true;
    m_cond.notify_all();
  }
  int32_t observeUserProgramStarting() override {
    return m_ds->observeUserProgramStarting();
  }
  int32_t observeUserProgram(HAL_ControlWord controlWord) override {
    return m_ds->observeUserProgram(controlWord);
  }
  int32_t getSystemTimeValid(bool* systemTimeValid) override {
    return m_ds->getSystemTimeValid(systemTimeValid);
  }
  int32_t writeDisplayAnsi(const struct WPI_String* line) override {
    return m_ds->writeDisplayAnsi(line);
  }

 private:
  wpi::hal::MrcLibDs* m_ds;
  wpi::util::mutex m_mutex;
  std::condition_variable_any m_cond;
  bool m_provided = false;
  bool m_removed = false;
};

}  // namespace

TEST_CASE("MotorSafetyTest ResetJoinsThread", "[wpilibc]") {
  // Make sure no motor safety thread is using the driver station while it's
  // swapped out
  wpi::impl::ResetMotorSafety();
  RecordingDs ds{wpi::hal::GetDefaultDriverStationImpl()};
  wpi::hal::ForceDsInstance(&ds);

  {
    MockMotorSafety motor;
    REQUIRE(ds.WaitForProvided());
  }

  // The thread must have fully exited, including removing its event handle,
  // by the time ResetMotorSafety() returns
  wpi::impl::ResetMotorSafety();
  bool removed = ds.IsRemoved();

  // If the thread wasn't joined, let it finish before ds goes out of scope
  ds.WaitForRemoved();
  wpi::hal::ForceDsInstance(wpi::hal::GetDefaultDriverStationImpl());
  CHECK(removed);
}
