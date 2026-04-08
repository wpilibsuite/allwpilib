// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NetworkTable.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "TestPrinters.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

class NetworkTableTest : public ::testing::Test {};

TEST_F(NetworkTableTest, BasenameKey) {
  EXPECT_EQ("simple", wpi::nt::NetworkTable::BasenameKey("simple"));
  EXPECT_EQ("simple",
            wpi::nt::NetworkTable::BasenameKey("one/two/many/simple"));
  EXPECT_EQ("simple", wpi::nt::NetworkTable::BasenameKey(
                          "//////an/////awful/key////simple"));
}

TEST_F(NetworkTableTest, NormalizeKeySlash) {
  EXPECT_EQ("/", wpi::nt::NetworkTable::NormalizeKey("///"));
  EXPECT_EQ("/no/normal/req",
            wpi::nt::NetworkTable::NormalizeKey("/no/normal/req"));
  EXPECT_EQ("/no/leading/slash",
            wpi::nt::NetworkTable::NormalizeKey("no/leading/slash"));
  EXPECT_EQ("/what/an/awful/key/", wpi::nt::NetworkTable::NormalizeKey(
                                       "//////what////an/awful/////key///"));
}

TEST_F(NetworkTableTest, NormalizeKeyNoSlash) {
  EXPECT_EQ("a", wpi::nt::NetworkTable::NormalizeKey("a", false));
  EXPECT_EQ("a", wpi::nt::NetworkTable::NormalizeKey("///a", false));
  EXPECT_EQ("leading/slash",
            wpi::nt::NetworkTable::NormalizeKey("/leading/slash", false));
  EXPECT_EQ("no/leading/slash",
            wpi::nt::NetworkTable::NormalizeKey("no/leading/slash", false));
  EXPECT_EQ("what/an/awful/key/",
            wpi::nt::NetworkTable::NormalizeKey(
                "//////what////an/awful/////key///", false));
}

TEST_F(NetworkTableTest, GetHierarchyEmpty) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, wpi::nt::NetworkTable::GetHierarchy(""));
}

TEST_F(NetworkTableTest, GetHierarchyRoot) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, wpi::nt::NetworkTable::GetHierarchy("/"));
}

TEST_F(NetworkTableTest, GetHierarchyNormal) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/baz"};
  ASSERT_EQ(expected, wpi::nt::NetworkTable::GetHierarchy("/foo/bar/baz"));
}

TEST_F(NetworkTableTest, GetHierarchyTrailingSlash) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/"};
  ASSERT_EQ(expected, wpi::nt::NetworkTable::GetHierarchy("/foo/bar/"));
}

TEST_F(NetworkTableTest, ContainsKey) {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/containskey/testkey").Exists());
  ASSERT_FALSE(inst.GetEntry("containskey/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, LeadingSlash) {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("leadingslash");
  auto nt2 = inst.GetTable("/leadingslash");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/leadingslash/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, EmptyOrNoSlash) {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("/");
  auto nt2 = inst.GetTable("");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/testkey").Exists());
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, ResetInstance) {
  auto inst = wpi::nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/containskey/testkey").Exists());
  wpi::nt::ResetInstance(inst.GetHandle());
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  wpi::nt::NetworkTableInstance::Destroy(inst);
}
