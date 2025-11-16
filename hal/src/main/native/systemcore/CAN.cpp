// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/CAN.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdio>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "PortsInternal.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/Threads.h"
#include "wpi/hal/handles/UnlimitedHandleResource.h"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Poll.hpp"
#include "wpi/net/uv/Timer.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/circular_buffer.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.h"

using namespace wpi::hal;

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

struct CANStreamStorage {
  CANStreamStorage(uint32_t maxMessages, uint8_t busId, uint32_t mask,
                   uint32_t filter)
      : receivedMessages{maxMessages},
        allowedMessages{maxMessages},
        canBusId{busId},
        canMask{mask},
        canFilter{filter} {}

  wpi::util::circular_buffer<struct HAL_CANStreamMessage> receivedMessages;
  bool didOverflow{false};
  uint32_t allowedMessages;
  uint8_t canBusId;
  uint32_t canMask;
  uint32_t canFilter;

  void CheckFrame(const HAL_CANStreamMessage& message);
};

struct SocketCanState {
  wpi::net::EventLoopRunner readLoopRunner;
  wpi::net::EventLoopRunner writeLoopRunner;
  wpi::util::mutex writeMutex[wpi::hal::kNumCanBuses];
  int socketHandle[wpi::hal::kNumCanBuses];
  // ms to count/timer map
  wpi::util::DenseMap<uint16_t,
                      std::pair<size_t, std::weak_ptr<wpi::net::uv::Timer>>>
      timers;
  // ms to bus mask/packet
  wpi::util::DenseMap<
      uint16_t, std::array<std::optional<canfd_frame>, wpi::hal::kNumCanBuses>>
      timedFrames;
  // packet to time
  wpi::util::DenseMap<uint32_t, std::array<uint16_t, wpi::hal::kNumCanBuses>>
      packetToTime;

  wpi::util::mutex readMutex[wpi::hal::kNumCanBuses];
  // TODO(thadhouse) we need a MUCH better way of doing this masking
  wpi::util::DenseMap<uint32_t, HAL_CANStreamMessage>
      readFrames[wpi::hal::kNumCanBuses];
  std::vector<CANStreamStorage*> canStreams[wpi::hal::kNumCanBuses];

  bool InitializeBuses();

  void TimerCallback(uint16_t time);

  void RemovePeriodic(uint8_t busMask, uint32_t messageId);
  void AddPeriodic(wpi::net::uv::Loop& loop, uint8_t busMask, uint16_t time,
                   const canfd_frame& frame);
};

}  // namespace

static UnlimitedHandleResource<HAL_CANStreamHandle, CANStreamStorage,
                               HAL_HandleEnum::CANStream>* canStreamHandles;

static SocketCanState* canState;

namespace wpi::hal::init {
void InitializeCAN() {
  canState = new SocketCanState{};
  static UnlimitedHandleResource<HAL_CANStreamHandle, CANStreamStorage,
                                 HAL_HandleEnum::CANStream>
      cSH;
  canStreamHandles = &cSH;
}
}  // namespace wpi::hal::init

void CANStreamStorage::CheckFrame(const HAL_CANStreamMessage& message) {
  if ((message.messageId & canMask) != canFilter) {
    return;
  }

  // We already are holding the lock here.

  if (receivedMessages.size() == allowedMessages) {
    didOverflow = true;
    return;
  }

  receivedMessages.push_back(message);
}

bool SocketCanState::InitializeBuses() {
  bool success = true;
  readLoopRunner.ExecSync([this, &success](wpi::net::uv::Loop& loop) {
    int32_t status = 0;
    HAL_SetCurrentThreadPriority(true, 50, &status);
    if (status != 0) {
      wpi::util::print("Failed to set CAN thread priority\n");
    }

    for (int i = 0; i < wpi::hal::kNumCanBuses; i++) {
      std::scoped_lock lock{writeMutex[i]};
      socketHandle[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
      if (socketHandle[i] == -1) {
        wpi::util::print("socket() for CAN {} failed with {}\n", i,
                         std::strerror(errno));
        success = false;
        return;
      }

      ifreq ifr;
      std::snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "can_s%d", i);

      if (ioctl(socketHandle[i], SIOCGIFINDEX, &ifr) == -1) {
        wpi::util::print("ioctl(SIOCGIFINDEX) for CAN {} failed with {}\n",
                         ifr.ifr_name, std::strerror(errno));
        success = false;
        return;
      }

      sockaddr_can addr;
      std::memset(&addr, 0, sizeof(addr));
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;

      if (bind(socketHandle[i], reinterpret_cast<const sockaddr*>(&addr),
               sizeof(addr)) == -1) {
        wpi::util::print("bind() for CAN {} failed with {}\n", ifr.ifr_name,
                         std::strerror(errno));
        success = false;
        return;
      }

      if (ioctl(socketHandle[i], SIOCGIFMTU, &ifr) == -1) {
        wpi::util::print("ioctl(SIOCGIFMTU) for CAN {} failed with {}\n",
                         ifr.ifr_name, std::strerror(errno));
        success = false;
        return;
      }

      if (ifr.ifr_mtu == CANFD_MTU) {
        int fdSet = 1;
        if (setsockopt(socketHandle[i], SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &fdSet,
                       sizeof(fdSet)) != 0) {
          wpi::util::print(
              "setsockopt(CAN_RAW_FD_FRAMES) for CAN {} failed with {}\n",
              ifr.ifr_name, std::strerror(errno));
          success = false;
          return;
        }
      }

      auto poll = wpi::net::uv::Poll::Create(loop, socketHandle[i]);
      if (!poll) {
        wpi::util::print("wpi::net::uv::Poll::Create for CAN {} failed\n",
                         ifr.ifr_name);
        success = false;
        return;
      }

      poll->pollEvent.connect([this, fd = socketHandle[i],
                               canIndex = i](int mask) {
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
          uint64_t timestamp = wpi::util::Now();
          // Ensure FDF flag is set for the read later.
          if (rVal == CANFD_MTU) {
            frame.flags = CANFD_FDF;
          }

          std::scoped_lock lock{readMutex[canIndex]};
          auto& msg = readFrames[canIndex][messageId];

          msg.messageId = messageId;
          msg.message.timeStamp = timestamp;

          msg.message.message.flags = HAL_CANFlags::HAL_CAN_NO_FLAGS;
          msg.message.message.flags |= (frame.flags & CANFD_FDF)
                                           ? HAL_CANFlags::HAL_CAN_FD_DATALENGTH
                                           : HAL_CANFlags::HAL_CAN_NO_FLAGS;
          msg.message.message.flags |=
              (frame.flags & CANFD_BRS) ? HAL_CANFlags::HAL_CAN_FD_BITRATESWITCH
                                        : HAL_CANFlags::HAL_CAN_NO_FLAGS;

          msg.message.message.dataSize = frame.len;
          if (frame.len > 0) {
            std::memcpy(msg.message.message.data, frame.data, frame.len);
          }

          for (auto&& stream : canStreams[canIndex]) {
            stream->CheckFrame(msg);
          }
        }
      });

      poll->Start(UV_READABLE);
    }
  });
  return success;
}

void SocketCanState::TimerCallback(uint16_t time) {
  auto& busFrames = timedFrames[time];
  for (size_t i = 0; i < busFrames.size(); i++) {
    const auto& frame = busFrames[i];
    if (!frame.has_value()) {
      continue;
    }
    std::scoped_lock lock{writeMutex[i]};
    int mtu = (frame->flags & CANFD_FDF) ? CANFD_MTU : CAN_MTU;
    send(canState->socketHandle[i], &*frame, mtu, 0);
  }
}

void SocketCanState::RemovePeriodic(uint8_t busId, uint32_t messageId) {
  // Find time, and remove from map
  auto& time = packetToTime[messageId][busId];
  auto storedTime = time;
  time = 0;

  // Its already been removed
  if (storedTime == 0) {
    return;
  }

  // Reset frame
  timedFrames[storedTime][busId].reset();

  auto& timer = timers[storedTime];
  // Stop the timer
  timer.first--;
  if (timer.first == 0) {
    if (auto l = timer.second.lock()) {
      l->Stop();
    }
  }
}

void SocketCanState::AddPeriodic(wpi::net::uv::Loop& loop, uint8_t busId,
                                 uint16_t time, const canfd_frame& frame) {
  packetToTime[frame.can_id][busId] = time;
  timedFrames[time][busId] = frame;
  auto& timer = timers[time];
  timer.first++;
  if (timer.first == 1) {
    auto newTimer = wpi::net::uv::Timer::Create(loop);
    newTimer->timeout.connect([this, time] { TimerCallback(time); });
    newTimer->Start(wpi::net::uv::Timer::Time{time},
                    wpi::net::uv::Timer::Time{time});
  }
}

namespace wpi::hal {
bool InitializeCanBuses() {
  return canState->InitializeBuses();
}
}  // namespace wpi::hal

namespace {}  // namespace

extern "C" {

void HAL_CAN_SendMessage(int32_t busId, uint32_t messageId,
                         const struct HAL_CANMessage* message, int32_t periodMs,
                         int32_t* status) {
  if (busId < 0 || busId >= wpi::hal::kNumCanBuses) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  messageId = MapMessageIdToSocketCan(messageId);

  // TODO determine on the real roborio is a non periodic send removes any
  // periodic send.
  if (periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING) {
    canState->writeLoopRunner.ExecSync([messageId, busId](wpi::net::uv::Loop&) {
      canState->RemovePeriodic(busId, messageId);
    });

    *status = 0;
    return;
  }

  canfd_frame frame;
  std::memset(&frame, 0, sizeof(frame));
  frame.can_id = messageId;
  frame.flags |=
      (message->flags & HAL_CANFlags::HAL_CAN_FD_DATALENGTH) ? CANFD_FDF : 0;
  frame.flags |=
      (message->flags & HAL_CANFlags::HAL_CAN_FD_BITRATESWITCH) ? CANFD_BRS : 0;
  if (message->dataSize) {
    auto size =
        (std::min)(message->dataSize, static_cast<uint8_t>(sizeof(frame.data)));
    std::memcpy(frame.data, message->data, size);
    frame.len = size;
  }

  int mtu = (message->flags & HAL_CANFlags::HAL_CAN_FD_DATALENGTH) ? CANFD_MTU
                                                                   : CAN_MTU;
  {
    std::scoped_lock lock{canState->writeMutex[busId]};
    int result = send(canState->socketHandle[busId], &frame, mtu, 0);
    if (result != mtu) {
      if (result == -1) {
        int err = errno;
        if (err == ENOBUFS) {
          *status = HAL_WARN_CANSessionMux_TxQueueFull;
          return;
        } else if (err == EAGAIN || err == EWOULDBLOCK) {
          *status = HAL_WARN_CANSessionMux_SocketBufferFull;
          return;
        }
      }

      // Print is here, and we can better debug this in the future.
      std::printf("Send Error %d %d %s\n", result, errno, std::strerror(errno));
      std::fflush(stdout);
      *status = HAL_ERR_CANSessionMux_InvalidBuffer;
      return;
    }
  }

  if (periodMs > 0) {
    canState->writeLoopRunner.ExecAsync(
        [busId, periodMs, frame](wpi::net::uv::Loop& loop) {
          canState->AddPeriodic(loop, busId, periodMs, frame);
        });
  }
}
void HAL_CAN_ReceiveMessage(int32_t busId, uint32_t messageId,
                            struct HAL_CANReceiveMessage* message,
                            int32_t* status) {
  if (busId < 0 || busId >= wpi::hal::kNumCanBuses) {
    message->message.dataSize = 0;
    message->timeStamp = 0;
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  std::scoped_lock lock{canState->readMutex[busId]};

  auto& msg = canState->readFrames[busId][messageId];
  if (msg.message.timeStamp == 0) {
    message->message.dataSize = 0;
    message->timeStamp = 0;
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
    return;
  }

  *message = msg.message;
  msg.message.timeStamp = 0;

  *status = 0;
  return;
}

HAL_CANStreamHandle HAL_CAN_OpenStreamSession(int32_t busId, uint32_t messageId,
                                              uint32_t messageIdMask,
                                              uint32_t maxMessages,
                                              int32_t* status) {
  if (busId < 0 || busId >= wpi::hal::kNumCanBuses) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  auto can = std::make_shared<CANStreamStorage>(maxMessages, busId,
                                                messageIdMask, messageId);

  auto handle = canStreamHandles->Allocate(can);

  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  std::scoped_lock lock{canState->readMutex[can->canBusId]};

  canState->canStreams[can->canBusId].push_back(can.get());

  return handle;
}

void HAL_CAN_CloseStreamSession(HAL_CANStreamHandle sessionHandle) {
  auto can = canStreamHandles->Free(sessionHandle);

  if (can == nullptr) {
    return;
  }

  std::scoped_lock lock{canState->readMutex[can->canBusId]};
  std::erase(canState->canStreams[can->canBusId], can.get());
}

void HAL_CAN_ReadStreamSession(HAL_CANStreamHandle sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  if (messages == nullptr || messagesRead == nullptr) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  auto can = canStreamHandles->Get(sessionHandle);
  if (!can) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  std::scoped_lock lock{canState->readMutex[can->canBusId]};

  size_t readCount = (std::min)(static_cast<size_t>(messagesToRead),
                                can->receivedMessages.size());

  for (size_t i = 0; i < readCount; i++) {
    messages[i] = can->receivedMessages.pop_front();
  }

  *messagesRead = readCount;

  if (can->didOverflow) {
    can->didOverflow = false;
    *status = HAL_ERR_CANSessionMux_SessionOverrun;
  }

  return;
}

void HAL_CAN_GetCANStatus(int32_t busId, float* percentBusUtilization,
                          uint32_t* busOffCount, uint32_t* txFullCount,
                          uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
}  // extern "C"
