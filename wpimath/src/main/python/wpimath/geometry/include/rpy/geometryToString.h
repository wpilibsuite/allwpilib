#include <string>

#include <frc/geometry/Ellipse2d.h>
#include <frc/geometry/Pose2d.h>
#include <frc/geometry/Pose3d.h>
#include <frc/geometry/Rectangle2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Rotation3d.h>
#include <frc/geometry/Translation2d.h>
#include <frc/geometry/Translation3d.h>

namespace rpy {

inline std::string toString(const frc::Rotation2d &self) {
  return "Rotation2d(" + std::to_string(self.Radians()()) + ")";
}

inline std::string toString(const frc::Rotation3d &self) {
  return "Rotation3d("
         "x=" +
         std::to_string(self.X()()) +
         ", "
         "y=" +
         std::to_string(self.Y()()) +
         ", "
         "z=" +
         std::to_string(self.Z()()) + ")";
}

inline std::string toString(const frc::Translation2d &self) {
  return "Translation2d("
         "x=" +
         std::to_string(self.X()()) +
         ", "
         "y=" +
         std::to_string(self.Y()()) + ")";
}

inline std::string toString(const frc::Translation3d &self) {
  return "Translation3d("
         "x=" +
         std::to_string(self.X()()) +
         ", "
         "y=" +
         std::to_string(self.Y()()) +
         ", "
         "z=" +
         std::to_string(self.Z()()) + ")";
}

inline std::string toString(const frc::Quaternion &self) {
  return "Quaternion("
         "w=" +
         std::to_string(self.W()) +
         ","
         "x=" +
         std::to_string(self.X()) +
         ", "
         "y=" +
         std::to_string(self.Y()) +
         ", "
         "z=" +
         std::to_string(self.Z()) + ")";
}

inline std::string toString(const frc::Transform2d &self) {
  return "Transform2d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const frc::Transform3d &self) {
  return "Transform3d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const frc::Pose2d &self) {
  return "Pose2d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const frc::Pose3d &self) {
  return "Pose3d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const frc::Rectangle2d &self) {
  return "Rectangle2d(center=" + rpy::toString(self.Center()) +
    ", xWidth=" + std::to_string(self.XWidth()()) +
    ", yWidth=" + std::to_string(self.YWidth()()) + ")";
}

inline std::string toString(const frc::Ellipse2d &self) {
  return "Ellipse2d(center=" + rpy::toString(self.Center()) +
    ", xSemiAxis=" + std::to_string(self.XSemiAxis()()) +
    ", ySemiAxis=" + std::to_string(self.YSemiAxis()()) + ")";
}


} // namespace rpy
