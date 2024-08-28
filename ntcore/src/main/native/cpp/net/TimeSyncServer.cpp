// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
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

struct TspPong {
  uint8_t version;
  uint8_t message_id;
  uint64_t client_time;
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

static_assert(wpi::StructSerializable<TspPing>);

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
        wpi::UnpackStruct<uint8_t, 0>(data),
        wpi::UnpackStruct<uint8_t, 1>(data),
        wpi::UnpackStruct<uint64_t, 2>(data),
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
      wpi::SmallVector<uint8_t, wpi::Struct<TspPing>::GetSize()> buffer;

      int n = m_udp.receive(buffer.data(), buffer.size(), &client_addr);

      if (n < 0) {
        std::perror("Failed to receive message");
        continue;
      }
      if (static_cast<size_t>(n) != buffer.size()) {
        std::perror("Didn't get enough bytes from client?");
        continue;
      }

      // Get the current time as a 64-bit integer
      uint64_t current_time = m_timeProvider();

      TspPong pong;

      m_udp.send(
          std::span<uint8_t>{reinterpret_cast<uint8_t*>(&pong), sizeof(pong)},
          client_addr);

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

    m_running.store(true);
    m_listener = std::thread{[this] { this->Go(); }};
  }
};
}  // namespace wpi

// #define PORT 5810
// #define BUFFER_SIZE 8

// uint64_t getCurrentTimeInMicroseconds() {
//   struct timeval tv;
//   gettimeofday(&tv, nullptr);
//   return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
// }

// uint64_t startTime = getCurrentTimeInMicroseconds();

// int main() {
//   int sockfd;
//   struct sockaddr_in server_addr, client_addr;
//   char buffer[BUFFER_SIZE];
//   socklen_t client_len = sizeof(client_addr);

//   // Create a UDP socket
//   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//     std::perror("Socket creation failed");
//     std::exit(EXIT_FAILURE);
//   }

//   // Set up the server address and bind it to the socket
//   std::memset(&server_addr, 0, sizeof(server_addr));
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_addr.s_addr = INADDR_ANY;
//   server_addr.sin_port = htons(PORT);

//   if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr))
//   <
//       0) {
//     std::perror("Bind failed");
//     close(sockfd);
//     std::exit(EXIT_FAILURE);
//   }

//   while (true) {
//     std::memset(buffer, 0, BUFFER_SIZE);
//     int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
//                      (struct sockaddr*)&client_addr, &client_len);

//     if (n < 0) {
//       std::perror("Failed to receive message");
//       continue;
//     }

//     // Get the current time as a 64-bit integer
//     uint64_t current_time = getCurrentTimeInMicroseconds() - startTime;

//     // Send the current time back to the client as a uint64_t
//     sendto(sockfd, &current_time, sizeof(current_time), 0,
//            (struct sockaddr*)&client_addr, client_len);

//     std::cout << "Sent current time (microseconds since epoch) to client: "
//               << current_time << std::endl;
//   }

//   // Close the socket
//   close(sockfd);
//   return 0;
// }
