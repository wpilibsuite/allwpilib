// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <wpi/mutex.h>

namespace frc {
class ExpansionHubServo;
class ExpansionHubMotor;

/** This class controls a REV ExpansionHub plugged in over USB to Systemcore. */
class ExpansionHub {
 public:
  /**
   * Constructs a new ExpansionHub for a given USB ID
   *
   * Multiple instances can be constructed, but will point to the same backing
   * object with a ref count.
   *
   * @param usbId The USB Port ID the hub is plugged into.
   */
  explicit ExpansionHub(int usbId);
  ~ExpansionHub() noexcept = default;

  friend class ExpansionHubServo;
  friend class ExpansionHubMotor;

  /**
   * Constructs a servo at the requested channel on this hub.
   *
   * Only a single instance of each servo per hub can be constructed at a time.
   *
   * @param channel The servo channel
   * @return Servo object
   */
  ExpansionHubServo MakeServo(int channel);

  /**
   * Constructs a motor at the requested channel on this hub.
   *
   * Only a single instance of each motor per hub can be constructed at a time.
   *
   * @param channel The motor channel
   * @return Motor object
   */
  ExpansionHubMotor MakeMotor(int channel);

  /**
   * Gets if the hub is currently connected over USB.
   *
   * @return True if hub connection, otherwise false
   */
  bool IsHubConnected() const;

  /**
   * Gets the USB ID of this hub.
   *
   * @return The USB ID
   */
  int GetUsbId() const { return m_usbId; }

  static constexpr int NumUsbPorts = 4;
  static constexpr int NumServoPorts = 6;
  static constexpr int NumMotorPorts = 4;

 private:
  bool CheckAndReserveServo(int channel);
  void UnreserveServo(int channel);

  bool CheckAndReserveMotor(int channel);
  void UnreserveMotor(int channel);

  void ReportUsage(std::string_view device, std::string_view data);

  class DataStore;
  friend class DataStore;

  std::shared_ptr<DataStore> m_dataStore;
  int m_usbId;

  static wpi::mutex m_handleLock;
  static std::weak_ptr<DataStore> m_storeMap[4];

  static std::shared_ptr<DataStore> GetForUsbId(int usbId);
};

}  // namespace frc
