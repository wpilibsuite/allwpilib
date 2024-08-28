// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>

#include <wpi/Logger.h>
#include <wpi/print.h>
#include <wpi/struct/Struct.h>
#include <wpinet/UDPClient.h>

#include "ntcore_cpp.h"

struct TspPing {
  uint8_t version;
  uint8_t message_id;
  uint64_t client_time;
};

struct TspPong : public TspPing {
  TspPong(TspPing ping, uint64_t servertime)
      : TspPing{ping}, server_time{servertime} {}
  uint64_t server_time;
};

template <>
struct wpi::Struct<TspPing> {
  static constexpr std::string_view GetTypeName() { return "TspPing"; }
  static constexpr size_t GetSize() { return 10; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time";
  }

  static TspPing Unpack(std::span<const uint8_t> data) {
    return TspPing{
        wpi::UnpackStruct<uint8_t, 0>(data),
        wpi::UnpackStruct<uint8_t, 1>(data),
        wpi::UnpackStruct<uint64_t, 2>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const TspPing& value) {
    wpi::PackStruct<0>(data, value.version);
    wpi::PackStruct<1>(data, value.message_id);
    wpi::PackStruct<2>(data, value.client_time);
  }
};

template <>
struct wpi::Struct<TspPong> {
  static constexpr std::string_view GetTypeName() { return "TspPong"; }
  static constexpr size_t GetSize() { return 18; }
  static constexpr std::string_view GetSchema() {
    return "uint8 version;uint8 message_id;uint64 client_time;uint64_t "
           "server_time";
  }

  static TspPong Unpack(std::span<const uint8_t> data) {
    return TspPong{
        TspPing{
            wpi::UnpackStruct<uint8_t, 0>(data),
            wpi::UnpackStruct<uint8_t, 1>(data),
            wpi::UnpackStruct<uint64_t, 2>(data),
        },
        wpi::UnpackStruct<uint64_t, 10>(data),
    };
  }
  static void Pack(std::span<uint8_t> data, const TspPong& value) {
    wpi::PackStruct<0>(data, value.version);
    wpi::PackStruct<1>(data, value.message_id);
    wpi::PackStruct<2>(data, value.client_time);
    wpi::PackStruct<10>(data, value.server_time);
  }
};

static_assert(wpi::StructSerializable<TspPong>);
static_assert(wpi::StructSerializable<TspPing>);

static void LoggerFunc(unsigned int level, const char* file, unsigned int line,
                       const char* msg) {
  if (level == 20) {
    wpi::print(stderr, "DS: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "DS: {}: {} ({}:{})\n", levelmsg, msg, file, line);
}

namespace wpi {

class TimeSyncServer {
  wpi::Logger m_logger{::LoggerFunc};
  std::function<uint64_t()> m_timeProvider;
  UDPClient m_udp;

  std::atomic_bool m_running;
  std::thread m_listener;

 private:
  void Go() {
    struct sockaddr_in client_addr;

    while (m_running) {
      wpi::SmallVector<uint8_t, wpi::Struct<TspPing>::GetSize()> pingData;

      int n = m_udp.receive(pingData.data(), pingData.size(), &client_addr);

      if (n < 0) {
        std::perror("Failed to receive message");
        continue;
      }
      if (static_cast<size_t>(n) != pingData.size()) {
        std::perror("Didn't get enough bytes from client?");
        continue;
      }

      TspPing ping{
          wpi::UnpackStruct<TspPing>(pingData),
      };

      if (ping.version != 1) {
        std::perror("Bad version from client?");
      }
      if (ping.message_id != 1) {
        std::perror("Bad message id from client?");
      }

      uint64_t current_time = m_timeProvider();

      TspPong pong{ping, current_time};
      pong.message_id = 2;

      wpi::SmallVector<uint8_t, wpi::Struct<TspPong>::GetSize()> pongData;
      wpi::PackStruct(pongData, pong);

      m_udp.send(std::span<uint8_t>{pongData}, client_addr);

      std::cout << "Sent current time (microseconds since epoch) to client: "
                << current_time << std::endl;
    }

    // Close the socket
    m_udp.shutdown();
  }

 public:
  TimeSyncServer(int port = 5810,
                 std::function<uint64_t()> timeProvider = nt::Now)
      : m_timeProvider(timeProvider), m_udp("", m_logger) {
    // This will bind to "port". Set port to 0 if you don't care about the local
    // port it binds to
    m_udp.start(port);
  }

  void Start() {
    m_running.store(true);
    m_listener = std::thread{[this] { this->Go(); }};
  }

  void Stop() {
    m_running.store(false);
    m_listener.join();
  }
};

class TimeSyncClient {
  wpi::Logger m_logger{::LoggerFunc};
  std::function<uint64_t()> m_timeProvider;
  UDPClient m_udp;

  std::string m_serverIP;
  int m_serverPort;
  std::chrono::milliseconds m_loopDelay;

  std::atomic_bool m_running;
  std::thread m_listener;

  int64_t m_offset{0};
  std::mutex m_offsetMutex;

 private:
  void Go(std::chrono::milliseconds loop_time) {
    struct sockaddr_in client_addr;

    while (m_running) {
      auto start{std::chrono::high_resolution_clock::now()};

      // Send a ping to the server
      uint64_t ping_local_time{m_timeProvider()};

      TspPing ping{
          .version = 1, .message_id = 1, .client_time = ping_local_time};

      wpi::SmallVector<uint8_t, wpi::Struct<TspPing>::GetSize()> pingData;
      wpi::PackStruct(pingData, ping);

      // TODO: cache sockaddr instead of recalculating?
      int n =
          m_udp.send(std::span<uint8_t>{pingData}, m_serverIP, m_serverPort);

      if (n < 0) {
        std::perror("Failed to send ping");
        continue;
      }

      // wait for our pong
      wpi::SmallVector<uint8_t, wpi::Struct<TspPong>::GetSize()> pongData;
      int n2 = m_udp.receive(pongData.data(), pongData.size(), &client_addr);
      uint64_t pong_local_time = m_timeProvider();

      if (n2 < 0) {
        std::perror("Failed to receive message");
        continue;
      }
      if (static_cast<size_t>(n2) != pongData.size()) {
        std::perror("Didn't get enough bytes from client?");
        continue;
      }

      TspPong pong{
          wpi::UnpackStruct<TspPong>(pingData),
      };

      if (pong.version != 1) {
        std::perror("Bad version from client?");
      }
      if (pong.message_id != 2) {
        std::perror("Bad message id from client?");
      }

      // when time = send_time+rtt2/2, server time = server time
      // server time = local time + offset
      // offset = (server time - local time) = (server time) - (send_time +
      // rtt2/2)
      auto rtt2 = pong_local_time - ping_local_time;
      int64_t serverTimeOffsetUs =
          pong.server_time - rtt2 / 2 - ping_local_time;

      using std::cout;
      fmt::println("Ping-ponged! RTT2 {} uS, offset {} uS", rtt2,
                   serverTimeOffsetUs);
      fmt::println("Estimated server time {} s",
                   (m_timeProvider() + serverTimeOffsetUs) / 1000000.0);

      auto end{std::chrono::high_resolution_clock::now()};
      auto sleep_duration = m_loopDelay - (end - start);
      std::this_thread::sleep_for(sleep_duration);
    }

    // Close the socket
    m_udp.shutdown();
  }

 public:
  TimeSyncClient(std::string_view server, int remote_port,
                 std::chrono::milliseconds ping_delay,
                 std::function<uint64_t()> timeProvider = nt::Now)
      : m_timeProvider(timeProvider),
        m_udp("", m_logger),
        m_serverIP{server},
        m_serverPort{remote_port},
        m_loopDelay(ping_delay) {
    // we don't care about the local port. In fact if i refactor udpclient, i
    // could just implicitly bind
    m_udp.start(0);

    // Set a super conservative 1-second timeout by default
    m_udp.set_timeout(1);
  }

  void Start() {
    m_running.store(true);
    m_listener = std::thread{[this] { this->Go(m_loopDelay); }};
  }

  void Stop() {
    m_running.store(false);
    m_listener.join();
  }

  int64_t GetOffset() {
    std::lock_guard lock{m_offsetMutex};
    return m_offset;
  }
};

}  // namespace wpi
