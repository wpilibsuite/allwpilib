// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/shuffleboard/ShuffleboardInstance.h"
#include "frc/shuffleboard/ShuffleboardTab.h"
#include "gtest/gtest.h"

using namespace frc;

class SuppliedValueWidgetTest : public testing::Test {
  void SetUp() override {
    m_ntInstance = nt::NetworkTableInstance::Create();
    m_instance = std::make_unique<detail::ShuffleboardInstance>(m_ntInstance);
    m_tab = &(m_instance->GetTab("Tab"));
  }

 protected:
  nt::NetworkTableInstance m_ntInstance;
  ShuffleboardTab* m_tab;
  std::unique_ptr<detail::ShuffleboardInstance> m_instance;
};

TEST_F(SuppliedValueWidgetTest, AddString) {
  std::string str = "foo";
  m_tab->AddString("String", [&str]() { return str; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/String");

  m_instance->Update();
  EXPECT_EQ("foo", entry.GetValue()->GetString());
}

TEST_F(SuppliedValueWidgetTest, AddNumber) {
  int num = 0;
  m_tab->AddNumber("Num", [&num]() { return ++num; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Num");

  m_instance->Update();
  EXPECT_FLOAT_EQ(1.0, entry.GetValue()->GetDouble());
}

TEST_F(SuppliedValueWidgetTest, AddBoolean) {
  bool value = true;
  m_tab->AddBoolean("Bool", [&value]() { return value; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Bool");

  m_instance->Update();
  EXPECT_EQ(true, entry.GetValue()->GetBoolean());
}

TEST_F(SuppliedValueWidgetTest, AddStringArray) {
  std::vector<std::string> strings = {"foo", "bar"};
  m_tab->AddStringArray("Strings", [&strings]() { return strings; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Strings");

  m_instance->Update();
  auto actual = entry.GetValue()->GetStringArray();

  EXPECT_EQ(strings.size(), actual.size());
  for (size_t i = 0; i < strings.size(); i++) {
    EXPECT_EQ(strings[i], actual[i]);
  }
}

TEST_F(SuppliedValueWidgetTest, AddNumberArray) {
  std::vector<double> nums = {0, 1, 2, 3};
  m_tab->AddNumberArray("Numbers", [&nums]() { return nums; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Numbers");

  m_instance->Update();
  auto actual = entry.GetValue()->GetDoubleArray();

  EXPECT_EQ(nums.size(), actual.size());
  for (size_t i = 0; i < nums.size(); i++) {
    EXPECT_FLOAT_EQ(nums[i], actual[i]);
  }
}

TEST_F(SuppliedValueWidgetTest, AddBooleanArray) {
  std::vector<int> bools = {true, false};
  m_tab->AddBooleanArray("Booleans", [&bools]() { return bools; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Booleans");

  m_instance->Update();
  auto actual = entry.GetValue()->GetBooleanArray();

  EXPECT_EQ(bools.size(), actual.size());
  for (size_t i = 0; i < bools.size(); i++) {
    EXPECT_FLOAT_EQ(bools[i], actual[i]);
  }
}

TEST_F(SuppliedValueWidgetTest, AddRaw) {
  wpi::StringRef bytes = "\1\2\3";
  m_tab->AddRaw("Raw", [&bytes]() { return bytes; });
  auto entry = m_ntInstance.GetEntry("/Shuffleboard/Tab/Raw");

  m_instance->Update();
  auto actual = entry.GetValue()->GetRaw();
  EXPECT_EQ(bytes, actual);
}
