// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <gtest/gtest.h>

#include "../CommandTestBase.h"
#include "frc2/command/button/Trigger.h"
#include "frc2/command/button/TriggerGroup.h"

using namespace frc2;

class TriggerGroupTest : public CommandTestBase {};

TEST_F(TriggerGroupTest, Add) {
  TriggerGroup group1;

  TriggerGroup group2 = group1.Add("a", Trigger());

  EXPECT_NE(&group1, &group2);

  EXPECT_THROW(group2.Add("a", Trigger()), std::invalid_argument);

  EXPECT_NO_THROW(group2.Only({"a"}));
}

TEST_F(TriggerGroupTest, Only) {
  TriggerGroup group = TriggerGroup({{"a", Trigger([] { return true; })},
                                     {"b", Trigger([] { return true; })},
                                     {"c", Trigger([] { return false; })}});

  EXPECT_TRUE(group.Only({"a", "b"}).Get());
  EXPECT_FALSE(group.Only({"a"}).Get());
  EXPECT_FALSE(group.Only({"a", "c"}).Get());
  EXPECT_FALSE(group.Only({"a", "b", "c"}).Get());
}

TEST_F(TriggerGroupTest, Any) {
  TriggerGroup group = TriggerGroup({{"a", Trigger([] { return true; })},
                                     {"b", Trigger([] { return true; })},
                                     {"c", Trigger([] { return false; })}});

  EXPECT_TRUE(group.Any({"a", "b"}).Get());
  EXPECT_TRUE(group.Any({"a", "c"}).Get());
  EXPECT_FALSE(group.Any({"c"}).Get());
}

TEST_F(TriggerGroupTest, AllOf) {
  TriggerGroup group = TriggerGroup({{"a", Trigger([] { return true; })},
                                     {"b", Trigger([] { return true; })},
                                     {"c", Trigger([] { return false; })}});

  EXPECT_TRUE(group.AllOf({"a", "b"}).Get());
  EXPECT_TRUE(group.AllOf({"a"}).Get());
  EXPECT_FALSE(group.Any({"c"}).Get());
}

TEST_F(TriggerGroupTest, All) {
  TriggerGroup group1 = TriggerGroup({
      {"a", Trigger([] { return true; })},
      {"b", Trigger([] { return false; })},
  });

  TriggerGroup group2 = TriggerGroup({
      {"a", Trigger([] { return true; })},
      {"b", Trigger([] { return true; })},
  });

  EXPECT_FALSE(group1.All().Get());
  EXPECT_TRUE(group2.All().Get());
}

TEST_F(TriggerGroupTest, None) {
  TriggerGroup group1 = TriggerGroup({
      {"a", Trigger([] { return true; })},
      {"b", Trigger([] { return false; })},
  });

  TriggerGroup group2 = TriggerGroup({
      {"a", Trigger([] { return false; })},
      {"b", Trigger([] { return false; })},
  });

  EXPECT_FALSE(group1.None().Get());
  EXPECT_TRUE(group2.None().Get());
}

TEST_F(TriggerGroupTest, InvalidName) {
  TriggerGroup group = TriggerGroup({{"a", Trigger([] { return true; })}});

  EXPECT_THROW(group.Only({"bogus"}), std::invalid_argument);
}
