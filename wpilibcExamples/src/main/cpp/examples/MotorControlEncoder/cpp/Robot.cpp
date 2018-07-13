/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include <Encoder.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <SmartDashboard/SmartDashboard.h>
#include <Spark.h>

constexpr double kPi = 3.14159265358979;

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs as
 * range from -1 to 1 making it easy to work together.
 *
 * In addition, the encoder value of an encoder connected to ports 0 and 1 is
 * consistently sent to the Dashboard.
 */
class Robot : public frc::IterativeRobot {
 public:
  void TeleopPeriodic() override { m_motor.Set(m_stick.GetY()); }

  /*
   * The RobotPeriodic function is called every control packet no matter the
   * robot mode.
   */
  void RobotPeriodic() override {
    frc::SmartDashboard::PutNumber("Encoder", m_encoder.GetDistance());
  }

  void RobotInit() override {
    // Use SetDistancePerPulse to set the multiplier for GetDistance
    // This is set up assuming a 6 inch wheel with a 360 CPR encoder.
    m_encoder.SetDistancePerPulse((kPi * 6) / 360.0);
  }

 private:
  frc::Joystick m_stick{0};
  frc::Spark m_motor{0};
  frc::Encoder m_encoder{0, 1};
};

int main() { return frc::StartRobot<Robot>(); }
