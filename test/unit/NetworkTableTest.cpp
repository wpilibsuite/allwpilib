/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTable.h"

#include "gtest/gtest.h"

class NetworkTableTest : public ::testing::Test {};

TEST_F(NetworkTableTest, ContainsKey) {
  auto nt = NetworkTable::GetTable("containskey");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
}

TEST_F(NetworkTableTest, LeadingSlash) {
  auto nt = NetworkTable::GetTable("leadingslash");
  auto nt2 = NetworkTable::GetTable("/leadingslash");
  ASSERT_FALSE(nt->ContainsKey("testkey"));
  nt2->PutNumber("testkey", 5);
  ASSERT_TRUE(nt->ContainsKey("testkey"));
}
