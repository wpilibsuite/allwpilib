// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#define PORT 5810
#define BUFFER_SIZE 8

uint64_t getCurrentTimeInMicroseconds() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

uint64_t startTime = getCurrentTimeInMicroseconds();

int main() {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[BUFFER_SIZE];
  socklen_t client_len = sizeof(client_addr);

  // Create a UDP socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    std::perror("Socket creation failed");
    std::exit(EXIT_FAILURE);
  }

  // Set up the server address and bind it to the socket
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    std::perror("Bind failed");
    close(sockfd);
    std::exit(EXIT_FAILURE);
  }

  while (true) {
    std::memset(buffer, 0, BUFFER_SIZE);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr*)&client_addr, &client_len);

    if (n < 0) {
      std::perror("Failed to receive message");
      continue;
    }

    // Get the current time as a 64-bit integer
    uint64_t current_time = getCurrentTimeInMicroseconds() - startTime;

    // Send the current time back to the client as a uint64_t
    sendto(sockfd, &current_time, sizeof(current_time), 0,
           (struct sockaddr*)&client_addr, client_len);

    std::cout << "Sent current time (microseconds since epoch) to client: "
              << current_time << std::endl;
  }

  // Close the socket
  close(sockfd);
  return 0;
}
