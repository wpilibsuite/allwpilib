/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include <AnalogGyro.h>
#include <Drive/MecanumDrive.h>
#include <IterativeRobot.h>
#include <Joystick.h>
#include <Spark.h>

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintian rotation vectorsin relation to the starting orientation of the robot
 * (field-oriented controls).
 */
class Robot : public frc::IterativeRobot {
 public:
  void RobotInit() override {
    // Invert the left side motors. You may need to change or remove this to
    // match your robot.
    m_frontLeft.SetInverted(true);
    m_rearLeft.SetInverted(true);

    m_gyro.SetSensitivity(kVoltsPerDegreePerSecond);
  }

  /**
   * Mecanum drive is used with the gyro angle as an input.
   */
  void TeleopPeriodic() override {
    m_robotDrive.DriveCartesian(m_joystick.GetX(), m_joystick.GetY(),
                                m_joystick.GetZ(), m_gyro.GetAngle());
  }

 private:
  // Gyro calibration constant, may need to be adjusted. Gyro value of 360 is
  // set to correspond to one full revolution.
  static constexpr double kVoltsPerDegreePerSecond = 0.0128;

  static constexpr int kFrontLeftMotorPort = 0;
  static constexpr int kRearLeftMotorPort = 1;
  static constexpr int kFrontRightMotorPort = 2;
  static constexpr int kRearRightMotorPort = 3;
  static constexpr int kGyroPort = 0;
  static constexpr int kJoystickPort = 0;

  frc::Spark m_frontLeft{kFrontLeftMotorPort};
  frc::Spark m_rearLeft{kRearLeftMotorPort};
  frc::Spark m_frontRight{kFrontRightMotorPort};
  frc::Spark m_rearRight{kRearRightMotorPort};
  frc::MecanumDrive m_robotDrive{m_frontLeft, m_rearLeft, m_frontRight,
                                 m_rearRight};

  frc::AnalogGyro m_gyro{kGyroPort};
  frc::Joystick m_joystick{kJoystickPort};
};

int main() { return frc::StartRobot<Robot>(); }
