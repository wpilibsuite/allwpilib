// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "server/ServerStorage.hpp"

#include <catch2/catch_test_macros.hpp>

#include "../MockLogger.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::nt;
using namespace wpi::nt::server;
using wpi::util::json;

TEST_CASE("ServerStorage property updates mark persistent data changed",
          "[ntcore][server][persistence]") {
  wpi::MockLogger logger;
  ServerStorage storage{logger, [](auto, auto) {}};
  auto topic = storage.CreateTopic(nullptr, "/test", "int",
                                   json::object("retained", true));
  storage.SetValue(nullptr, topic, Value::MakeInteger(42));
  storage.SetProperties(nullptr, topic, json::object("unit", "m"));
  CHECK_FALSE(storage.PersistentChanged());

  storage.SetProperties(nullptr, topic, json::object("persistent", true));
  CHECK(storage.PersistentChanged());
  CHECK_FALSE(storage.PersistentChanged());

  storage.SetProperties(nullptr, topic, json::object("unit", "cm"));
  CHECK(storage.PersistentChanged());
  storage.SetProperties(nullptr, topic, json::object("unit", nullptr));
  CHECK(storage.PersistentChanged());
  CHECK(topic->lastValue.GetInteger() == 42);

  storage.SetProperties(nullptr, topic, json::object("persistent", false));
  CHECK(storage.PersistentChanged());
  storage.SetProperties(nullptr, topic, json::object("unit", "mm"));
  CHECK_FALSE(storage.PersistentChanged());
}
