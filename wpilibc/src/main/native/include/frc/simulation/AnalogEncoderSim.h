/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/angle.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/AnalogEncoder.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/simulation/SimDeviceSim.h"

namespace frc {

namespace sim {

/**
 * Class to control a simulated analog encoder.
 */
class AnalogEncoderSim {
 public:
  /**
   * Constructs from an AnalogEncoder object.
   *
   * @param analogInput AnalogEncoder to simulate
   */
  explicit AnalogEncoderSim(const frc::AnalogEncoder& analogEncoder) {
    wpi::SmallString<128> fullname;
    wpi::raw_svector_ostream os(fullname);
    os << "AnalogEncoder" << '[' << analogEncoder.GetChannel() << ']';
    frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
    m_positionSim = deviceSim.GetDouble("Position");
  }

  /**
   * Set the position using an {@link Rotation2d}.
   */
  void SetPosition(frc::Rotation2d angle) { SetTurns(angle.Degrees()); }

  /**
   * Set the position of the encoder.
   *
   * @param turns The position.
   */
  void SetTurns(units::turn_t turns) { m_positionSim.Set(turns.to<double>()); }

  /**
   * Get the simulated position.
   */
  units::turn_t GetTurns() { return units::turn_t{m_positionSim.Get()}; }

  /**
   * Get the position as a {@link Rotation2d}.
   */
  frc::Rotation2d GetPosition() {
    units::radian_t rads = GetTurns();
    return frc::Rotation2d{rads};
  }

 private:
  hal::SimDouble m_positionSim;
};
}  // namespace sim
}  // namespace frc
