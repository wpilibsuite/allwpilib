#pragma once

#include <frc/trajectory/constraint/TrajectoryConstraint.h>
#include <semiwrap.h>

namespace frc {

struct PyTrajectoryConstraint : public TrajectoryConstraint {

  PyTrajectoryConstraint() {}

  units::meters_per_second_t
  MaxVelocity(const Pose2d &pose, units::curvature_t curvature,
              units::meters_per_second_t velocity) const override {
    return m_constraint->MaxVelocity(pose, curvature, velocity);
  }

  MinMax MinMaxAcceleration(const Pose2d &pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override {
    return m_constraint->MinMaxAcceleration(pose, curvature, speed);
  }

  std::shared_ptr<TrajectoryConstraint> m_constraint;
};

}; // namespace frc

namespace pybind11 {
namespace detail {

template <> struct type_caster<frc::PyTrajectoryConstraint> {
  using value_conv = make_caster<std::shared_ptr<frc::TrajectoryConstraint>>;

  PYBIND11_TYPE_CASTER(frc::PyTrajectoryConstraint, _("wpimath._controls._controls.constraint.TrajectoryConstraint"));

  bool load(handle src, bool convert) {
    value_conv conv;
    if (!conv.load(src, convert)) {
      return false;
    }

    value.m_constraint =
        cast_op<std::shared_ptr<frc::TrajectoryConstraint>>(std::move(conv));
    return true;
  }

  static handle cast(const frc::PyTrajectoryConstraint &src,
                     return_value_policy policy, handle parent) {
    return value_conv::cast(src.m_constraint, policy, parent);
  }
};

}; // namespace detail
}; // namespace pybind11
