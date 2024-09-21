// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/ShuffleboardInstance.h"  // NOLINT(build/include_order)

#include <string_view>

#include <gtest/gtest.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <networktables/StringTopic.h>

#include "shuffleboard/MockActuatorSendable.h"

class NTWrapper {
 public:
  NTWrapper() { inst = nt::NetworkTableInstance::Create(); }

  ~NTWrapper() { nt::NetworkTableInstance::Destroy(inst); }

  nt::NetworkTableInstance inst;
};

TEST(ShuffleboardInstanceTest, PathFluent) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};

  auto entry = shuffleboardInst.GetTab("Tab Title")
                   .GetLayout("List", "List Layout")
                   .Add("Data", "string")
                   .WithWidget("Text View")
                   .GetEntry();

  EXPECT_EQ("string", entry->GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab Title/List/Data", entry->GetTopic().GetName())
      << "Entry path generated incorrectly";
}

TEST(ShuffleboardInstanceTest, NestedLayoutsFluent) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};

  auto entry = shuffleboardInst.GetTab("Tab")
                   .GetLayout("First", "List")
                   .GetLayout("Second", "List")
                   .GetLayout("Third", "List")
                   .GetLayout("Fourth", "List")
                   .Add("Value", "string")
                   .GetEntry();

  EXPECT_EQ("string", entry->GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab/First/Second/Third/Fourth/Value",
            entry->GetTopic().GetName())
      << "Entry path generated incorrectly";
}

TEST(ShuffleboardInstanceTest, NestedLayoutsOop) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};

  frc::ShuffleboardTab& tab = shuffleboardInst.GetTab("Tab");
  frc::ShuffleboardLayout& first = tab.GetLayout("First", "List");
  frc::ShuffleboardLayout& second = first.GetLayout("Second", "List");
  frc::ShuffleboardLayout& third = second.GetLayout("Third", "List");
  frc::ShuffleboardLayout& fourth = third.GetLayout("Fourth", "List");
  frc::SimpleWidget& widget = fourth.Add("Value", "string");
  auto entry = widget.GetEntry();

  EXPECT_EQ("string", entry->GetString("")) << "Wrong entry value";
  EXPECT_EQ("/Shuffleboard/Tab/First/Second/Third/Fourth/Value",
            entry->GetTopic().GetName())
      << "Entry path generated incorrectly";
}

TEST(ShuffleboardInstanceTest, LayoutTypeIsSet) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};

  std::string_view layoutType = "Type";
  shuffleboardInst.GetTab("Tab").GetLayout("Title", layoutType);
  shuffleboardInst.Update();
  auto entry = ntInst.inst.GetEntry(
      "/Shuffleboard/.metadata/Tab/Title/PreferredComponent");
  EXPECT_EQ(layoutType, entry.GetString("Not Set")) << "Layout type not set";
}

TEST(ShuffleboardInstanceTest, NestedActuatorWidgetsAreDisabled) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};

  MockActuatorSendable sendable("Actuator");
  shuffleboardInst.GetTab("Tab").GetLayout("Title", "Layout").Add(sendable);
  auto controllableEntry =
      ntInst.inst.GetEntry("/Shuffleboard/Tab/Title/Actuator/.controllable");
  shuffleboardInst.Update();

  // Note: we use the unsafe `GetBoolean()` method because if the value is NOT
  // a boolean, or if it is not present, then something has clearly gone very,
  // very wrong
  bool controllable = controllableEntry.GetValue().GetBoolean();
  // Sanity check
  EXPECT_TRUE(controllable)
      << "The nested actuator widget should be enabled by default";
  shuffleboardInst.DisableActuatorWidgets();
  controllable = controllableEntry.GetValue().GetBoolean();
  EXPECT_FALSE(controllable)
      << "The nested actuator widget should have been disabled";
}

TEST(ShuffleboardInstanceTest, DuplicateSelectTabs) {
  NTWrapper ntInst;
  frc::detail::ShuffleboardInstance shuffleboardInst{ntInst.inst};
  std::atomic_int counter = 0;
  auto subscriber =
      ntInst.inst.GetStringTopic("/Shuffleboard/.metadata/Selected")
          .Subscribe("", {.keepDuplicates = true});
  ntInst.inst.AddListener(
      subscriber, nt::EventFlags::kValueAll | nt::EventFlags::kImmediate,
      [&counter](auto& event) { counter++; });

  // There shouldn't be anything there
  EXPECT_EQ(0, counter);

  shuffleboardInst.SelectTab("tab1");
  shuffleboardInst.SelectTab("tab1");
  EXPECT_TRUE(ntInst.inst.WaitForListenerQueue(1.0))
      << "Listener queue timed out!";
  EXPECT_EQ(2, counter);
}
