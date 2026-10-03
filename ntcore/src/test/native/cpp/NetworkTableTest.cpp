// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NetworkTable.hpp"

#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestPrinters.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

TEST_CASE("NetworkTableTest BasenameKey", "[ntcore][network-table]") {
  CHECK("simple" == wpi::nt::NetworkTable::BasenameKey("simple"));
  CHECK("simple" == wpi::nt::NetworkTable::BasenameKey("one/two/many/simple"));
  CHECK("simple" ==
        wpi::nt::NetworkTable::BasenameKey("//////an/////awful/key////simple"));
}

TEST_CASE("NetworkTableTest NormalizeKeySlash", "[ntcore][network-table]") {
  CHECK("/" == wpi::nt::NetworkTable::NormalizeKey("///"));
  CHECK("/no/normal/req" ==
        wpi::nt::NetworkTable::NormalizeKey("/no/normal/req"));
  CHECK("/no/leading/slash" ==
        wpi::nt::NetworkTable::NormalizeKey("no/leading/slash"));
  CHECK("/what/an/awful/key/" == wpi::nt::NetworkTable::NormalizeKey(
                                     "//////what////an/awful/////key///"));
}

TEST_CASE("NetworkTableTest NormalizeKeyNoSlash", "[ntcore][network-table]") {
  CHECK("a" == wpi::nt::NetworkTable::NormalizeKey("a", false));
  CHECK("a" == wpi::nt::NetworkTable::NormalizeKey("///a", false));
  CHECK("leading/slash" ==
        wpi::nt::NetworkTable::NormalizeKey("/leading/slash", false));
  CHECK("no/leading/slash" ==
        wpi::nt::NetworkTable::NormalizeKey("no/leading/slash", false));
  CHECK("what/an/awful/key/" ==
        wpi::nt::NetworkTable::NormalizeKey("//////what////an/awful/////key///",
                                            false));
}

TEST_CASE("NetworkTableTest GetHierarchyEmpty", "[ntcore][network-table]") {
  std::vector<std::string> expected{"/"};
  REQUIRE(expected == wpi::nt::NetworkTable::GetHierarchy(""));
}

TEST_CASE("NetworkTableTest GetHierarchyRoot", "[ntcore][network-table]") {
  std::vector<std::string> expected{"/"};
  REQUIRE(expected == wpi::nt::NetworkTable::GetHierarchy("/"));
}

TEST_CASE("NetworkTableTest GetHierarchyNormal", "[ntcore][network-table]") {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/baz"};
  REQUIRE(expected == wpi::nt::NetworkTable::GetHierarchy("/foo/bar/baz"));
}

TEST_CASE("NetworkTableTest GetHierarchyTrailingSlash",
          "[ntcore][network-table]") {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/"};
  REQUIRE(expected == wpi::nt::NetworkTable::GetHierarchy("/foo/bar/"));
}

TEST_CASE("NetworkTableTest ContainsKey", "[ntcore][network-table]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  REQUIRE_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  REQUIRE(nt->ContainsKey("testkey"));
  REQUIRE(inst.GetEntry("/containskey/testkey").Exists());
  REQUIRE_FALSE(inst.GetEntry("containskey/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTableTest LeadingSlash", "[ntcore][network-table]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("leadingslash");
  auto nt2 = inst.GetTable("/leadingslash");
  REQUIRE_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  REQUIRE(nt->ContainsKey("testkey"));
  REQUIRE(inst.GetEntry("/leadingslash/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTableTest EmptyOrNoSlash", "[ntcore][network-table]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("/");
  auto nt2 = inst.GetTable("");
  REQUIRE_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  REQUIRE(nt->ContainsKey("testkey"));
  REQUIRE(inst.GetEntry("/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_CASE("NetworkTableTest ResetInstance", "[ntcore][network-table]") {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  REQUIRE_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  REQUIRE(nt->ContainsKey("testkey"));
  REQUIRE(inst.GetEntry("/containskey/testkey").Exists());
  wpi::nt::ResetInstance(inst.GetHandle());
  REQUIRE_FALSE(nt->ContainsKey("testkey"));
  wpi::nt::NetworkTableInstance::Destroy(inst);
}
