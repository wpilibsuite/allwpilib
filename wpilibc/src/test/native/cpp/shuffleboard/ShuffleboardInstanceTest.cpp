/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardInstance.h"  // NOLINT(build/include_order)

#include <memory>
#include <string>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/shuffleboard/ShuffleboardInstance.h"
#include "gtest/gtest.h"
#include "shuffleboard/MockActuatorSendable.h"

using namespace frc;

class ShuffleboardInstanceTest : public testing::Test {
  void SetUp() override {
    m_ntInstance = nt::NetworkTableInstance::Create();
    m_shuffleboardInstance =
        std::make_unique<detail::ShuffleboardInstance>(m_ntInstance);
  }

 protected:
  nt::NetworkTableInstance m_ntInstance;
  std::unique_ptr<detail::ShuffleboardInstance> m_shuffleboardInstance;
};

TEST_F(ShuffleboardInstanceTest, PathFluent) {
  auto entry = m_shuffleboardInstance->GetTab("Tab Title")
                   .GetLayout("List Layout", "List")
                   .Add("Data", "string")
                   .WithWidget("Text View")
                   .GetEntry();

  EXPECT_EQ("string", entry.GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab Title/List Layout/Data", entry.GetName())
      << "Entry path generated incorrectly";
}

TEST_F(ShuffleboardInstanceTest, NestedLayoutsFluent) {
  auto entry = m_shuffleboardInstance->GetTab("Tab")
                   .GetLayout("First", "List")
                   .GetLayout("Second", "List")
                   .GetLayout("Third", "List")
                   .GetLayout("Fourth", "List")
                   .Add("Value", "string")
                   .GetEntry();

  EXPECT_EQ("string", entry.GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab/First/Second/Third/Fourth/Value",
            entry.GetName())
      << "Entry path generated incorrectly";
}

TEST_F(ShuffleboardInstanceTest, NestedLayoutsOop) {
  ShuffleboardTab& tab = m_shuffleboardInstance->GetTab("Tab");
  ShuffleboardLayout& first = tab.GetLayout("First", "List");
  ShuffleboardLayout& second = first.GetLayout("Second", "List");
  ShuffleboardLayout& third = second.GetLayout("Third", "List");
  ShuffleboardLayout& fourth = third.GetLayout("Fourth", "List");
  SimpleWidget& widget = fourth.Add("Value", "string");
  auto entry = widget.GetEntry();

  EXPECT_EQ("string", entry.GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab/First/Second/Third/Fourth/Value",
            entry.GetName())
      << "Entry path generated incorrectly";
}

TEST_F(ShuffleboardInstanceTest, LayoutTypeIsSet) {
  std::string layoutType = "Type";
  m_shuffleboardInstance->GetTab("Tab").GetLayout("Title", layoutType);
  m_shuffleboardInstance->Update();
  nt::NetworkTableEntry entry = m_ntInstance.GetEntry(
      "/Shuffleboard/.metadata/Tab/Title/PreferredComponent");
  EXPECT_EQ(layoutType, entry.GetString("Not Set")) << "Layout type not set";
}

TEST_F(ShuffleboardInstanceTest, NestedActuatorWidgetsAreDisabled) {
  MockActuatorSendable sendable("Actuator");
  m_shuffleboardInstance->GetTab("Tab")
      .GetLayout("Title", "Type")
      .Add(sendable);
  auto controllableEntry =
      m_ntInstance.GetEntry("/Shuffleboard/Tab/Title/Actuator/.controllable");
  m_shuffleboardInstance->Update();

  // Note: we use the unsafe `GetBoolean()` method because if the value is NOT
  // a boolean, or if it is not present, then something has clearly gone very,
  // very wrong
  bool controllable = controllableEntry.GetValue()->GetBoolean();
  // Sanity check
  EXPECT_TRUE(controllable)
      << "The nested actuator widget should be enabled by default";
  m_shuffleboardInstance->DisableActuatorWidgets();
  controllable = controllableEntry.GetValue()->GetBoolean();
  EXPECT_FALSE(controllable)
      << "The nested actuator widget should have been disabled";
}
