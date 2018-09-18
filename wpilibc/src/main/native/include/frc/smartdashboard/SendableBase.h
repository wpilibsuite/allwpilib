/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <wpi/mutex.h>

#include "frc/smartdashboard/Sendable.h"

namespace frc {

class SendableBase : public Sendable {
 public:
  /**
   * Creates an instance of the sensor base.
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  explicit SendableBase(bool addLiveWindow = true);

  ~SendableBase() override;

  SendableBase(SendableBase&& rhs);
  SendableBase& operator=(SendableBase&& rhs);

  using Sendable::SetName;

  std::string GetName() const final;
  void SetName(const wpi::Twine& name) final;
  std::string GetSubsystem() const final;
  void SetSubsystem(const wpi::Twine& subsystem) final;

 protected:
  /**
   * Add a child component.
   *
   * @param child child component
   */
  void AddChild(std::shared_ptr<Sendable> child);

  /**
   * Add a child component.
   *
   * @param child child component
   */
  void AddChild(void* child);

  /**
   * Sets the name of the sensor with a channel number.
   *
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  void SetName(const wpi::Twine& moduleType, int channel);

  /**
   * Sets the name of the sensor with a module and channel number.
   *
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually
   * PWM)
   */
  void SetName(const wpi::Twine& moduleType, int moduleNumber, int channel);

 private:
  mutable wpi::mutex m_mutex;
  std::string m_name;
  std::string m_subsystem = "Ungrouped";
};

}  // namespace frc
