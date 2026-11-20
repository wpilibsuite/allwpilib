"""
Protobuf support for wpimath.

For each wpimath type with a protobuf wire format there is a serializer class
(``Pose2dProto``, ``SwerveModulePositionProto``, ...) that converts between the type and its
generated message. They are stateless, so use the class directly::

    data = Pose2dProto.serialize(pose)
    pose = Pose2dProto.deserialize(data)

The serializers are also available from ``wpimath`` itself. The generated message classes
live in ``wpimath.protobuf._generated`` (from ``wpimath/src/main/proto``) and their modules
are re-exported here (``geometry2d_pb2`` and so on). The matrix types need ``numpy``.
"""

from ._generated import (
    controller_pb2,
    geometry2d_pb2,
    geometry3d_pb2,
    kinematics_pb2,
    shape_pb2,
    spline_pb2,
    system_pb2,
    trajectory_pb2,
    wpimath_pb2,
)
from .controller import (
    ArmFeedforwardProto,
    DifferentialDriveFeedforwardProto,
    DifferentialDriveWheelVoltagesProto,
    ElevatorFeedforwardProto,
    SimpleMotorFeedforwardProto,
)
from .geometry import (
    Pose2dProto,
    Pose3dProto,
    QuaternionProto,
    Rotation2dProto,
    Rotation3dProto,
    Transform2dProto,
    Transform3dProto,
    Translation2dProto,
    Translation3dProto,
    Twist2dProto,
    Twist3dProto,
)
from .kinematics import (
    ChassisAccelerationsProto,
    ChassisVelocitiesProto,
    DifferentialDriveKinematicsProto,
    DifferentialDriveWheelAccelerationsProto,
    DifferentialDriveWheelPositionsProto,
    DifferentialDriveWheelVelocitiesProto,
    MecanumDriveKinematicsProto,
    MecanumDriveWheelAccelerationsProto,
    MecanumDriveWheelPositionsProto,
    MecanumDriveWheelVelocitiesProto,
    SwerveDriveKinematicsProto,
    SwerveModuleAccelerationProto,
    SwerveModulePositionProto,
    SwerveModuleVelocityProto,
)
from .linalg import MatrixProto, VectorProto
from .shape import Ellipse2dProto, Rectangle2dProto
from .spline import CubicHermiteSplineProto, QuinticHermiteSplineProto
from .system import DCMotorProto, LinearSystemProto
from .trajectory import (
    DifferentialSampleProto,
    DifferentialTrajectoryProto,
    HolonomicSampleProto,
    HolonomicTrajectoryProto,
)

__all__ = [
    # generated message modules
    "controller_pb2",
    "geometry2d_pb2",
    "geometry3d_pb2",
    "kinematics_pb2",
    "shape_pb2",
    "spline_pb2",
    "system_pb2",
    "trajectory_pb2",
    "wpimath_pb2",
    # controller
    "ArmFeedforwardProto",
    "DifferentialDriveFeedforwardProto",
    "DifferentialDriveWheelVoltagesProto",
    "ElevatorFeedforwardProto",
    "SimpleMotorFeedforwardProto",
    # geometry
    "Pose2dProto",
    "Pose3dProto",
    "QuaternionProto",
    "Rotation2dProto",
    "Rotation3dProto",
    "Transform2dProto",
    "Transform3dProto",
    "Translation2dProto",
    "Translation3dProto",
    "Twist2dProto",
    "Twist3dProto",
    # kinematics
    "ChassisAccelerationsProto",
    "ChassisVelocitiesProto",
    "DifferentialDriveKinematicsProto",
    "DifferentialDriveWheelAccelerationsProto",
    "DifferentialDriveWheelPositionsProto",
    "DifferentialDriveWheelVelocitiesProto",
    "MecanumDriveKinematicsProto",
    "MecanumDriveWheelAccelerationsProto",
    "MecanumDriveWheelPositionsProto",
    "MecanumDriveWheelVelocitiesProto",
    "SwerveDriveKinematicsProto",
    "SwerveModuleAccelerationProto",
    "SwerveModulePositionProto",
    "SwerveModuleVelocityProto",
    # linalg
    "MatrixProto",
    "VectorProto",
    # shape
    "Ellipse2dProto",
    "Rectangle2dProto",
    # spline
    "CubicHermiteSplineProto",
    "QuinticHermiteSplineProto",
    # system
    "DCMotorProto",
    "LinearSystemProto",
    # trajectory
    "DifferentialSampleProto",
    "DifferentialTrajectoryProto",
    "HolonomicSampleProto",
    "HolonomicTrajectoryProto",
]
