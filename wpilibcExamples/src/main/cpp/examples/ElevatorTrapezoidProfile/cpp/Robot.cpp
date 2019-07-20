/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/trajectory/TrapezoidProfile.h>

class Robot : public frc::TimedRobot {
 public:
  static constexpr units::second_t kDt = 20_ms;

  Robot() { m_encoder.SetDistancePerPulse(1.0 / 360.0 * 2.0 * 3.1415 * 1.5); }

  void TeleopPeriodic() override {
    if (m_joystick.GetRawButtonPressed(2)) {
      m_goal = {5_m, 0_mps};
    } else if (m_joystick.GetRawButtonPressed(3)) {
      m_goal = {0_m, 0_mps};
    }

    // Create a motion profile with the given maximum velocity and maximum
    // acceleration constraints for the next setpoint, the desired goal, and the
    // current setpoint.
    frc::TrapezoidProfile profile{m_constraints, m_goal, m_setpoint};

    // Retrieve the profiled setpoint for the next timestep. This setpoint moves
    // toward the goal while obeying the constraints.
    m_setpoint = profile.Calculate(kDt);

    // Run controller with profiled setpoint and update motor output
    double output = m_controller.Calculate(m_encoder.GetDistance(),
                                           m_setpoint.position.to<double>());
    m_motor.Set(output);
  }

 private:
  frc::Joystick m_joystick{1};
  frc::Encoder m_encoder{1, 2};
  frc::PWMVictorSPX m_motor{1};
  frc2::PIDController m_controller{1.3, 0.0, 0.7, kDt};

  frc::TrapezoidProfile::Constraints m_constraints{1.75_mps, 0.75_mps_sq};
  frc::TrapezoidProfile::State m_goal;
  frc::TrapezoidProfile::State m_setpoint;
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
