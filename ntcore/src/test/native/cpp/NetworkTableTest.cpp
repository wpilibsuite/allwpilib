// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "TestPrinters.h"
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"

class NetworkTableTest : public ::testing::Test {};

TEST_F(NetworkTableTest, BasenameKey) {
  EXPECT_EQ("simple", nt::NetworkTable::BasenameKey("simple"));
  EXPECT_EQ("simple", nt::NetworkTable::BasenameKey("one/two/many/simple"));
  EXPECT_EQ("simple",
            nt::NetworkTable::BasenameKey("//////an/////awful/key////simple"));
}

TEST_F(NetworkTableTest, NormalizeKeySlash) {
  EXPECT_EQ("/", nt::NetworkTable::NormalizeKey("///"));
  EXPECT_EQ("/no/normal/req", nt::NetworkTable::NormalizeKey("/no/normal/req"));
  EXPECT_EQ("/no/leading/slash",
            nt::NetworkTable::NormalizeKey("no/leading/slash"));
  EXPECT_EQ("/what/an/awful/key/", nt::NetworkTable::NormalizeKey(
                                       "//////what////an/awful/////key///"));
}

TEST_F(NetworkTableTest, NormalizeKeyNoSlash) {
  EXPECT_EQ("a", nt::NetworkTable::NormalizeKey("a", false));
  EXPECT_EQ("a", nt::NetworkTable::NormalizeKey("///a", false));
  EXPECT_EQ("leading/slash",
            nt::NetworkTable::NormalizeKey("/leading/slash", false));
  EXPECT_EQ("no/leading/slash",
            nt::NetworkTable::NormalizeKey("no/leading/slash", false));
  EXPECT_EQ("what/an/awful/key/",
            nt::NetworkTable::NormalizeKey("//////what////an/awful/////key///",
                                           false));
}

TEST_F(NetworkTableTest, GetHierarchyEmpty) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, nt::NetworkTable::GetHierarchy(""));
}

TEST_F(NetworkTableTest, GetHierarchyRoot) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, nt::NetworkTable::GetHierarchy("/"));
}

TEST_F(NetworkTableTest, GetHierarchyNormal) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/baz"};
  ASSERT_EQ(expected, nt::NetworkTable::GetHierarchy("/foo/bar/baz"));
}

TEST_F(NetworkTableTest, GetHierarchyTrailingSlash) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/"};
  ASSERT_EQ(expected, nt::NetworkTable::GetHierarchy("/foo/bar/"));
}

TEST_F(NetworkTableTest, ContainsKey) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/containskey/testkey").Exists());
  ASSERT_FALSE(inst.GetEntry("containskey/testkey").Exists());
  nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, LeadingSlash) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("leadingslash");
  auto nt2 = inst.GetTable("/leadingslash");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/leadingslash/testkey").Exists());
  nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, EmptyOrNoSlash) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("/");
  auto nt2 = inst.GetTable("");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/testkey").Exists());
  nt::NetworkTableInstance::Destroy(inst);
}

TEST_F(NetworkTableTest, ResetInstance) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/containskey/testkey").Exists());
  nt::ResetInstance(inst.GetHandle());
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt::NetworkTableInstance::Destroy(inst);
}
