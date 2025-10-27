// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/hardware/discrete/AnalogInput.hpp>
#include <wpi/units/length.hpp>

namespace wpi::xrp {

/**
 * @ingroup xrp_api
 * @{
 */

/** This class represents the ultrasonic rangefinder on an XRP robot. */
class XRPRangefinder {
 public:
  /**
   * Get the measured distance in meters. Distance further than 4m will be
   * reported as 4m.
   *
   * @return distance in meters
   */
  wpi::units::meter_t GetDistance() const;

 private:
  wpi::AnalogInput m_rangefinder{2};
};

/** @} */

}  // namespace wpi::xrp
