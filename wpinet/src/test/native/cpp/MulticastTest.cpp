// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/net/MulticastServiceAnnouncer.h"
#include "wpi/net/MulticastServiceResolver.h"
#include "wpi/util/timestamp.h"

namespace {

// Convert a host-order ipv4 address to string
static std::string ipv4ToString(uint32_t addr) {
  char buffer[16];
  std::snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u", (addr >> 24) & 0xFF,
                (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);
  return std::string(buffer);
}

}  // namespace

TEST(MulticastServiceAnnouncerTest, EmptyText) {
  const std::string_view serviceName = "TestServiceNoText";
  const std::string_view serviceType = "_wpinotxt._tcp";
  const int port = std::rand();
  wpi::net::MulticastServiceAnnouncer announcer(serviceName, serviceType, port);
  wpi::net::MulticastServiceResolver resolver(serviceType);

  if (announcer.HasImplementation() && resolver.HasImplementation()) {
    announcer.Start();
    resolver.Start();

    std::vector<wpi::net::MulticastServiceResolver::ServiceData> allData{};

    for (int i = 0; i < 15; i++) {
      auto data = resolver.GetData();

      printf("Got %zu data\n", data.size());
      for (const auto& it : data) {
        std::string ipv4 = ipv4ToString(it.ipv4Address);

        printf("service %s at host %s ipv4 %s\n", it.serviceName.c_str(),
               it.hostName.c_str(), ipv4.c_str());

        allData.push_back(it);
      }

      if (!allData.empty()) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    printf("Ending with %zu\n", allData.size());
    ASSERT_GT(allData.size(), 0ul);

    resolver.Stop();
    announcer.Stop();
  }
}

TEST(MulticastServiceAnnouncerTest, SingleText) {
  const std::string_view serviceName = "TestServiceSingle";
  const std::string_view serviceType = "_wpitxt";
  const int port = std::rand();
  std::array<std::pair<std::string, std::string>, 1> txt = {
      std::pair{"hello", "world"}};
  wpi::net::MulticastServiceAnnouncer announcer(serviceName, serviceType, port,
                                                txt);
  wpi::net::MulticastServiceResolver resolver(serviceType);

  if (announcer.HasImplementation() && resolver.HasImplementation()) {
    announcer.Start();
    resolver.Start();

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
