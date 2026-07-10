// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/net/MulticastServiceAnnouncer.hpp"
#include "wpi/net/MulticastServiceResolver.hpp"

TEST_CASE("MulticastServiceAnnouncerTest EmptyText",
          "[multicast][service-discovery]") {
  const std::string_view serviceName = "TestServiceNoText";
  const std::string_view serviceType = "_wpinotxt._tcp";
  const int port = std::rand();
  wpi::net::MulticastServiceAnnouncer announcer(serviceName, serviceType, port);
  wpi::net::MulticastServiceResolver resolver(serviceType);

  if (announcer.HasImplementation() && resolver.HasImplementation()) {
    announcer.Start();
    resolver.Start();

    std::vector<wpi::net::MulticastServiceResolver::ServiceData> allData;

    for (int i = 0; i < 15; i++) {
      // GetData gives me new data since last time. This smoketest is just
      // looking for -any- response
      allData = resolver.GetData();
      if (!allData.empty()) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    REQUIRE(allData.size() > 0ul);

    resolver.Stop();
    announcer.Stop();
  }
}

TEST_CASE("MulticastServiceAnnouncerTest SingleText",
          "[multicast][service-discovery]") {
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
