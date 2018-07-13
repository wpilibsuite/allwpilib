/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include <CameraServer.h>
#include <IterativeRobot.h>
#include <wpi/raw_ostream.h>

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam
 * and send it to the FRC dashboard without doing any vision processing. This is
 * the easiest way to get camera images to the dashboard. Just add this to the
 * RobotInit() method in your program.
 */
class Robot : public frc::IterativeRobot {
 public:
  void RobotInit() override {
#if defined(__linux__)
    frc::CameraServer::GetInstance()->StartAutomaticCapture();
#else
    wpi::errs() << "Vision only available on Linux.\n";
    wpi::errs().flush();
#endif
  }
};

int main() { return frc::StartRobot<Robot>(); }
