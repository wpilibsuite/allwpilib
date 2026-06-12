// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DriverStation.h"

#ifdef __APPLE__
#include <pthread.h>
#endif

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <fmt/format.h>

#include "HALInitializer.hpp"
#include "mockdata/DriverStationDataInternal.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/cpp/MrcLibDs.hpp"
#include "wpi/hal/monotonic_clock.hpp"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

using namespace wpi::hal;

static constexpr int kJoystickPorts = 6;

namespace {
struct JoystickDataCache {
  JoystickDataCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();

  HAL_JoystickAxes axes[kJoystickPorts];
  HAL_JoystickPOVs povs[kJoystickPorts];
  HAL_JoystickButtons buttons[kJoystickPorts];
  HAL_JoystickTouchpads touchpads[kJoystickPorts];
  HAL_AllianceStationID allianceStation;
  double matchTime;
  HAL_ControlWord controlWord;
  HAL_GameData gameData;
};
static_assert(std::is_standard_layout_v<JoystickDataCache>);
// static_assert(std::is_trivial_v<JoystickDataCache>);

static std::atomic_bool gShutdown{false};
}  // namespace

void JoystickDataCache::Update() {
  for (int i = 0; i < kJoystickPorts; i++) {
    SimDriverStationData->GetJoystickAxes(i, &axes[i]);
    SimDriverStationData->GetJoystickPOVs(i, &povs[i]);
    SimDriverStationData->GetJoystickButtons(i, &buttons[i]);
    SimDriverStationData->GetJoystickTouchpads(i, &touchpads[i]);
  }
  allianceStation = SimDriverStationData->allianceStationId;
  matchTime = SimDriverStationData->matchTime;

  controlWord = HAL_MakeControlWord(
      SimDriverStationData->opMode, SimDriverStationData->robotMode,
      SimDriverStationData->enabled, SimDriverStationData->eStop,
      SimDriverStationData->fmsAttached, SimDriverStationData->dsAttached);
  SimDriverStationData->GetGameData(&gameData);
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_MAX_JOYSTICKS) \
    return HAL_PARAMETER_OUT_OF_RANGE;

namespace {
struct TcpCache {
  TcpCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();
  void CloneTo(TcpCache* other) { std::memcpy(other, this, sizeof(*this)); }

  HAL_MatchInfo matchInfo;
  HAL_JoystickDescriptor descriptors[HAL_MAX_JOYSTICKS];
};
static_assert(std::is_standard_layout_v<TcpCache>);
}  // namespace

void TcpCache::Update() {
  SimDriverStationData->GetMatchInfo(&matchInfo);

  for (int i = 0; i < HAL_MAX_JOYSTICKS; i++) {
    SimDriverStationData->GetJoystickDescriptor(i, &descriptors[i]);
  }
}

class MrcLibDsSimImpl : public MrcLibDs {
 public:
  MrcLibDsSimImpl();
  ~MrcLibDsSimImpl() override { gShutdown = true; }
  int32_t sendError(bool isError, int32_t errorCode,
                    const struct WPI_String* details,
                    const struct WPI_String* location,
                    const struct WPI_String* callStack, bool printMsg) override;

  int32_t sendConsoleLine(const struct WPI_String* line) override;

  int32_t sendProgramCrash(const struct WPI_String* details,
                           const struct WPI_String* location,
                           const struct WPI_String* callStack) override;

  int32_t getControlWord(HAL_ControlWord* controlWord) override;

  int32_t getUncachedControlWord(HAL_ControlWord* controlWord) override;

  int32_t setOpModeOptions(const struct HAL_OpModeOption* options,
                           int32_t count) override;

  int32_t getAllianceStation(HAL_AllianceStationID* allianceStation) override;

  int32_t getJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) override;

  int32_t getJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) override;

  int32_t getJoystickButtons(int32_t joystickNum,
                             HAL_JoystickButtons* buttons) override;

  int32_t getJoystickTouchpads(int32_t joystickNum,
                               HAL_JoystickTouchpads* touchpads) override;

  int32_t getAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                             HAL_JoystickPOVs* povs,
                             HAL_JoystickButtons* buttons,
                             HAL_JoystickTouchpads* touchpads) override;

  int32_t getJoystickDescriptor(int32_t joystickNum,
                                HAL_JoystickDescriptor* desc) override;

  int32_t getGameData(HAL_GameData* gameData) override;

  int32_t setJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                            int32_t rightRumble, int32_t leftTriggerRumble,
                            int32_t rightTriggerRumble) override;

  int32_t setJoystickLeds(int32_t joystickNum, int32_t leds) override;

  int32_t getMatchTime(double* matchTime) override;

  int32_t getMatchInfo(HAL_MatchInfo* info) override;

  int32_t getOutputsEnabled(bool* outputsEnabled) override;

  int32_t refreshDSData(bool* wasRefreshed) override;

  void provideNewDataEventHandle(WPI_EventHandle handle) override;

  void removeNewDataEventHandle(WPI_EventHandle handle) override;

  int32_t observeUserProgramStarting() override;

  int32_t observeUserProgram(HAL_ControlWord controlWord) override;

  int32_t getSystemTimeValid(bool* systemTimeValid) override;

  int32_t writeAnsi(const struct WPI_String* line) override;

  int32_t clearDisplay() override;

  wpi::util::EventVector newDataEvents;

  void NewDriverStationData();

 private:
  int32_t BackendPrintFunctionImpl(bool isError, int32_t errorCode,
                                   const struct WPI_String* details,
                                   const struct WPI_String* location,
                                   const struct WPI_String* callStack,
                                   bool* forcePrintMsg);
  wpi::util::mutex cacheMutex;
  wpi::util::mutex tcpCacheMutex;

  BackendPrintFunction backendPrintFunc;

  HAL_ControlWord newestControlWord;
  JoystickDataCache caches[3];
  JoystickDataCache* currentRead = &caches[0];
  JoystickDataCache* currentReadLocal = &caches[0];
  std::atomic<JoystickDataCache*> currentCache{nullptr};
  JoystickDataCache* lastGiven = &caches[1];
  JoystickDataCache* cacheToUpdate = &caches[2];

  TcpCache tcpCache;
  TcpCache tcpCurrent;
};

MrcLibDsSimImpl::MrcLibDsSimImpl() {
  newestControlWord.value = 0;
  backendPrintFunc =
      [this](bool isError, int32_t errorCode, const struct WPI_String* details,
             const struct WPI_String* location,
             const struct WPI_String* callStack, bool* forcePrintMsg) {
        return BackendPrintFunctionImpl(isError, errorCode, details, location,
                                        callStack, forcePrintMsg);
      };
}

static std::atomic<HALSIM_SendErrorHandler> sendErrorHandler{nullptr};
static std::atomic<HALSIM_SendConsoleLineHandler> sendConsoleLineHandler{
    nullptr};

extern "C" {

void HALSIM_SetSendError(HALSIM_SendErrorHandler handler) {
  sendErrorHandler.store(handler);
}

void HALSIM_SetSendConsoleLine(HALSIM_SendConsoleLineHandler handler) {
  sendConsoleLineHandler.store(handler);
}
}  // extern "C"

int32_t MrcLibDsSimImpl::BackendPrintFunctionImpl(
    bool isError, int32_t errorCode, const struct WPI_String* details,
    const struct WPI_String* location, const struct WPI_String* callStack,
    bool* forcePrintMsg) {
  // This will defer to the caller, which needs to force print to true.
  *forcePrintMsg = true;
  return 0;
}

int32_t MrcLibDsSimImpl::sendError(bool isError, int32_t errorCode,
                                   const struct WPI_String* details,
                                   const struct WPI_String* location,
                                   const struct WPI_String* callStack,
                                   bool printMsg) {
  auto errorHandler = sendErrorHandler.load();
  if (errorHandler) {
    return errorHandler(isError, errorCode, details, location, callStack,
                        printMsg);
  }
  return DefaultSendErrorImpl(isError, errorCode, details, location, callStack,
                              printMsg, backendPrintFunc);
}

int32_t MrcLibDsSimImpl::sendConsoleLine(const struct WPI_String* line) {
  auto handler = sendConsoleLineHandler.load();
  if (handler) {
    return handler(line);
  }
  fmt::print("{}\n", wpi::util::to_string_view(line));
  std::fflush(stdout);
  return 0;
}

int32_t MrcLibDsSimImpl::sendProgramCrash(const struct WPI_String* details,
                                          const struct WPI_String* location,
                                          const struct WPI_String* callStack) {
  fmt::print(stderr, "Program Crash: {}\nLocation: {}\nCall Stack:\n{}\n",
             wpi::util::to_string_view(details),
             wpi::util::to_string_view(location),
             wpi::util::to_string_view(callStack));
  std::fflush(stderr);
  return 0;
}

int32_t MrcLibDsSimImpl::getControlWord(HAL_ControlWord* controlWord) {
  if (gShutdown) {
    controlWord->value = 0;
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *controlWord = newestControlWord;
  return 0;
}

int32_t MrcLibDsSimImpl::getUncachedControlWord(HAL_ControlWord* controlWord) {
  if (gShutdown) {
    controlWord->value = 0;
    return HAL_INCOMPATIBLE_STATE;
  }
  bool dsAttached = SimDriverStationData->dsAttached;
  if (dsAttached) {
    *controlWord = HAL_MakeControlWord(
        SimDriverStationData->opMode, SimDriverStationData->robotMode,
        SimDriverStationData->enabled, SimDriverStationData->eStop,
        SimDriverStationData->fmsAttached, SimDriverStationData->dsAttached);
  } else {
    controlWord->value = 0;
  }
  return 0;
}

int32_t MrcLibDsSimImpl::setOpModeOptions(
    const struct HAL_OpModeOption* options, int32_t count) {
  if (gShutdown) {
    return 0;
  }
  if (count < 0 || count > 1000 || (count != 0 && !options)) {
    return HAL_PARAMETER_OUT_OF_RANGE;
  }
  if (count == 0) {
    SimDriverStationData->SetOpModeOptions({});
  } else {
    SimDriverStationData->SetOpModeOptions({options, options + count});
  }
  return 0;
}

int32_t MrcLibDsSimImpl::getAllianceStation(
    HAL_AllianceStationID* allianceStation) {
  if (gShutdown) {
    *allianceStation = HAL_ALLIANCE_STATION_UNKNOWN;
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *allianceStation = currentRead->allianceStation;
  return 0;
}

int32_t MrcLibDsSimImpl::getJoystickAxes(int32_t joystickNum,
                                         HAL_JoystickAxes* axes) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getJoystickPOVs(int32_t joystickNum,
                                         HAL_JoystickPOVs* povs) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *povs = currentRead->povs[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getJoystickButtons(int32_t joystickNum,
                                            HAL_JoystickButtons* buttons) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *buttons = currentRead->buttons[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getJoystickTouchpads(
    int32_t joystickNum, HAL_JoystickTouchpads* touchpads) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *touchpads = currentRead->touchpads[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getAllJoystickData(int32_t joystickNum,
                                            HAL_JoystickAxes* axes,
                                            HAL_JoystickPOVs* povs,
                                            HAL_JoystickButtons* buttons,
                                            HAL_JoystickTouchpads* touchpads) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  *povs = currentRead->povs[joystickNum];
  *buttons = currentRead->buttons[joystickNum];
  *touchpads = currentRead->touchpads[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getJoystickDescriptor(int32_t joystickNum,
                                               HAL_JoystickDescriptor* desc) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{tcpCacheMutex};
  *desc = tcpCurrent.descriptors[joystickNum];
  return 0;
}

int32_t MrcLibDsSimImpl::getGameData(HAL_GameData* gameData) {
  if (gShutdown) {
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *gameData = currentRead->gameData;
  return 0;
}

int32_t MrcLibDsSimImpl::setJoystickRumble(int32_t joystickNum,
                                           int32_t leftRumble,
                                           int32_t rightRumble,
                                           int32_t leftTriggerRumble,
                                           int32_t rightTriggerRumble) {
  SimDriverStationData->SetJoystickRumbles(joystickNum, leftRumble, rightRumble,
                                           leftTriggerRumble,
                                           rightTriggerRumble);
  return 0;
}

int32_t MrcLibDsSimImpl::setJoystickLeds(int32_t joystickNum, int32_t leds) {
  SimDriverStationData->SetJoystickLeds(joystickNum, leds);
  return 0;
}

int32_t MrcLibDsSimImpl::getMatchTime(double* matchTime) {
  if (gShutdown) {
    *matchTime = 0;
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *matchTime = currentRead->matchTime;
  return 0;
}

int32_t MrcLibDsSimImpl::getMatchInfo(HAL_MatchInfo* info) {
  std::scoped_lock lock{tcpCacheMutex};
  *info = tcpCurrent.matchInfo;
  return 0;
}

int32_t MrcLibDsSimImpl::observeUserProgramStarting(void) {
  HALSIM_SetProgramStarted(true);
  return 0;
}

int32_t MrcLibDsSimImpl::observeUserProgram(HAL_ControlWord word) {
  HALSIM_SetProgramState(word);
  return 0;
}

int32_t MrcLibDsSimImpl::refreshDSData(bool* wasRefreshed) {
  if (gShutdown) {
    *wasRefreshed = false;
    return HAL_INCOMPATIBLE_STATE;
  }
  bool dsAttached = SimDriverStationData->dsAttached;
  JoystickDataCache* prev;
  {
    std::scoped_lock lock{cacheMutex};
    prev = currentCache.exchange(nullptr);
    if (prev != nullptr) {
      currentRead = prev;
    }
    // If newest state shows we have a DS attached, just use the
    // control word out of the cache, As it will be the one in sync
    // with the data. If no data has been updated, at this point,
    // and a DS wasn't attached previously, this will still return
    // a zeroed out control word, with is the correct state for
    // no new data.
    if (!dsAttached) {
      // If the DS is not attached, we need to zero out the control word.
      // This is because HAL_RefreshDSData is called asynchronously from
      // the DS data. The dsAttached variable comes directly from netcomm
      // and could be updated before the caches are. If that happens,
      // we would end up returning the previous cached control word,
      // which is out of sync with the current control word and could
      // break invariants such as which alliance station is in used.
      // Also, when the DS has never been connected the rest of the fields
      // in control word are garbage, so we also need to zero out in that
      // case too
      currentRead->controlWord.value = 0;
    }
    newestControlWord = currentRead->controlWord;
  }

  {
    tcpCache.Update();
    std::scoped_lock tcpLock(tcpCacheMutex);
    tcpCache.CloneTo(&tcpCurrent);
  }

  *wasRefreshed = prev != nullptr;
  return 0;
}

void MrcLibDsSimImpl::provideNewDataEventHandle(WPI_EventHandle handle) {
  if (gShutdown) {
    return;
  }
  wpi::hal::init::CheckInit();
  newDataEvents.Add(handle);
}

void MrcLibDsSimImpl::removeNewDataEventHandle(WPI_EventHandle handle) {
  if (gShutdown) {
    return;
  }
  newDataEvents.Remove(handle);
}

int32_t MrcLibDsSimImpl::getOutputsEnabled(bool* outputsEnabled) {
  if (gShutdown) {
    *outputsEnabled = false;
    return HAL_INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{cacheMutex};
  *outputsEnabled = HAL_ControlWord_IsEnabled(newestControlWord) &&
                    HAL_ControlWord_IsDSAttached(newestControlWord);
  return 0;
}

int32_t MrcLibDsSimImpl::getSystemTimeValid(bool* systemTimeValid) {
  *systemTimeValid = true;
  return 0;
}

int32_t MrcLibDsSimImpl::writeAnsi(const struct WPI_String* line) {
  return 0;
}

int32_t MrcLibDsSimImpl::clearDisplay() {
  return 0;
}

void MrcLibDsSimImpl::NewDriverStationData() {
  SimDriverStationData->dsAttached = true;
  cacheToUpdate->Update();

  JoystickDataCache* given = cacheToUpdate;
  JoystickDataCache* prev = currentCache.exchange(cacheToUpdate);
  if (prev == nullptr) {
    cacheToUpdate = currentReadLocal;
    currentReadLocal = lastGiven;
  } else {
    // Current read local does not update
    cacheToUpdate = prev;
  }
  lastGiven = given;

  newDataEvents.Wakeup();
  SimDriverStationData->CallNewDataCallbacks();
}

namespace wpi::hal {
void NewDriverStationData() {
  if (gShutdown) {
    return;
  }

  MrcLibDsSimImpl* defaultImpl =
      static_cast<MrcLibDsSimImpl*>(GetDefaultDriverStationImpl());
  defaultImpl->NewDriverStationData();
}

MrcLibDs* GetDefaultDriverStationImpl() {
  static MrcLibDsSimImpl impl;
  return &impl;
}

}  // namespace wpi::hal
