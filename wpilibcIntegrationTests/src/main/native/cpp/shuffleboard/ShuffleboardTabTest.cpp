/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <array>
#include <memory>
#include <string>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/commands/InstantCommand.h"
#include "frc/shuffleboard/ShuffleboardInstance.h"
#include "frc/shuffleboard/ShuffleboardTab.h"
#include "frc/smartdashboard/Sendable.h"
#include "gtest/gtest.h"

using namespace frc;

class ShuffleboardTabTest : public testing::Test {
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

TEST_F(ShuffleboardTabTest, AddDouble) {
  auto entry = m_tab->Add("Double", 1.0).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/Double", entry.GetName());
  EXPECT_FLOAT_EQ(1.0, entry.GetValue()->GetDouble());
}

TEST_F(ShuffleboardTabTest, AddInteger) {
  auto entry = m_tab->Add("Int", 1).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/Int", entry.GetName());
  EXPECT_FLOAT_EQ(1.0, entry.GetValue()->GetDouble());
}

TEST_F(ShuffleboardTabTest, AddBoolean) {
  auto entry = m_tab->Add("Bool", false).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/Bool", entry.GetName());
  EXPECT_FALSE(entry.GetValue()->GetBoolean());
}

TEST_F(ShuffleboardTabTest, AddString) {
  auto entry = m_tab->Add("String", "foobar").GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/String", entry.GetName());
  EXPECT_EQ("foobar", entry.GetValue()->GetString());
}

TEST_F(ShuffleboardTabTest, AddNamedSendableWithProperties) {
  InstantCommand sendable("Command");
  std::string widgetType = "Command Widget";
  wpi::StringMap<std::shared_ptr<nt::Value>> map;
  map.try_emplace("foo", nt::Value::MakeDouble(1234));
  map.try_emplace("bar", nt::Value::MakeString("baz"));
  m_tab->Add(sendable).WithWidget(widgetType).WithProperties(map);

  m_instance->Update();
  std::string meta = "/Shuffleboard/.metadata/Tab/Command";

  EXPECT_EQ(1234, m_ntInstance.GetEntry(meta + "/Properties/foo").GetDouble(-1))
      << "Property 'foo' not set correctly";
  EXPECT_EQ("baz",
            m_ntInstance.GetEntry(meta + "/Properties/bar").GetString(""))
      << "Property 'bar' not set correctly";
  EXPECT_EQ(widgetType,
            m_ntInstance.GetEntry(meta + "/PreferredComponent").GetString(""))
      << "Preferred component not set correctly";
}

TEST_F(ShuffleboardTabTest, AddNumberArray) {
  std::array<double, 3> expect = {{1.0, 2.0, 3.0}};
  auto entry = m_tab->Add("DoubleArray", expect).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/DoubleArray", entry.GetName());

  auto actual = entry.GetValue()->GetDoubleArray();
  EXPECT_EQ(expect.size(), actual.size());
  for (size_t i = 0; i < expect.size(); i++) {
    EXPECT_FLOAT_EQ(expect[i], actual[i]);
  }
}

TEST_F(ShuffleboardTabTest, AddBooleanArray) {
  std::array<bool, 2> expect = {{true, false}};
  auto entry = m_tab->Add("BoolArray", expect).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/BoolArray", entry.GetName());

  auto actual = entry.GetValue()->GetBooleanArray();
  EXPECT_EQ(expect.size(), actual.size());
  for (size_t i = 0; i < expect.size(); i++) {
    EXPECT_EQ(expect[i], actual[i]);
  }
}

TEST_F(ShuffleboardTabTest, AddStringArray) {
  std::array<std::string, 2> expect = {{"foo", "bar"}};
  auto entry = m_tab->Add("StringArray", expect).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/StringArray", entry.GetName());

  auto actual = entry.GetValue()->GetStringArray();
  EXPECT_EQ(expect.size(), actual.size());
  for (size_t i = 0; i < expect.size(); i++) {
    EXPECT_EQ(expect[i], actual[i]);
  }
}
