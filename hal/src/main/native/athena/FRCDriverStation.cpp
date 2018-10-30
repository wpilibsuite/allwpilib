/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/NetCommRPCProxy_Occur.h>
#include <wpi/SafeThread.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "hal/DriverStation.h"

static_assert(sizeof(int32_t) >= sizeof(int),
              "FRC_NetworkComm status variable is larger than 32 bits");

struct HAL_JoystickAxesInt {
  int16_t count;
  int16_t axes[HAL_kMaxJoystickAxes];
};

static constexpr int kJoystickPorts = 6;

// Joystick User Data
static std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxes;
static std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVs;
static std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtons;
static std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptor;
static std::unique_ptr<HAL_MatchInfo> m_matchInfo;

// Joystick Cached Data
static std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxesCache;
static std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVsCache;
static std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtonsCache;
static std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptorCache;
static std::unique_ptr<HAL_MatchInfo> m_matchInfoCache;

static wpi::mutex m_cacheDataMutex;

// Control word variables
static HAL_ControlWord m_controlWordCache;
static std::chrono::steady_clock::time_point m_lastControlWordUpdate;
static wpi::mutex m_controlWordMutex;

// Message and Data variables
static wpi::mutex msgMutex;

static void InitializeDriverStationCaches() {
  m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptor =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);
  m_matchInfo = std::make_unique<HAL_MatchInfo>();
  m_joystickAxesCache = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVsCache = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtonsCache =
      std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptorCache =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);
  m_matchInfoCache = std::make_unique<HAL_MatchInfo>();

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to speed controllers.
  for (unsigned int i = 0; i < kJoystickPorts; i++) {
    m_joystickAxes[i].count = 0;
    m_joystickPOVs[i].count = 0;
    m_joystickButtons[i].count = 0;
    m_joystickDescriptor[i].isXbox = 0;
    m_joystickDescriptor[i].type = -1;
    m_joystickDescriptor[i].name[0] = '\0';

    m_joystickAxesCache[i].count = 0;
    m_joystickPOVsCache[i].count = 0;
    m_joystickButtonsCache[i].count = 0;
    m_joystickDescriptorCache[i].isXbox = 0;
    m_joystickDescriptorCache[i].type = -1;
    m_joystickDescriptorCache[i].name[0] = '\0';
  }
}

static int32_t HAL_GetJoystickAxesInternal(int32_t joystickNum,
                                           HAL_JoystickAxes* axes) {
  HAL_JoystickAxesInt axesInt;

  int retVal = FRC_NetworkCommunication_getJoystickAxes(
      joystickNum, reinterpret_cast<JoystickAxes_t*>(&axesInt),
      HAL_kMaxJoystickAxes);

  // copy integer values to double values
  axes->count = axesInt.count;
  // current scaling is -128 to 127, can easily be patched in the future by
  // changing this function.
  for (int32_t i = 0; i < axesInt.count; i++) {
    int8_t value = axesInt.axes[i];
    if (value < 0) {
      axes->axes[i] = value / 128.0;
    } else {
      axes->axes[i] = value / 127.0;
    }
  }

  return retVal;
}

static int32_t HAL_GetJoystickPOVsInternal(int32_t joystickNum,
                                           HAL_JoystickPOVs* povs) {
  return FRC_NetworkCommunication_getJoystickPOVs(
      joystickNum, reinterpret_cast<JoystickPOV_t*>(povs),
      HAL_kMaxJoystickPOVs);
}

static int32_t HAL_GetJoystickButtonsInternal(int32_t joystickNum,
                                              HAL_JoystickButtons* buttons) {
  return FRC_NetworkCommunication_getJoystickButtons(
      joystickNum, &buttons->buttons, &buttons->count);
}
/**
 * Retrieve the Joystick Descriptor for particular slot
 * @param desc [out] descriptor (data transfer object) to fill in.  desc is
 * filled in regardless of success. In other words, if descriptor is not
 * available, desc is filled in with default values matching the init-values in
 * Java and C++ Driverstation for when caller requests a too-large joystick
 * index.
 *
 * @return error code reported from Network Comm back-end.  Zero is good,
 * nonzero is bad.
 */
static int32_t HAL_GetJoystickDescriptorInternal(int32_t joystickNum,
                                                 HAL_JoystickDescriptor* desc) {
  desc->isXbox = 0;
  desc->type = std::numeric_limits<uint8_t>::max();
  desc->name[0] = '\0';
  desc->axisCount =
      HAL_kMaxJoystickAxes; /* set to the desc->axisTypes's capacity */
  desc->buttonCount = 0;
  desc->povCount = 0;
  int retval = FRC_NetworkCommunication_getJoystickDesc(
      joystickNum, &desc->isXbox, &desc->type,
      reinterpret_cast<char*>(&desc->name), &desc->axisCount,
      reinterpret_cast<uint8_t*>(&desc->axisTypes), &desc->buttonCount,
      &desc->povCount);
  /* check the return, if there is an error and the RIOimage predates FRC2017,
   * then axisCount needs to be cleared */
  if (retval != 0) {
    /* set count to zero so downstream code doesn't decode invalid axisTypes. */
    desc->axisCount = 0;
  }
  return retval;
}

static int32_t HAL_GetControlWordInternal(HAL_ControlWord* controlWord) {
  std::memset(controlWord, 0, sizeof(HAL_ControlWord));
  return FRC_NetworkCommunication_getControlWord(
      reinterpret_cast<ControlWord_t*>(controlWord));
}

static int32_t HAL_GetMatchInfoInternal(HAL_MatchInfo* info) {
  MatchType_t matchType = MatchType_t::kMatchType_none;
  int status = FRC_NetworkCommunication_getMatchInfo(
      info->eventName, &matchType, &info->matchNumber, &info->replayNumber,
      info->gameSpecificMessage, &info->gameSpecificMessageSize);

  info->matchType = static_cast<HAL_MatchType>(matchType);

  *(std::end(info->eventName) - 1) = '\0';

  return status;
}

static void UpdateDriverStationControlWord(bool force,
                                           HAL_ControlWord& controlWord) {
  auto now = std::chrono::steady_clock::now();
  std::lock_guard<wpi::mutex> lock(m_controlWordMutex);
  // Update every 50 ms or on force.
  if ((now - m_lastControlWordUpdate > std::chrono::milliseconds(50)) ||
      force) {
    HAL_GetControlWordInternal(&m_controlWordCache);
    m_lastControlWordUpdate = now;
  }
  controlWord = m_controlWordCache;
}

static void UpdateDriverStationDataCaches() {
  // Get the status of all of the joysticks, and save to the cache
  for (uint8_t stick = 0; stick < kJoystickPorts; stick++) {
    HAL_GetJoystickAxesInternal(stick, &m_joystickAxesCache[stick]);
    HAL_GetJoystickPOVsInternal(stick, &m_joystickPOVsCache[stick]);
    HAL_GetJoystickButtonsInternal(stick, &m_joystickButtonsCache[stick]);
    HAL_GetJoystickDescriptorInternal(stick, &m_joystickDescriptorCache[stick]);
  }
  // Grab match specific data
  HAL_GetMatchInfoInternal(m_matchInfoCache.get());

  // Force a control word update, to make sure the data is the newest.
  HAL_ControlWord controlWord;
  UpdateDriverStationControlWord(true, controlWord);

  {
    // Obtain a lock on the data, swap the cached data into the main data arrays
    std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);

    m_joystickAxes.swap(m_joystickAxesCache);
    m_joystickPOVs.swap(m_joystickPOVsCache);
    m_joystickButtons.swap(m_joystickButtonsCache);
    m_joystickDescriptor.swap(m_joystickDescriptorCache);
    m_matchInfo.swap(m_matchInfoCache);
  }
}

class DriverStationThread : public wpi::SafeThread {
 public:
  void Main() {
    std::unique_lock<wpi::mutex> lock(m_mutex);
    while (m_active) {
      m_cond.wait(lock, [&] { return !m_active || m_notify; });
      if (!m_active) break;
      m_notify = false;

      lock.unlock();
      UpdateDriverStationDataCaches();
      lock.lock();

      // Notify all threads
      newDSDataAvailableCounter++;
      newDSDataAvailableCond.notify_all();
    }

    // Notify waiters on thread exit
    newDSDataAvailableCounter++;
    newDSDataAvailableCond.notify_all();
  }

  bool m_notify = false;
  wpi::condition_variable newDSDataAvailableCond;
  int newDSDataAvailableCounter{0};
};

class DriverStationThreadOwner
    : public wpi::SafeThreadOwner<DriverStationThread> {
 public:
  void Notify() {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_notify = true;
    thr->m_cond.notify_one();
  }
};

static std::unique_ptr<DriverStationThreadOwner> dsThread = nullptr;

namespace hal {
namespace init {
void InitializeFRCDriverStation() {}
}  // namespace init
}  // namespace hal

extern "C" {

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::lock_guard<wpi::mutex> lock(msgMutex);
  static std::string prevMsg[KEEP_MSGS];
  static std::chrono::time_point<std::chrono::steady_clock>
      prevMsgTime[KEEP_MSGS];
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < KEEP_MSGS; i++) {
      prevMsgTime[i] =
          std::chrono::steady_clock::now() - std::chrono::seconds(2);
    }
    initialized = true;
  }

  auto curTime = std::chrono::steady_clock::now();
  int i;
  for (i = 0; i < KEEP_MSGS; ++i) {
    if (prevMsg[i] == details) break;
  }
  int retval = 0;
  if (i == KEEP_MSGS || (curTime - prevMsgTime[i]) >= std::chrono::seconds(1)) {
    retval = FRC_NetworkCommunication_sendError(isError, errorCode, isLVCode,
                                                details, location, callStack);
    if (printMsg) {
      if (location && location[0] != '\0') {
        wpi::errs() << (isError ? "Error" : "Warning") << " at " << location
                    << ": ";
      }
      wpi::errs() << details << "\n";
      if (callStack && callStack[0] != '\0') {
        wpi::errs() << callStack << "\n";
      }
    }
    if (i == KEEP_MSGS) {
      // replace the oldest one
      i = 0;
      auto first = prevMsgTime[0];
      for (int j = 1; j < KEEP_MSGS; ++j) {
        if (prevMsgTime[j] < first) {
          first = prevMsgTime[j];
          i = j;
        }
      }
      prevMsg[i] = details;
    }
    prevMsgTime[i] = curTime;
  }
  return retval;
}

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  std::memset(controlWord, 0, sizeof(HAL_ControlWord));
  UpdateDriverStationControlWord(false, *controlWord);
  return 0;
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  *axes = m_joystickAxes[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  *povs = m_joystickPOVs[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  *buttons = m_joystickButtons[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  *desc = m_joystickDescriptor[joystickNum];
  return 0;
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  *info = *m_matchInfo;
  return 0;
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  HAL_AllianceStationID allianceStation;
  *status = FRC_NetworkCommunication_getAllianceStation(
      reinterpret_cast<AllianceStationID_t*>(&allianceStation));
  return allianceStation;
}

HAL_Bool HAL_GetJoystickIsXbox(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return 0;
  } else {
    return joystickDesc.isXbox;
  }
}

int32_t HAL_GetJoystickType(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.type;
  }
}

char* HAL_GetJoystickName(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    char* name = static_cast<char*>(std::malloc(1));
    name[0] = '\0';
    return name;
  } else {
    size_t len = std::strlen(joystickDesc.name);
    char* name = static_cast<char*>(std::malloc(len + 1));
    std::strncpy(name, joystickDesc.name, len);
    name[len] = '\0';
    return name;
  }
}

void HAL_FreeJoystickName(char* name) { std::free(name); }

int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.axisTypes[axis];
  }
}

int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs,
                                                     leftRumble, rightRumble);
}

double HAL_GetMatchTime(int32_t* status) {
  float matchTime;
  *status = FRC_NetworkCommunication_getMatchTime(&matchTime);
  return matchTime;
}

void HAL_ObserveUserProgramStarting(void) {
  FRC_NetworkCommunication_observeUserProgramStarting();
}

void HAL_ObserveUserProgramDisabled(void) {
  FRC_NetworkCommunication_observeUserProgramDisabled();
}

void HAL_ObserveUserProgramAutonomous(void) {
  FRC_NetworkCommunication_observeUserProgramAutonomous();
}

void HAL_ObserveUserProgramTeleop(void) {
  FRC_NetworkCommunication_observeUserProgramTeleop();
}

void HAL_ObserveUserProgramTest(void) {
  FRC_NetworkCommunication_observeUserProgramTest();
}

HAL_Bool HAL_IsNewControlData(void) {
  // There is a rollover error condition here. At Packet# = n * (uintmax), this
  // will return false when instead it should return true. However, this at a
  // 20ms rate occurs once every 2.7 years of DS connected runtime, so not
  // worth the cycles to check.
  thread_local int lastCount{-1};
  if (!dsThread) return false;
  auto thr = dsThread->GetThread();
  if (!thr) return false;
  int currentCount = thr->newDSDataAvailableCounter;
  if (lastCount == currentCount) return false;
  lastCount = currentCount;
  return true;
}

/**
 * Waits for the newest DS packet to arrive. Note that this is a blocking call.
 */
void HAL_WaitForDSData(void) { HAL_WaitForDSDataTimeout(0); }

/**
 * Waits for the newest DS packet to arrive. If timeout is <= 0, this will wait
 * forever. Otherwise, it will wait until either a new packet, or the timeout
 * time has passed. Returns true on new data, false on timeout.
 */
HAL_Bool HAL_WaitForDSDataTimeout(double timeout) {
  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  if (!dsThread) return false;
  auto thr = dsThread->GetThread();
  if (!thr) return false;
  int currentCount = thr->newDSDataAvailableCounter;
  while (thr->newDSDataAvailableCounter == currentCount) {
    if (timeout > 0) {
      auto timedOut =
          thr->newDSDataAvailableCond.wait_until(thr.GetLock(), timeoutTime);
      if (timedOut == std::cv_status::timeout) {
        return false;
      }
    } else {
      thr->newDSDataAvailableCond.wait(thr.GetLock());
    }
  }
  return true;
}

// Constant number to be used for our occur handle
constexpr int32_t refNumber = 42;

static void newDataOccur(uint32_t refNum) {
  // Since we could get other values, require our specific handle
  // to signal our threads
  if (refNum != refNumber) return;
  dsThread->Notify();
}

/*
 * Call this to initialize the driver station communication. This will properly
 * handle multiple calls. However note that this CANNOT be called from a library
 * that interfaces with LabVIEW.
 */
void HAL_InitializeDriverStation(void) {
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<wpi::mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;

  InitializeDriverStationCaches();

  dsThread = std::make_unique<DriverStationThreadOwner>();
  dsThread->Start();

  // Set up the occur function internally with NetComm
  NetCommRPCProxy_SetOccurFuncPointer(newDataOccur);
  // Set up our occur reference number
  setNewDataOccurRef(refNumber);

  initialized = true;
}

/*
 * Releases the DS Mutex to allow proper shutdown of any threads that are
 * waiting on it.
 */
void HAL_ReleaseDSMutex(void) { newDataOccur(refNumber); }

}  // extern "C"
