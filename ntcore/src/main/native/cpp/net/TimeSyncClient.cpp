// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#define SERVER_IP \
  "127.0.0.1"  // Replace with the server's IP address if different
#define PORT 5810

inline uint64_t getCurrentTimeInMicroseconds() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

using std::cout, std::endl;

int main() {
  int sockfd;
  struct sockaddr_in server_addr;

  // Create a UDP socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    std::perror("Socket creation failed");
    std::exit(EXIT_FAILURE);
  }

  // Enable the SO_TIMESTAMP option
  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_TIMESTAMP, &enable, sizeof(enable)) <
      0) {
    std::perror("Setting SO_TIMESTAMP failed");
    close(sockfd);
    std::exit(EXIT_FAILURE);
  }

  // Set up the server address
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    std::perror("Invalid address/Address not supported");
    close(sockfd);
    std::exit(EXIT_FAILURE);
  }

  while (true) {
    // Get the current system time as a uint64_t
    uint64_t send_time = getCurrentTimeInMicroseconds();

    // Send the current time to the server
    sendto(sockfd, &send_time, sizeof(send_time), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    // std::cout << "Sent current time (microseconds since epoch): " <<
    // send_time << std::endl;
    uint64_t server_time{0};
    int len = recv(sockfd, &server_time, sizeof(server_time), 0);
    uint64_t rx_time = getCurrentTimeInMicroseconds();

    if (len != sizeof(server_time)) {
      cout << "Got only x bytes on recv: " << len << endl;
    }

    // when time = send_time+rtt2/2, server time = server time
    // server time = local time + offset
    // offset = (server time - local time) = (server time) - (send_time +
    // rtt2/2)
    auto rtt2 = rx_time - send_time;
    int64_t serverTimeOffsetUs = server_time - rtt2 / 2 - send_time;

    cout << "Ping-ponged! RTT " << (rx_time - send_time) << " uS, offset "
         << serverTimeOffsetUs << " uS" << endl;
    cout << "Estimated server time "
         << (getCurrentTimeInMicroseconds() + serverTimeOffsetUs) / 1000000.0
         << " s" << endl;

    // Sleep for 1 second before sending the next time
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Close the socket
  close(sockfd);
  return 0;
}
