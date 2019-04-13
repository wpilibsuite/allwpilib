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
#include "frc/shuffleboard/BuiltInWidgets.h"
#include "frc/shuffleboard/ShuffleboardInstance.h"
#include "frc/shuffleboard/ShuffleboardTab.h"
#include "frc/shuffleboard/ShuffleboardWidget.h"
#include "frc/smartdashboard/Sendable.h"
#include "gtest/gtest.h"

using namespace frc;

class ShuffleboardWidgetTest : public testing::Test {
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

TEST_F(ShuffleboardWidgetTest, UseBuiltInWidget) {
  auto entry =
      m_tab->Add("Name", "").WithWidget(BuiltInWidgets::kTextView).GetEntry();
  EXPECT_EQ("/Shuffleboard/Tab/Name", entry.GetName())
      << "The widget entry has the wrong name";
}

TEST_F(ShuffleboardWidgetTest, WithProperties) {
  wpi::StringMap<std::shared_ptr<nt::Value>> properties{
      std::make_pair("min", nt::Value::MakeDouble(0)),
      std::make_pair("max", nt::Value::MakeDouble(1))};
  auto entry =
      m_tab->Add("WithProperties", "").WithProperties(properties).GetEntry();

  // Update the instance to generate
  // the metadata entries for the widget properties
  m_instance->Update();

  auto propertiesTable = m_ntInstance.GetTable(
      "/Shuffleboard/.metadata/Tab/WithProperties/Properties");

  EXPECT_EQ("/Shuffleboard/Tab/WithProperties", entry.GetName())
      << "The widget entry has the wrong name";
  EXPECT_FLOAT_EQ(0, propertiesTable->GetEntry("min").GetDouble(-1))
      << "The 'min' property should be 0";
  EXPECT_FLOAT_EQ(1, propertiesTable->GetEntry("max").GetDouble(-1))
      << "The 'max' property should be 1";
}
