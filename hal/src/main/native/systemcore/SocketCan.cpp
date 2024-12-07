#include "wpinet/EventLoopRunner.h"
#include "wpi/mutex.h"
#include "wpi/DenseMap.h"

#include "wpinet/uv/Poll.h"
#include "wpinet/uv/Timer.h"

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "hal/Threads.h"

#define NUM_CAN_BUSES 2

namespace {

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

  bool InitializeBuses();
};

}  // namespace

static SocketCanState* canState;

namespace hal::init {
void InitializeSocketCan() {
  canState = new SocketCanState{};
}
}  // namespace hal::init

bool SocketCanState::InitializeBuses() {
  bool success = true;
  loopRunner.ExecSync([this, &success](wpi::uv::Loop& loop) {
    int32_t status = 0;
    HAL_SetCurrentThreadPriority(true, 50, &status);
    if (status != 0) {
        printf("Failed to set CAN thread priority\n");
    }

    for (int i = 0; i < NUM_CAN_BUSES; i++) {
      std::scoped_lock lock{writeMutex[i]};
      socketHandle[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
      if (socketHandle[i] == -1) {
        success = false;
        return;
      }

      ifreq ifr;
      strcpy(ifr.ifr_name, "canX");
      ifr.ifr_name[3] = ('0' + i);

      if (ioctl(socketHandle[i], SIOCGIFINDEX, &ifr) == -1) {
        success = false;
        return;
      }

      sockaddr_can addr;
      memset(&addr, 0, sizeof(addr));
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;

      if (bind(socketHandle[i], reinterpret_cast<const sockaddr*>(&addr),
               sizeof(addr)) == -1) {
        success = false;
        return;
      }

      printf("Successfully bound to can interface %d\n", i);

      auto poll = wpi::uv::Poll::Create(loop, socketHandle[i]);
      if (!poll) {
        success = false;
        return;
      }

      poll->pollEvent.connect([fd = socketHandle[i]](int mask){
        if (mask & UV_READABLE) {
            canfd_frame frame;
            int rVal = read(fd, &frame, sizeof(frame));
            if (rVal <= 0) {
                // TODO error handling
                return;
            }
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
