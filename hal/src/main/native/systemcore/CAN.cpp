// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CAN.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

#include <wpi/DenseMap.h>
#include <wpi/mutex.h>
#include <wpi/timestamp.h>

#include "hal/Errors.h"
#include "hal/Threads.h"
#include "wpinet/EventLoopRunner.h"
#include "wpinet/uv/Poll.h"
#include "wpinet/uv/Timer.h"

#define NUM_CAN_BUSES 1

namespace {

static constexpr uint32_t MatchingBitMask = CAN_EFF_MASK | CAN_RTR_FLAG;

static_assert(CAN_RTR_FLAG == HAL_CAN_IS_FRAME_REMOTE);
static_assert(CAN_EFF_FLAG == HAL_CAN_IS_FRAME_11BIT);

uint32_t MapMessageIdToSocketCan(uint32_t id) {
  // Message and RTR map directly
  uint32_t toRet = id & MatchingBitMask;

  // Reverse the 11 bit flag
  if ((id & HAL_CAN_IS_FRAME_11BIT) == 0) {
    toRet |= CAN_EFF_FLAG;
  }

  return toRet;
}

uint32_t MapSocketCanToMessageId(uint32_t id) {
  // Message and RTR map directly
  uint32_t toRet = id & MatchingBitMask;

  // Reverse the 11 bit flag
  if ((id & CAN_EFF_FLAG) == 0) {
    toRet |= HAL_CAN_IS_FRAME_11BIT;
  }

  return toRet;
}

struct FrameStore {
  canfd_frame frame;
  uint64_t timestamp{0};
};

struct SocketCanState {
  wpi::EventLoopRunner loopRunner;
  wpi::mutex writeMutex[NUM_CAN_BUSES];
  int socketHandle[NUM_CAN_BUSES];
  wpi::mutex timerMutex;
  // ms to count/timer map
  wpi::DenseMap<uint16_t, std::pair<size_t, std::weak_ptr<wpi::uv::Timer>>>
      timers;
  // ms to bus mask/packet
  wpi::DenseMap<uint16_t, std::vector<std::pair<uint8_t, canfd_frame>>>
      timedFrames;
  // packet to time
  wpi::DenseMap<uint32_t, uint16_t> packetToTime;

  wpi::mutex readMutex[NUM_CAN_BUSES];
  // TODO(thadhouse) we need a MUCH better way of doing this masking
  wpi::DenseMap<uint32_t, FrameStore> readFrames[NUM_CAN_BUSES];

  bool InitializeBuses();
};

}  // namespace

static SocketCanState* canState;

namespace hal::init {
void InitializeCAN() {
  canState = new SocketCanState{};
}
}  // namespace hal::init

bool SocketCanState::InitializeBuses() {
  bool success = true;
  loopRunner.ExecSync([this, &success](wpi::uv::Loop& loop) {
    int32_t status = 0;
    HAL_SetCurrentThreadPriority(true, 50, &status);
    if (status != 0) {
      std::printf("Failed to set CAN thread priority\n");
    }

    for (int i = 0; i < NUM_CAN_BUSES; i++) {
      std::scoped_lock lock{writeMutex[i]};
      socketHandle[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
      if (socketHandle[i] == -1) {
        success = false;
        return;
      }

      ifreq ifr;
      std::snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "can%d", i);

      if (ioctl(socketHandle[i], SIOCGIFINDEX, &ifr) == -1) {
        success = false;
        return;
      }

      sockaddr_can addr;
      std::memset(&addr, 0, sizeof(addr));
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;

      if (bind(socketHandle[i], reinterpret_cast<const sockaddr*>(&addr),
               sizeof(addr)) == -1) {
        success = false;
        return;
      }

      std::printf("Successfully bound to can interface %d\n", i);

      auto poll = wpi::uv::Poll::Create(loop, socketHandle[i]);
      if (!poll) {
        success = false;
        return;
      }

      poll->pollEvent.connect(
          [this, fd = socketHandle[i], canIndex = i](int mask) {
            if (mask & UV_READABLE) {
              canfd_frame frame;
              int rVal = read(fd, &frame, sizeof(frame));
              if (rVal <= 0) {
                // TODO(thadhouse) error handling
                return;
              }
              if (frame.can_id & CAN_ERR_FLAG) {
                // Do nothing if this is an error frame
                return;
              }

              uint32_t messageId = MapSocketCanToMessageId(frame.can_id);
              uint64_t timestamp = wpi::Now();
              // Ensure FDF flag is set for the read later.
              if (rVal == CANFD_MTU) {
                frame.flags = CANFD_FDF;
              }

              std::scoped_lock lock{readMutex[canIndex]};
              auto& msg = readFrames[canIndex][messageId];
              msg.frame = frame;
              msg.timestamp = timestamp;
            }
          });

      poll->Start(UV_READABLE);
    }
  });
  return success;
}

namespace hal {
bool InitializeCanBuses() {
  return canState->InitializeBuses();
}
}  // namespace hal

namespace {}  // namespace

extern "C" {

void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status) {
  // TODO(thadhouse) this will become a parameter
  // isFd will also be a part of this parameter
  uint8_t busId = 0;

  if (busId >= NUM_CAN_BUSES) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  if (periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING) {
    // TODO(thadhouse) actually stop
    *status = 0;
    return;
  }

  bool isFd = false;
  messageID = MapMessageIdToSocketCan(messageID);

  canfd_frame frame;
  std::memset(&frame, 0, sizeof(frame));
  frame.can_id = messageID;
  frame.flags = isFd ? CANFD_FDF | CANFD_BRS : 0;
  if (dataSize) {
    auto size = (std::min)(dataSize, static_cast<uint8_t>(sizeof(frame.data)));
    std::memcpy(frame.data, data, size);
    frame.len = size;
  }

  int mtu = isFd ? CANFD_MTU : CAN_MTU;
  std::scoped_lock lock{canState->writeMutex[busId]};
  int result = send(canState->socketHandle[busId], &frame, mtu, 0);
  if (result != mtu) {
    // TODO(thadhouse) better error
    *status = HAL_ERR_CANSessionMux_InvalidBuffer;
    return;
  }

  if (periodMs > 0) {
    // TODO(thadhouse) set repeating
  }
}
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status) {
  uint8_t busId = 0;

  if (busId >= NUM_CAN_BUSES) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{canState->readMutex[busId]};

  // TODO(thadhouse) this is going to be wrong, but we're going to assume that
  // any lookup without the 11 bit mask set wants to look for a 29 bit frame.
  // Also, only do fast lookups for 29 bit frames

  // Fast case is the following.
  // Mask doesn't include 11 bit flag
  // Mask doesn't include RTR flag
  // Mask is full
  if (messageIDMask == CAN_EFF_MASK) {
    //  We're doing a fast lookup
    auto& msg = canState->readFrames[busId][*messageID];
    if (msg.timestamp == 0) {
      *status = HAL_ERR_CANSessionMux_MessageNotFound;
      return;
    }
    if ((msg.frame.flags & CANFD_FDF) || msg.frame.len > 8) {
      std::printf("FD frames not supported for read right now\n");
      *status = HAL_ERR_CANSessionMux_InvalidBuffer;
      return;
    }
    // TODO(thadhouse) this time needs to be fixed up.
    *timeStamp = msg.timestamp / 1000;
    std::memcpy(data, msg.frame.data, msg.frame.len);
    *dataSize = msg.frame.len;
    *status = 0;
    msg.timestamp = 0;
    return;
  }

  std::printf("Slow lookup not supported yet\n");

  // Add support for slow lookup later
  *status = HAL_ERR_CANSessionMux_NotAllowed;
  return;
}
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status) {
  *sessionHandle = 0;
  *status = HAL_HANDLE_ERROR;
  return;
}
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle) {}
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
}  // extern "C"
