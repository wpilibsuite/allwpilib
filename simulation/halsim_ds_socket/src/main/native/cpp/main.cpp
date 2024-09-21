// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------
**  This extension reimplements enough of the FRC_Network layer to enable the
**    simulator to communicate with a driver station.  That includes a
**    simple udp layer for communication.
**  The protocol does not appear to be well documented; this implementation
**    is based in part on the Toast ds_comms.cpp by Jaci and in part
**    by the protocol specification given by QDriverStation.
**--------------------------------------------------------------------------*/

#include <sys/types.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <exception>
#include <memory>
#include <string_view>

#include <DSCommPacket.h>
#include <hal/Extensions.h>
#include <wpi/print.h>
#include <wpinet/EventLoopRunner.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/Tcp.h>
#include <wpinet/uv/Timer.h>
#include <wpinet/uv/Udp.h>
#include <wpinet/uv/util.h>

#if defined(Win32) || defined(_WIN32)
#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace wpi::uv;

static std::unique_ptr<Buffer> singleByte;
static std::atomic<bool> gDSConnected = false;

namespace {
struct DataStore {
  wpi::SmallVector<uint8_t, 128> m_frame;
  size_t m_frameSize = (std::numeric_limits<size_t>::max)();
  halsim::DSCommPacket* dsPacket;
};
}  // namespace

static SimpleBufferPool<4>& GetBufferPool() {
  static SimpleBufferPool<4> bufferPool;
  return bufferPool;
}

static void HandleTcpDataStream(Buffer& buf, size_t size, DataStore& store) {
  std::string_view data{buf.base, size};
  while (!data.empty()) {
    if (store.m_frameSize == (std::numeric_limits<size_t>::max)()) {
      if (store.m_frame.size() < 2u) {
        size_t toCopy = (std::min)(2u - store.m_frame.size(), data.size());
        store.m_frame.append(data.data(), data.data() + toCopy);
        data.remove_prefix(toCopy);
        if (store.m_frame.size() < 2u) {
          return;  // need more data
        }
      }
      store.m_frameSize = (static_cast<uint16_t>(store.m_frame[0]) << 8) |
                          static_cast<uint16_t>(store.m_frame[1]);
    }
    if (store.m_frameSize != (std::numeric_limits<size_t>::max)()) {
      size_t need = store.m_frameSize - (store.m_frame.size() - 2);
      size_t toCopy = (std::min)(need, data.size());
      store.m_frame.append(data.data(), data.data() + toCopy);
      data.remove_prefix(toCopy);
      need -= toCopy;
      if (need == 0) {
        auto ds = store.dsPacket;
        ds->DecodeTCP(store.m_frame);
        store.m_frame.clear();
        store.m_frameSize = (std::numeric_limits<size_t>::max)();
      }
    }
  }
}

static void SetupTcp(wpi::uv::Loop& loop) {
  auto tcp = Tcp::Create(loop);
  auto tcpWaitTimer = Timer::Create(loop);

  auto recStore = std::make_shared<DataStore>();
  recStore->dsPacket = loop.GetData<halsim::DSCommPacket>().get();

  tcp->SetData(recStore);

  tcp->Bind("0.0.0.0", 1740);

  tcp->Listen([t = tcp.get()] {
    auto client = t->Accept();
    gDSConnected = true;

    client->data.connect([t](Buffer& buf, size_t len) {
      HandleTcpDataStream(buf, len, *t->GetData<DataStore>());
    });
    client->StartRead();
    client->end.connect([c = client.get()] {
      c->Close();
      gDSConnected = false;
    });
  });
}

static void SetupUdp(wpi::uv::Loop& loop) {
  auto udp = wpi::uv::Udp::Create(loop);
  udp->Bind("0.0.0.0", 1110);

  // Simulation mode packet
  auto simLoopTimer = Timer::Create(loop);
  struct sockaddr_in simAddr;
  NameToAddr("127.0.0.1", 1135, &simAddr);
  simLoopTimer->timeout.connect([udpLocal = udp.get(), simAddr] {
    udpLocal->Send(simAddr, {singleByte.get(), 1}, [](auto buf, Error err) {
      if (err) {
        wpi::print(stderr, "{}\n", err.str());
        std::fflush(stderr);
      }
    });
  });
  simLoopTimer->Start(Timer::Time{100}, Timer::Time{100});
  // DS Timeout
  int timeoutMs = 100;
  if (auto envTimeout = std::getenv("DS_TIMEOUT_MS")) {
    try {
      timeoutMs = std::stoi(envTimeout);
    } catch (const std::exception& e) {
      wpi::print(stderr, "Error parsing DS_TIMEOUT_MS: {}\n", e.what());
    }
  }
  auto autoDisableTimer = Timer::Create(loop);
  autoDisableTimer->timeout.connect([] { HALSIM_SetDriverStationEnabled(0); });

  // UDP Receive then send
  udp->received.connect(
      [udpLocal = udp.get(), autoDisableTimer, timeoutMs](
          Buffer& buf, size_t len, const sockaddr& recSock, unsigned int port) {
        autoDisableTimer->Start(Timer::Time(timeoutMs));
        auto ds = udpLocal->GetLoop()->GetData<halsim::DSCommPacket>();
        ds->DecodeUDP({reinterpret_cast<uint8_t*>(buf.base), len});

        struct sockaddr_in outAddr;
        std::memcpy(&outAddr, &recSock, sizeof(sockaddr_in));
        outAddr.sin_family = PF_INET;
        outAddr.sin_port = htons(1150);

        wpi::SmallVector<wpi::uv::Buffer, 4> sendBufs;
        wpi::raw_uv_ostream stream{sendBufs,
                                   [] { return GetBufferPool().Allocate(); }};
        ds->SetupSendBuffer(stream);

        udpLocal->Send(outAddr, sendBufs, [](auto bufs, Error err) {
          GetBufferPool().Release(bufs);
          if (err) {
            wpi::print(stderr, "{}\n", err.str());
            std::fflush(stderr);
          }
        });
        ds->SendUDPToHALSim();
      });

  udp->StartRecv();
}

static void SetupEventLoop(wpi::uv::Loop& loop) {
  auto loopData = std::make_shared<halsim::DSCommPacket>();
  loop.SetData(loopData);
  SetupUdp(loop);
  SetupTcp(loop);
}

static std::unique_ptr<wpi::EventLoopRunner> eventLoopRunner;

/*----------------------------------------------------------------------------
** Main entry point.  We will start listen threads going, processing
**  against our driver station packet
**--------------------------------------------------------------------------*/
extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  static bool once = false;

  if (once) {
    std::fputs("Error: cannot invoke HALSIM_InitExtension twice.\n", stderr);
    return -1;
  }
  once = true;

  std::puts("DriverStationSocket Initializing.");

  HAL_RegisterExtension("ds_socket", &gDSConnected);

  singleByte = std::make_unique<Buffer>("0");

  eventLoopRunner = std::make_unique<wpi::EventLoopRunner>();

  eventLoopRunner->ExecAsync(SetupEventLoop);

  std::puts("DriverStationSocket Initialized!");
  return 0;
}
}  // extern "C"
