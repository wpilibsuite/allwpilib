#include <functional>
#include <units/units.h>

#include "CommandBase.h"
#include "CommandHelper.h"
#include "frc/trajectory/Trajectory.h"
#include "frc/geometry/Pose2d.h"
#include "frc/controller/RamseteController.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/controller/PIDController.h"
#include "frc/Timer.h"

#pragma once

using namespace units;

namespace frc2{
class RamseteCommand : public CommandHelper<CommandBase, RamseteCommand> {
using voltsecondspermeter = compound_unit<voltage::volt, second, inverse<meter>>;
using voltsecondssquaredpermeter = compound_unit<voltage::volt, squared<second>, inverse<meter>>;

 public:
  RamseteCommand(frc::Trajectory trajectory,
                 std::function<frc::Pose2d()> pose,
                 frc::RamseteController follower,
                 
                voltage::volt_t ks,
                 
                unit_t<voltsecondspermeter> kv,
                 
                unit_t<voltsecondssquaredpermeter> ka,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<
                meters_per_second_t()> leftSpeed,
                 std::function<
                meters_per_second_t()> rightSpeed,
                 frc2::PIDController leftController,
                 frc2::PIDController rightController,
                 std::function<void(
                  voltage::volt_t, 
                voltage::volt_t)> output);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::RamseteController m_follower;
  const 
voltage::volt_t m_ks;
  const 
unit_t<voltsecondspermeter> m_kv;
  const 
unit_t<voltsecondssquaredpermeter> m_ka;
  frc::DifferentialDriveKinematics m_kinematics;
  std::function<
meters_per_second_t()> m_leftSpeed;
  std::function<
meters_per_second_t()> m_rightSpeed;
  frc2::PIDController m_leftController;
  frc2::PIDController m_rightController;
  std::function<void(
  voltage::volt_t, 
voltage::volt_t)> m_output;

  frc::Timer m_timer;
  
second_t m_prevTime;
  frc::DifferentialDriveWheelSpeeds m_prevSpeeds;
};
}
