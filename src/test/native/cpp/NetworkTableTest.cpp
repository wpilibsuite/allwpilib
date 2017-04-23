/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"

#include "gtest/gtest.h"
#include "TestPrinters.h"

class NetworkTableTest : public ::testing::Test {};

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
