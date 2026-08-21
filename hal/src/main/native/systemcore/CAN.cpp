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

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "CANInternal.hpp"
#include "PortsInternal.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/Threads.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"
#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Poll.hpp"
#include "wpi/net/uv/Timer.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/circular_buffer.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.hpp"

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

canfd_frame MakeSocketCanFrame(uint32_t messageId,
                               const HAL_CANMessage& message) {
  canfd_frame frame;
  std::memset(&frame, 0, sizeof(frame));
  frame.can_id = messageId;
  frame.flags |=
      (message.flags & HAL_CANFlags::HAL_CAN_FD_DATALENGTH) ? CANFD_FDF : 0;
  frame.flags |=
      (message.flags & HAL_CANFlags::HAL_CAN_FD_BITRATESWITCH) ? CANFD_BRS : 0;
  if (message.dataSize) {
    auto size =
        (std::min)(message.dataSize, static_cast<uint8_t>(sizeof(frame.data)));
    std::memcpy(frame.data, message.data, size);
    frame.len = size;
  }
  return frame;
}

HAL_CANMessage MakeHALCanMessage(const canfd_frame& frame) {
  HAL_CANMessage message;
  std::memset(&message, 0, sizeof(message));
  message.flags |= (frame.flags & CANFD_FDF)
                       ? HAL_CANFlags::HAL_CAN_FD_DATALENGTH
                       : HAL_CANFlags::HAL_CAN_NO_FLAGS;
  message.flags |= (frame.flags & CANFD_BRS)
                       ? HAL_CANFlags::HAL_CAN_FD_BITRATESWITCH
                       : HAL_CANFlags::HAL_CAN_NO_FLAGS;
  message.dataSize = frame.len;
  if (frame.len > 0) {
    std::memcpy(message.data, frame.data, frame.len);
  }
  return message;
}

struct CANStreamStorage {
  CANStreamStorage(uint32_t maxMessages, uint8_t busId, uint32_t mask,
                   uint32_t filter)
      : receivedMessages{maxMessages},
        allowedMessages{maxMessages},
        canBusId{busId},
        canMask{mask},
        canFilter{filter & mask} {}

  wpi::util::circular_buffer<struct HAL_CANStreamMessage> receivedMessages;
  bool didOverflow{false};
  uint32_t allowedMessages;
  uint8_t canBusId;
  uint32_t canMask;
  uint32_t canFilter;

  void CheckFrame(const HAL_CANStreamMessage& message);
};

struct PeriodicFrame {
  canfd_frame frame;
  std::shared_ptr<wpi::net::uv::Timer> timer;
  CANPeriodicSendCallback callback;
  void* callbackParam;
  int32_t status{0};
};

struct SocketCanState {
  wpi::net::EventLoopRunner readLoopRunner;
  wpi::net::EventLoopRunner writeLoopRunner;
  wpi::util::mutex writeMutex[wpi::hal::NUM_CAN_BUSES];
  int socketHandle[wpi::hal::NUM_CAN_BUSES];
  // Message ID to per-bus periodic frame state. Accessed only on
  // writeLoopRunner.
  wpi::util::DenseMap<uint32_t, std::array<std::optional<PeriodicFrame>,
                                           wpi::hal::NUM_CAN_BUSES>>
      periodicFrames;

  wpi::util::mutex readMutex[wpi::hal::NUM_CAN_BUSES];
  // TODO(thadhouse) we need a MUCH better way of doing this masking
  wpi::util::DenseMap<uint32_t, HAL_CANStreamMessage>
      readFrames[wpi::hal::NUM_CAN_BUSES];
  std::vector<CANStreamStorage*> canStreams[wpi::hal::NUM_CAN_BUSES];

  bool InitializeBuses();

  int32_t SendFrame(uint8_t busId, const canfd_frame& frame);
  int32_t SendFrameWithCallback(uint8_t busId, const canfd_frame& frame,
                                CANPeriodicSendCallback callback,
                                void* callbackParam);

  void TimerCallback(uint8_t busId, uint32_t messageId);

  int32_t RemovePeriodic(uint8_t busId, uint32_t messageId);
  int32_t AddOrUpdatePeriodic(wpi::net::uv::Loop& loop, uint8_t busId,
                              uint32_t periodMs, const canfd_frame& frame,
                              CANPeriodicSendCallback callback,
                              void* callbackParam);
};

}  // namespace

static UnlimitedHandleResource<HAL_CANStreamHandle, CANStreamStorage,
                               HAL_HandleEnum::CAN_STREAM>* canStreamHandles;

static SocketCanState* canState;

namespace wpi::hal::init {
void InitializeCAN() {
  canState = new SocketCanState{};
  static UnlimitedHandleResource<HAL_CANStreamHandle, CANStreamStorage,
                                 HAL_HandleEnum::CAN_STREAM>
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
    if (HAL_SetCurrentThreadPriority(50) != 0) {
      wpi::util::print("Failed to set CAN thread priority\n");
    }

    for (int i = 0; i < wpi::hal::NUM_CAN_BUSES; i++) {
      std::scoped_lock lock{writeMutex[i]};
      socketHandle[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
      if (socketHandle[i] == -1) {
        wpi::util::print("socket() for CAN {} failed with {}\n", i,
                         std::strerror(errno));
        success = false;
        return;
      }

      ifreq ifr;

      if (i < 5) {
        std::snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "can_s%u",
                      static_cast<unsigned>(i));
      } else {
        std::snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "can_d%u",
                      static_cast<unsigned>(i - 5));
      }

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
            frame.flags |= CANFD_FDF;
          }

          std::scoped_lock lock{readMutex[canIndex]};
          auto& msg = readFrames[canIndex][messageId];

          msg.messageId = messageId;
          msg.message.timeStamp = timestamp;

          msg.message.message.flags = HAL_CANFlags::HAL_CAN_NO_FLAGS;
          msg.message.message.flags |= (frame.flags & CANFD_FDF)
                                           ? HAL_CANFlags::HAL_CAN_FD_DATALENGTH
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

int32_t SocketCanState::SendFrame(uint8_t busId, const canfd_frame& frame) {
  std::scoped_lock lock{writeMutex[busId]};
  int mtu = (frame.flags & CANFD_FDF) ? CANFD_MTU : CAN_MTU;
  int result = send(socketHandle[busId], &frame, mtu, 0);
  if (result == mtu) {
    return 0;
  }

  if (result == -1) {
    int err = errno;
    if (err == ENOBUFS) {
      return HAL_WARN_CANSessionMux_TxQueueFull;
    } else if (err == EAGAIN || err == EWOULDBLOCK) {
      return HAL_WARN_CANSessionMux_SocketBufferFull;
    }
  }

  return HAL_ERR_CANSessionMux_InvalidBuffer;
}

int32_t SocketCanState::SendFrameWithCallback(uint8_t busId,
                                              const canfd_frame& frame,
                                              CANPeriodicSendCallback callback,
                                              void* callbackParam) {
  if (callback == nullptr) {
    return SendFrame(busId, frame);
  }

  // CAN reads do not use writeLoopRunner, so they are safe from this callback.
  // CAN sends would synchronously reenter writeLoopRunner and deadlock.
  HAL_CANMessage message = MakeHALCanMessage(frame);
  int32_t status = callback(callbackParam, &message);

  bool noToken = (status == HAL_WARN_CANSessionMux_NoToken);
  bool notAllowed = (status == HAL_ERR_CANSessionMux_NotAllowed);

  if (status != 0) {
    // If not allowed, that just means not enabled and to not send.
    // Return 0 to indicate that it was not sent, but not an error.
    if (notAllowed) {
      return 0;
    } else if (!noToken) {
      // If we have an error other then no token, just error.
      return status;
    }
    // We have no token, but we will still send the frame. The status will be
    // returned to the caller.
  }

  status = SendFrame(busId, MakeSocketCanFrame(frame.can_id, message));

  // If the send succeeded, but we have no token, return the no token warning.
  if (noToken && status == 0) {
    return HAL_WARN_CANSessionMux_NoToken;
  }
  return status;
}

void SocketCanState::TimerCallback(uint8_t busId, uint32_t messageId) {
  auto messageIt = periodicFrames.find(messageId);
  if (messageIt == periodicFrames.end()) {
    return;
  }

  auto& periodic = messageIt->second[busId];
  if (!periodic) {
    return;
  }

  periodic->status = SendFrameWithCallback(
      busId, periodic->frame, periodic->callback, periodic->callbackParam);
}

int32_t SocketCanState::RemovePeriodic(uint8_t busId, uint32_t messageId) {
  auto messageIt = periodicFrames.find(messageId);
  if (messageIt == periodicFrames.end()) {
    return 0;
  }

  auto& periodic = messageIt->second[busId];
  if (!periodic) {
    return 0;
  }

  int32_t status = periodic->status;
  periodic->timer->Stop();
  periodic->timer->Close();
  periodic.reset();

  bool hasPeriodicFrame = false;
  for (const auto& busFrame : messageIt->second) {
    if (busFrame) {
      hasPeriodicFrame = true;
      break;
    }
  }
  if (!hasPeriodicFrame) {
    periodicFrames.erase(messageIt);
  }

  return status;
}

int32_t SocketCanState::AddOrUpdatePeriodic(wpi::net::uv::Loop& loop,
                                            uint8_t busId, uint32_t periodMs,
                                            const canfd_frame& frame,
                                            CANPeriodicSendCallback callback,
                                            void* callbackParam) {
  auto [messageIt, inserted] = periodicFrames.try_emplace(frame.can_id);
  auto& periodic = messageIt->second[busId];
  if (periodic) {
    int32_t status = periodic->status;
    periodic->frame = frame;
    periodic->callback = callback;
    periodic->callbackParam = callbackParam;
    periodic->timer->SetRepeat(wpi::net::uv::Timer::Time{periodMs});
    return status;
  }

  auto timer = wpi::net::uv::Timer::Create(loop);
  if (!timer) {
    if (inserted) {
      periodicFrames.erase(messageIt);
    }
    return HAL_ERR_CANSessionMux_NotInitialized;
  }

  periodic.emplace(PeriodicFrame{frame, timer, callback, callbackParam, 0});
  timer->timeout.connect([this, busId, messageId = frame.can_id] {
    TimerCallback(busId, messageId);
  });
  timer->Start(wpi::net::uv::Timer::Time{periodMs},
               wpi::net::uv::Timer::Time{periodMs});
  return SendFrameWithCallback(busId, frame, callback, callbackParam);
}

namespace wpi::hal {
bool InitializeCanBuses() {
  return canState->InitializeBuses();
}
}  // namespace wpi::hal

namespace {

void SendMessage(int32_t busId, uint32_t messageId,
                 const struct HAL_CANMessage* message, int32_t periodMs,
                 CANPeriodicSendCallback callback, void* callbackParam,
                 int32_t* status) {
  *status = 0;

  if (busId < 0 || busId >= wpi::hal::NUM_CAN_BUSES) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  if (message == nullptr || periodMs < HAL_CAN_SEND_PERIOD_STOP_REPEATING) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  if (busId >= 5 &&
      ((message->flags & HAL_CANFlags::HAL_CAN_FD_DATALENGTH) ||
       (message->flags & HAL_CANFlags::HAL_CAN_FD_BITRATESWITCH))) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return;
  }

  messageId = MapMessageIdToSocketCan(messageId);

  if (periodMs == HAL_CAN_SEND_PERIOD_STOP_REPEATING) {
    canState->writeLoopRunner.ExecSync(
        [messageId, busId, status](wpi::net::uv::Loop&) {
          *status = canState->RemovePeriodic(busId, messageId);
        });
    return;
  }

  canfd_frame frame = MakeSocketCanFrame(messageId, *message);

  if (periodMs == HAL_CAN_SEND_PERIOD_NO_REPEAT) {
    canState->writeLoopRunner.ExecSync([messageId, busId, &frame, callback,
                                        callbackParam,
                                        status](wpi::net::uv::Loop&) {
      canState->RemovePeriodic(busId, messageId);
      *status = canState->SendFrameWithCallback(busId, frame, callback,
                                                callbackParam);
    });
    return;
  }

  canState->writeLoopRunner.ExecSync([busId, periodMs, &frame, callback,
                                      callbackParam,
                                      status](wpi::net::uv::Loop& loop) {
    *status = canState->AddOrUpdatePeriodic(loop, busId, periodMs, frame,
                                            callback, callbackParam);
  });
}

}  // namespace

namespace wpi::hal {
void SendCANMessageWithPeriodicCallback(int32_t busId, uint32_t messageId,
                                        const HAL_CANMessage* message,
                                        int32_t periodMs,
                                        CANPeriodicSendCallback callback,
                                        void* param, int32_t* status) {
  SendMessage(busId, messageId, message, periodMs, callback, param, status);
}
}  // namespace wpi::hal

extern "C" {

void HAL_CAN_SendMessage(int32_t busId, uint32_t messageId,
                         const struct HAL_CANMessage* message, int32_t periodMs,
                         int32_t* status) {
  SendMessage(busId, messageId, message, periodMs, nullptr, nullptr, status);
}

void HAL_CAN_ReceiveMessage(int32_t busId, uint32_t messageId,
                            struct HAL_CANReceiveMessage* message,
                            int32_t* status) {
  if (busId < 0 || busId >= wpi::hal::NUM_CAN_BUSES) {
    message->message.dataSize = 0;
    message->timeStamp = 0;
    *status = HAL_PARAMETER_OUT_OF_RANGE;
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
  *status = 0;

  if (busId < 0 || busId >= wpi::hal::NUM_CAN_BUSES) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  if (maxMessages == 0) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  auto can = std::make_shared<CANStreamStorage>(maxMessages, busId,
                                                messageIdMask, messageId);

  auto handle = canStreamHandles->Allocate(can);

  if (handle == HAL_INVALID_HANDLE) {
    *status = HAL_NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
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
  *status = 0;

  if (messages == nullptr || messagesRead == nullptr) {
    *status = HAL_PARAMETER_OUT_OF_RANGE;
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
