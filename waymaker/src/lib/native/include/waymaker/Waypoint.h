
#include <frc/geometry/Pose2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Translation2d.h>
namespace waymaker {
class Waypoint : public frc::Pose2d {
 public:
  Pose2d* posePtr;
  float* tangentPtr;
};
}  // namespace waymaker