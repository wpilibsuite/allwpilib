// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ds_socket/DSCommPacket.hpp"

#include <gtest/gtest.h>

class DSCommPacketTest : public ::testing::Test {
 public:
  DSCommPacketTest() = default;

  void SendJoysticks() { commPacket.SendJoysticks(); }

  halsim::DSCommJoystickPacket& ReadJoystickTag(std::span<const uint8_t> data,
                                                int index) {
    commPacket.ReadJoystickTag(data, index);
    return commPacket.m_joystick_packets[index];
  }

  halsim::DSCommJoystickPacket& ReadDescriptorTag(
      std::span<const uint8_t> data) {
    commPacket.ReadJoystickDescriptionTag(data);
    return commPacket.m_joystick_packets[data[3]];
  }

  HAL_MatchInfo& ReadNewMatchInfoTag(std::span<const uint8_t> data) {
    commPacket.ReadNewMatchInfoTag(data);
    return commPacket.matchInfo;
  }

  HAL_MatchInfo& ReadGameSpecificTag(std::span<const uint8_t> data) {
    commPacket.ReadGameSpecificMessageTag(data);
    return commPacket.matchInfo;
  }

 protected:
  halsim::DSCommPacket commPacket;
};

TEST_F(DSCommPacketTest, EmptyJoystickTag) {
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    uint8_t arr[2];
    auto& data = ReadJoystickTag(arr, 0);
    ASSERT_EQ(data.axes.available, 0);
    ASSERT_EQ(data.povs.available, 0);
    ASSERT_EQ(data.buttons.available, 0llu);
  }
}

TEST_F(DSCommPacketTest, BlankJoystickTag) {
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    uint8_t arr[5];
    arr[0] = 4;
    arr[1] = 2;
    arr[2] = 0;
    arr[3] = 0;
    arr[4] = 0;
    auto& data = ReadJoystickTag(arr, 0);
    ASSERT_EQ(data.axes.available, 0);
    ASSERT_EQ(data.povs.available, 0);
    ASSERT_EQ(data.buttons.available, 0llu);
  }
}

TEST_F(DSCommPacketTest, MainJoystickTag) {
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    // Just random data
    std::array<uint8_t, 12> _buttons{{0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1}};

    std::array<uint8_t, 2> _button_bytes{{0, 0}};
    for (int btn = 0; btn < 8; btn++) {
      _button_bytes[1] |= _buttons[btn] << btn;
    }
    for (int btn = 8; btn < 12; btn++) {
      _button_bytes[0] |= _buttons[btn] << (btn - 8);
    }

    // 5 for base, 4 joystick, 12 buttons (2 bytes) 3 povs
    uint8_t arr[5 + 4 + 2 + 6] = {// Size, Tag
                                  16, 12,
                                  // Axes
                                  4, 0x9C, 0xCE, 0, 75,
                                  // Buttons (LSB 0)
                                  12, _button_bytes[0], _button_bytes[1],
                                  // POVs
                                  3, 0, 50, 0, 100, 0x0F, 0x00};

    auto& data = ReadJoystickTag(arr, 0);
    ASSERT_EQ(data.axes.available, 0xF);
    ASSERT_EQ(data.povs.available, 0x7);
    ASSERT_EQ(data.buttons.available, 0xFFFllu);

    for (int btn = 0; btn < 12; btn++) {
      ASSERT_EQ((data.buttons.buttons & (1llu << btn)) != 0, _buttons[btn] != 0)
          << "Button " << btn;
    }
  }
}

TEST_F(DSCommPacketTest, DescriptorTag) {
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    uint8_t arr[] = {// Size (2), tag
                     0, 0, 7,
                     // Joystick index, Is Xbox, Type
                     static_cast<uint8_t>(i), 1, 0,
                     // NameLen, Name (Not null terminated)
                     11, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd',
                     // Axes count, Axes types
                     4, 1, 2, 3, 4,
                     // Button count, pov count,
                     12, 3};
    arr[1] = sizeof(arr) - 2;
    auto& data = ReadDescriptorTag(arr);
    ASSERT_EQ(data.descriptor.isGamepad, 1);
    ASSERT_EQ(data.descriptor.gamepadType, 0);
    ASSERT_STREQ(data.descriptor.name, "Hello World");
  }
}

TEST_F(DSCommPacketTest, MatchInfoTag) {
  uint8_t arr[]{// Size (2), tag
                0, 0, 8,
                // Event Name Len, Event Name
                4, 'W', 'C', 'B', 'C',
                // Match type, Match num (2), replay num
                2, 0, 18, 1};
  arr[1] = sizeof(arr) - 2;
  auto& matchInfo = ReadNewMatchInfoTag(arr);
  ASSERT_STREQ(matchInfo.eventName, "WCBC");
  ASSERT_EQ(matchInfo.matchType, HAL_MatchType::HAL_kMatchType_qualification);
  ASSERT_EQ(matchInfo.matchNumber, 18);
  ASSERT_EQ(matchInfo.replayNumber, 1);
}

TEST_F(DSCommPacketTest, GameDataTag) {
  uint8_t arr[]{
      // Size (2), tag
      0,
      0,
      17,
      // Match data (length is taglength - 1)
      'W',
      'C',
      'B',
      'C',
  };
  arr[1] = sizeof(arr) - 2;
  auto& matchInfo = ReadGameSpecificTag(arr);
  ASSERT_EQ(matchInfo.gameSpecificMessageSize, 4);
  ASSERT_EQ(matchInfo.gameSpecificMessage[0], 'W');
  ASSERT_EQ(matchInfo.gameSpecificMessage[1], 'C');
  ASSERT_EQ(matchInfo.gameSpecificMessage[2], 'B');
  ASSERT_EQ(matchInfo.gameSpecificMessage[3], 'C');
}
