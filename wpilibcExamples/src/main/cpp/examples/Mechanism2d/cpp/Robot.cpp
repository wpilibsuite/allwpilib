// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Encoder.h>
#include <frc/AnalogPotentiometer.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/smartdashboard/Mechanism2d.h>
#include <frc/smartdashboard/MechanismLigament2d.h>
#include <units/angle.h>
#include <frc/Joystick.h>
#include <frc/PWMSparkMax.h>
#include <frc/TimedRobot.h>

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs as
 * range from -1 to 1 making it easy to work together.
 */
class Robot : public frc::TimedRobot {
  static constexpr double kMetersPerPulse = 0.1;
 public:
  void RobotInit() override {
    m_elevatorEncoder.SetDistancePerPulse(kMetersPerPulse);
    m_root.Append<frc::MechanismLigament2d>(m_elevator);
    m_elevator.Append(m_wrist);

    frc::SmartDashboard::PutData(wpi::StringRef("Mech2d"), &m_mech);
  }
  void RobotPeriodic() override {
    m_elevator.SetLength(m_elevatorEncoder.GetDistance());
    m_wrist.SetAngle(units::degree_t(m_wristPotentiometer.Get()));
  }
  void TeleopPeriodic() override { 
    m_elevatorMotor.Set(m_joystick.GetRawAxis(0));
    m_wristMotor.Set(m_joystick.GetRawAxis(1));
  }

 private:
  frc::PWMSparkMax m_elevatorMotor {0};
  frc::PWMSparkMax m_wristMotor {1};
  frc::Encoder m_elevatorEncoder {0, 1};
  frc::AnalogPotentiometer m_wristPotentiometer {1, 180};
  frc::Joystick m_joystick{0};

  frc::Mechanism2d m_mech {200, 200};
  frc::MechanismRoot2d m_root = m_mech.GetRoot("climber", 80, 100);
  frc::MechanismLigament2d m_elevator {"elevator", frc::Color::kDarkOrange, 10, 0_deg, 10};
  frc::MechanismLigament2d m_wrist {"wrist", frc::Color::kForestGreen, 6, 90_deg, 4};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
