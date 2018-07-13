/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include <IterativeRobot.h>
#include <Joystick.h>
#include <Spark.h>

/**
 * This sample program shows how to control a motor using a joystick. In the
 * operator control part of the program, the joystick is read and the value is
 * written to the motor.
 *
 * Joystick analog values range from -1 to 1 and speed controller inputs as
 * range from -1 to 1 making it easy to work together.
 */
class Robot : public frc::IterativeRobot {
 public:
  void TeleopPeriodic() override { m_motor.Set(m_stick.GetY()); }

 private:
  frc::Joystick m_stick{0};
  frc::Spark m_motor{0};
};

int main() { return frc::StartRobot<Robot>(); }
