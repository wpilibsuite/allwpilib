#include <string>

#include "wpi/math/geometry/Ellipse2d.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rectangle2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"

namespace rpy {

inline std::string toString(const wpi::math::Rotation2d &self) {
  return "Rotation2d(" + std::to_string(self.Radians()()) + ")";
}

inline std::string toString(const wpi::math::Rotation3d &self) {
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

inline std::string toString(const wpi::math::Translation2d &self) {
  return "Translation2d("
         "x=" +
         std::to_string(self.X()()) +
         ", "
         "y=" +
         std::to_string(self.Y()()) + ")";
}

inline std::string toString(const wpi::math::Translation3d &self) {
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

inline std::string toString(const wpi::math::Quaternion &self) {
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

inline std::string toString(const wpi::math::Transform2d &self) {
  return "Transform2d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const wpi::math::Transform3d &self) {
  return "Transform3d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const wpi::math::Pose2d &self) {
  return "Pose2d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const wpi::math::Pose3d &self) {
  return "Pose3d(" + rpy::toString(self.Translation()) + ", " +
         rpy::toString(self.Rotation()) + ")";
}

inline std::string toString(const wpi::math::Rectangle2d &self) {
  return "Rectangle2d(center=" + rpy::toString(self.Center()) +
    ", xWidth=" + std::to_string(self.XWidth()()) +
    ", yWidth=" + std::to_string(self.YWidth()()) + ")";
}

inline std::string toString(const wpi::math::Ellipse2d &self) {
  return "Ellipse2d(center=" + rpy::toString(self.Center()) +
    ", xSemiAxis=" + std::to_string(self.XSemiAxis()()) +
    ", ySemiAxis=" + std::to_string(self.YSemiAxis()()) + ")";
}


} // namespace rpy
