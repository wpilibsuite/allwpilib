/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TestPrinters.h"
#include "gtest/gtest.h"
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"

class NetworkTableTest : public ::testing::Test {};

TEST_F(NetworkTableTest, BasenameKey) {
  EXPECT_EQ("simple", NetworkTable::BasenameKey("simple"));
  EXPECT_EQ("simple", NetworkTable::BasenameKey("one/two/many/simple"));
  EXPECT_EQ("simple",
            NetworkTable::BasenameKey("//////an/////awful/key////simple"));
}

TEST_F(NetworkTableTest, NormalizeKeySlash) {
  EXPECT_EQ("/", NetworkTable::NormalizeKey("///"));
  EXPECT_EQ("/no/normal/req", NetworkTable::NormalizeKey("/no/normal/req"));
  EXPECT_EQ("/no/leading/slash",
            NetworkTable::NormalizeKey("no/leading/slash"));
  EXPECT_EQ("/what/an/awful/key/",
            NetworkTable::NormalizeKey("//////what////an/awful/////key///"));
}

TEST_F(NetworkTableTest, NormalizeKeyNoSlash) {
  EXPECT_EQ("a", NetworkTable::NormalizeKey("a", false));
  EXPECT_EQ("a", NetworkTable::NormalizeKey("///a", false));
  EXPECT_EQ("leading/slash",
            NetworkTable::NormalizeKey("/leading/slash", false));
  EXPECT_EQ("no/leading/slash",
            NetworkTable::NormalizeKey("no/leading/slash", false));
  EXPECT_EQ(
      "what/an/awful/key/",
      NetworkTable::NormalizeKey("//////what////an/awful/////key///", false));
}

TEST_F(NetworkTableTest, GetHierarchyEmpty) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, NetworkTable::GetHierarchy(""));
}

TEST_F(NetworkTableTest, GetHierarchyRoot) {
  std::vector<std::string> expected{"/"};
  ASSERT_EQ(expected, NetworkTable::GetHierarchy("/"));
}

TEST_F(NetworkTableTest, GetHierarchyNormal) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/baz"};
  ASSERT_EQ(expected, NetworkTable::GetHierarchy("/foo/bar/baz"));
}

TEST_F(NetworkTableTest, GetHierarchyTrailingSlash) {
  std::vector<std::string> expected{"/", "/foo", "/foo/bar", "/foo/bar/"};
  ASSERT_EQ(expected, NetworkTable::GetHierarchy("/foo/bar/"));
}

TEST_F(NetworkTableTest, ContainsKey) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/containskey/testkey").Exists());
  ASSERT_FALSE(inst.GetEntry("containskey/testkey").Exists());
}

TEST_F(NetworkTableTest, LeadingSlash) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("leadingslash");
  auto nt2 = inst.GetTable("/leadingslash");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/leadingslash/testkey").Exists());
}

TEST_F(NetworkTableTest, EmptyOrNoSlash) {
  auto inst = nt::NetworkTableInstance::Create();
  auto nt = inst.GetTable("/");
  auto nt2 = inst.GetTable("");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
  ASSERT_TRUE(inst.GetEntry("/testkey").Exists());
}
