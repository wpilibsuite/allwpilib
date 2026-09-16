// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "server/ServerSubscriber.hpp"

#include <limits>
#include <span>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "PubSubOptions.hpp"

namespace wpi::nt {

TEST_CASE("ServerSubscriberTest MaximumPeriodicInterval", "[ntcore][server]") {
  std::span<const std::string> noTopics;
  PubSubOptionsImpl options;
  options.periodicMs = std::numeric_limits<unsigned int>::max();

  SECTION("constructor") {
    server::ServerSubscriber subscriber{"client", noTopics, 1, options};
    CHECK(subscriber.GetPeriodMs() == 4294967290u);
  }

  SECTION("update") {
    server::ServerSubscriber subscriber{"client", noTopics, 1,
                                        PubSubOptionsImpl{}};
    subscriber.Update(noTopics, options);
    CHECK(subscriber.GetPeriodMs() == 4294967290u);
  }
}

}  // namespace wpi::nt
