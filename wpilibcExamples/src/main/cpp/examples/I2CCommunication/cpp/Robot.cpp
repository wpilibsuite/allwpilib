/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/DriverStation.h>
#include <frc/I2C.h>
#include <frc/TimedRobot.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's I2C port.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    // Creates a string to hold current robot state information, including
    // alliance, enabled state, operation mode, and match time. The message
    // is sent in format "AEM###" where A is the alliance color, (R)ed or
    // (B)lue, E is the enabled state, (E)nabled or (D)isabled, M is the
    // operation mode, (A)utonomous or (T)eleop, and ### is the zero-padded
    // time remaining in the match.
    //
    // For example, "RET043" would indicate that the robot is on the red
    // alliance, enabled in teleop mode, with 43 seconds left in the match.
    wpi::SmallString<128> data;
    wpi::raw_svector_ostream os(data);
    os << (m_ds.GetAlliance() == frc::DriverStation::Alliance::kRed ? "R" : "B")
       << (m_ds.IsEnabled() ? "E" : "D") << (m_ds.IsAutonomous() ? "A" : "T")
       << wpi::format("%03d", m_ds.GetMatchTime());

    arduino.WriteBulk(reinterpret_cast<uint8_t*>(data.data()), data.size());
  }

 private:
  int deviceAddress = 4;
  frc::I2C arduino{frc::I2C::Port::kOnboard, deviceAddress};
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
