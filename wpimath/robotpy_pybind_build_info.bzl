# THIS FILE IS AUTO GENERATED

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "publish_casters", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def wpimath_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_HEADER_GEN = [
        struct(
            class_name = "ComputerVisionUtil",
            yml_file = "semiwrap/ComputerVisionUtil.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/util/ComputerVisionUtil.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "MathUtil",
            yml_file = "semiwrap/MathUtil.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/util/MathUtil.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "ArmFeedforward",
            yml_file = "semiwrap/ArmFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ArmFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ArmFeedforward", "wpi__math__ArmFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "BangBangController",
            yml_file = "semiwrap/BangBangController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/BangBangController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::BangBangController", "wpi__math__BangBangController.hpp"),
            ],
        ),
        struct(
            class_name = "ControlAffinePlantInversionFeedforward",
            yml_file = "semiwrap/ControlAffinePlantInversionFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ControlAffinePlantInversionFeedforward.hpp",
            tmpl_class_names = [
                ("ControlAffinePlantInversionFeedforward_tmpl1", "ControlAffinePlantInversionFeedforward_1_1"),
                ("ControlAffinePlantInversionFeedforward_tmpl2", "ControlAffinePlantInversionFeedforward_2_1"),
                ("ControlAffinePlantInversionFeedforward_tmpl3", "ControlAffinePlantInversionFeedforward_2_2"),
            ],
            trampolines = [
                ("wpi::math::ControlAffinePlantInversionFeedforward", "wpi__math__ControlAffinePlantInversionFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveAccelerationLimiter",
            yml_file = "semiwrap/DifferentialDriveAccelerationLimiter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveAccelerationLimiter.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveAccelerationLimiter", "wpi__math__DifferentialDriveAccelerationLimiter.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveFeedforward",
            yml_file = "semiwrap/DifferentialDriveFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveFeedforward", "wpi__math__DifferentialDriveFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelVoltages",
            yml_file = "semiwrap/DifferentialDriveWheelVoltages.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveWheelVoltages.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelVoltages", "wpi__math__DifferentialDriveWheelVoltages.hpp"),
            ],
        ),
        struct(
            class_name = "ElevatorFeedforward",
            yml_file = "semiwrap/ElevatorFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ElevatorFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ElevatorFeedforward", "wpi__math__ElevatorFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "ImplicitModelFollower",
            yml_file = "semiwrap/ImplicitModelFollower.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ImplicitModelFollower.hpp",
            tmpl_class_names = [
                ("ImplicitModelFollower_tmpl1", "ImplicitModelFollower_1_1"),
                ("ImplicitModelFollower_tmpl2", "ImplicitModelFollower_2_1"),
                ("ImplicitModelFollower_tmpl3", "ImplicitModelFollower_2_2"),
            ],
            trampolines = [
                ("wpi::math::ImplicitModelFollower", "wpi__math__ImplicitModelFollower.hpp"),
            ],
        ),
        struct(
            class_name = "LTVDifferentialDriveController",
            yml_file = "semiwrap/LTVDifferentialDriveController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LTVDifferentialDriveController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LTVDifferentialDriveController", "wpi__math__LTVDifferentialDriveController.hpp"),
            ],
        ),
        struct(
            class_name = "LTVUnicycleController",
            yml_file = "semiwrap/LTVUnicycleController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LTVUnicycleController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LTVUnicycleController", "wpi__math__LTVUnicycleController.hpp"),
            ],
        ),
        struct(
            class_name = "LinearPlantInversionFeedforward",
            yml_file = "semiwrap/LinearPlantInversionFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LinearPlantInversionFeedforward.hpp",
            tmpl_class_names = [
                ("LinearPlantInversionFeedforward_tmpl1", "LinearPlantInversionFeedforward_1_1"),
                ("LinearPlantInversionFeedforward_tmpl2", "LinearPlantInversionFeedforward_2_1"),
                ("LinearPlantInversionFeedforward_tmpl3", "LinearPlantInversionFeedforward_2_2"),
                ("LinearPlantInversionFeedforward_tmpl4", "LinearPlantInversionFeedforward_3_2"),
            ],
            trampolines = [
                ("wpi::math::LinearPlantInversionFeedforward", "wpi__math__LinearPlantInversionFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "LinearQuadraticRegulator",
            yml_file = "semiwrap/LinearQuadraticRegulator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LinearQuadraticRegulator.hpp",
            tmpl_class_names = [
                ("LinearQuadraticRegulator_tmpl1", "LinearQuadraticRegulator_1_1"),
                ("LinearQuadraticRegulator_tmpl2", "LinearQuadraticRegulator_2_1"),
                ("LinearQuadraticRegulator_tmpl3", "LinearQuadraticRegulator_2_2"),
                ("LinearQuadraticRegulator_tmpl4", "LinearQuadraticRegulator_3_2"),
            ],
            trampolines = [
                ("wpi::math::LinearQuadraticRegulator", "wpi__math__LinearQuadraticRegulator.hpp"),
            ],
        ),
        struct(
            class_name = "PIDController",
            yml_file = "semiwrap/PIDController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/PIDController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::PIDController", "wpi__math__PIDController.hpp"),
            ],
        ),
        struct(
            class_name = "ProfiledPIDController",
            yml_file = "semiwrap/ProfiledPIDController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ProfiledPIDController.hpp",
            tmpl_class_names = [
                ("ProfiledPIDController_tmpl1", "ProfiledPIDController"),
                ("ProfiledPIDController_tmpl2", "ProfiledPIDControllerRadians"),
            ],
            trampolines = [
                ("wpi::math::ProfiledPIDController", "wpi__math__ProfiledPIDController.hpp"),
            ],
        ),
        struct(
            class_name = "SimpleMotorFeedforward",
            yml_file = "semiwrap/SimpleMotorFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/SimpleMotorFeedforward.hpp",
            tmpl_class_names = [
                ("SimpleMotorFeedforward_tmpl1", "SimpleMotorFeedforwardMeters"),
                ("SimpleMotorFeedforward_tmpl2", "SimpleMotorFeedforwardRadians"),
            ],
            trampolines = [
                ("wpi::math::SimpleMotorFeedforward", "wpi__math__SimpleMotorFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDrivePoseEstimator",
            yml_file = "semiwrap/DifferentialDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/DifferentialDrivePoseEstimator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDrivePoseEstimator", "wpi__math__DifferentialDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDrivePoseEstimator3d",
            yml_file = "semiwrap/DifferentialDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/DifferentialDrivePoseEstimator3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDrivePoseEstimator3d", "wpi__math__DifferentialDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "ExtendedKalmanFilter",
            yml_file = "semiwrap/ExtendedKalmanFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/ExtendedKalmanFilter.hpp",
            tmpl_class_names = [
                ("ExtendedKalmanFilter_tmpl1", "ExtendedKalmanFilter_1_1_1"),
                ("ExtendedKalmanFilter_tmpl2", "ExtendedKalmanFilter_2_1_1"),
                ("ExtendedKalmanFilter_tmpl3", "ExtendedKalmanFilter_2_1_2"),
                ("ExtendedKalmanFilter_tmpl4", "ExtendedKalmanFilter_2_2_2"),
            ],
            trampolines = [
                ("wpi::math::ExtendedKalmanFilter", "wpi__math__ExtendedKalmanFilter.hpp"),
            ],
        ),
        struct(
            class_name = "KalmanFilter",
            yml_file = "semiwrap/KalmanFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/KalmanFilter.hpp",
            tmpl_class_names = [
                ("KalmanFilter_tmpl1", "KalmanFilter_1_1_1"),
                ("KalmanFilter_tmpl2", "KalmanFilter_2_1_1"),
                ("KalmanFilter_tmpl3", "KalmanFilter_2_1_2"),
                ("KalmanFilter_tmpl4", "KalmanFilter_2_2_2"),
                ("KalmanFilter_tmpl5", "KalmanFilter_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::KalmanFilter", "wpi__math__KalmanFilter.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDrivePoseEstimator",
            yml_file = "semiwrap/MecanumDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/MecanumDrivePoseEstimator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDrivePoseEstimator", "wpi__math__MecanumDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDrivePoseEstimator3d",
            yml_file = "semiwrap/MecanumDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/MecanumDrivePoseEstimator3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDrivePoseEstimator3d", "wpi__math__MecanumDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "PoseEstimator",
            yml_file = "semiwrap/PoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/PoseEstimator.hpp",
            tmpl_class_names = [
                ("PoseEstimator_tmpl1", "DifferentialDrivePoseEstimatorBase"),
                ("PoseEstimator_tmpl2", "MecanumDrivePoseEstimatorBase"),
                ("PoseEstimator_tmpl3", "SwerveDrive2PoseEstimatorBase"),
                ("PoseEstimator_tmpl4", "SwerveDrive3PoseEstimatorBase"),
                ("PoseEstimator_tmpl5", "SwerveDrive4PoseEstimatorBase"),
                ("PoseEstimator_tmpl6", "SwerveDrive6PoseEstimatorBase"),
            ],
            trampolines = [
                ("wpi::math::PoseEstimator", "wpi__math__PoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "PoseEstimator3d",
            yml_file = "semiwrap/PoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/PoseEstimator3d.hpp",
            tmpl_class_names = [
                ("PoseEstimator3d_tmpl1", "DifferentialDrivePoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl2", "MecanumDrivePoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl3", "SwerveDrive2PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl4", "SwerveDrive3PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl5", "SwerveDrive4PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl6", "SwerveDrive6PoseEstimator3dBase"),
            ],
            trampolines = [
                ("wpi::math::PoseEstimator3d", "wpi__math__PoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDrivePoseEstimator",
            yml_file = "semiwrap/SwerveDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/SwerveDrivePoseEstimator.hpp",
            tmpl_class_names = [
                ("SwerveDrivePoseEstimator_tmpl1", "SwerveDrive2PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl2", "SwerveDrive3PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl3", "SwerveDrive4PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl4", "SwerveDrive6PoseEstimator"),
            ],
            trampolines = [
                ("wpi::math::SwerveDrivePoseEstimator", "wpi__math__SwerveDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDrivePoseEstimator3d",
            yml_file = "semiwrap/SwerveDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/SwerveDrivePoseEstimator3d.hpp",
            tmpl_class_names = [
                ("SwerveDrivePoseEstimator3d_tmpl1", "SwerveDrive2PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl2", "SwerveDrive3PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl3", "SwerveDrive4PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl4", "SwerveDrive6PoseEstimator3d"),
            ],
            trampolines = [
                ("wpi::math::SwerveDrivePoseEstimator3d", "wpi__math__SwerveDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "Debouncer",
            yml_file = "semiwrap/Debouncer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/Debouncer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Debouncer", "wpi__math__Debouncer.hpp"),
            ],
        ),
        struct(
            class_name = "LinearFilter",
            yml_file = "semiwrap/LinearFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/LinearFilter.hpp",
            tmpl_class_names = [
                ("LinearFilter_tmpl1", "LinearFilter"),
            ],
            trampolines = [
                ("wpi::math::LinearFilter", "wpi__math__LinearFilter.hpp"),
            ],
        ),
        struct(
            class_name = "MedianFilter",
            yml_file = "semiwrap/MedianFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/MedianFilter.hpp",
            tmpl_class_names = [
                ("MedianFilter_tmpl1", "MedianFilter"),
            ],
            trampolines = [
                ("wpi::math::MedianFilter", "wpi__math__MedianFilter.hpp"),
            ],
        ),
        struct(
            class_name = "SlewRateLimiter",
            yml_file = "semiwrap/SlewRateLimiter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/SlewRateLimiter.hpp",
            tmpl_class_names = [
                ("SlewRateLimiter_tmpl1", "SlewRateLimiter"),
            ],
            trampolines = [
                ("wpi::math::SlewRateLimiter", "wpi__math__SlewRateLimiter.hpp"),
            ],
        ),
        struct(
            class_name = "CoordinateAxis",
            yml_file = "semiwrap/CoordinateAxis.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/CoordinateAxis.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CoordinateAxis", "wpi__math__CoordinateAxis.hpp"),
            ],
        ),
        struct(
            class_name = "CoordinateSystem",
            yml_file = "semiwrap/CoordinateSystem.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/CoordinateSystem.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CoordinateSystem", "wpi__math__CoordinateSystem.hpp"),
            ],
        ),
        struct(
            class_name = "Ellipse2d",
            yml_file = "semiwrap/Ellipse2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Ellipse2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Ellipse2d", "wpi__math__Ellipse2d.hpp"),
            ],
        ),
        struct(
            class_name = "Pose2d",
            yml_file = "semiwrap/Pose2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Pose2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Pose2d", "wpi__math__Pose2d.hpp"),
            ],
        ),
        struct(
            class_name = "Pose3d",
            yml_file = "semiwrap/Pose3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Pose3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Pose3d", "wpi__math__Pose3d.hpp"),
            ],
        ),
        struct(
            class_name = "Quaternion",
            yml_file = "semiwrap/Quaternion.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Quaternion.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Quaternion", "wpi__math__Quaternion.hpp"),
            ],
        ),
        struct(
            class_name = "Rectangle2d",
            yml_file = "semiwrap/Rectangle2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rectangle2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rectangle2d", "wpi__math__Rectangle2d.hpp"),
            ],
        ),
        struct(
            class_name = "Rotation2d",
            yml_file = "semiwrap/Rotation2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rotation2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rotation2d", "wpi__math__Rotation2d.hpp"),
            ],
        ),
        struct(
            class_name = "Rotation3d",
            yml_file = "semiwrap/Rotation3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rotation3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rotation3d", "wpi__math__Rotation3d.hpp"),
            ],
        ),
        struct(
            class_name = "Transform2d",
            yml_file = "semiwrap/Transform2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Transform2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Transform2d", "wpi__math__Transform2d.hpp"),
            ],
        ),
        struct(
            class_name = "Transform3d",
            yml_file = "semiwrap/Transform3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Transform3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Transform3d", "wpi__math__Transform3d.hpp"),
            ],
        ),
        struct(
            class_name = "Translation2d",
            yml_file = "semiwrap/Translation2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Translation2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Translation2d", "wpi__math__Translation2d.hpp"),
            ],
        ),
        struct(
            class_name = "Translation3d",
            yml_file = "semiwrap/Translation3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Translation3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Translation3d", "wpi__math__Translation3d.hpp"),
            ],
        ),
        struct(
            class_name = "Twist2d",
            yml_file = "semiwrap/Twist2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Twist2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Twist2d", "wpi__math__Twist2d.hpp"),
            ],
        ),
        struct(
            class_name = "Twist3d",
            yml_file = "semiwrap/Twist3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Twist3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Twist3d", "wpi__math__Twist3d.hpp"),
            ],
        ),
        struct(
            class_name = "TimeInterpolatableBuffer",
            yml_file = "semiwrap/TimeInterpolatableBuffer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/interpolation/TimeInterpolatableBuffer.hpp",
            tmpl_class_names = [
                ("TimeInterpolatableBuffer_tmpl1", "TimeInterpolatablePose2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl2", "TimeInterpolatablePose3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl3", "TimeInterpolatableRotation2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl4", "TimeInterpolatableRotation3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl5", "TimeInterpolatableTranslation2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl6", "TimeInterpolatableTranslation3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl7", "TimeInterpolatableFloatBuffer"),
            ],
            trampolines = [
                ("wpi::math::TimeInterpolatableBuffer", "wpi__math__TimeInterpolatableBuffer.hpp"),
            ],
        ),
        struct(
            class_name = "ChassisSpeeds",
            yml_file = "semiwrap/ChassisSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/ChassisSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ChassisSpeeds", "wpi__math__ChassisSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "ChassisAccelerations",
            yml_file = "semiwrap/ChassisAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/ChassisAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ChassisAccelerations", "wpi__math__ChassisAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveKinematics",
            yml_file = "semiwrap/DifferentialDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveKinematics.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveKinematics", "wpi__math__DifferentialDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveOdometry3d",
            yml_file = "semiwrap/DifferentialDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveOdometry3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveOdometry3d", "wpi__math__DifferentialDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveOdometry",
            yml_file = "semiwrap/DifferentialDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveOdometry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveOdometry", "wpi__math__DifferentialDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelPositions",
            yml_file = "semiwrap/DifferentialDriveWheelPositions.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelPositions.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelPositions", "wpi__math__DifferentialDriveWheelPositions.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelSpeeds",
            yml_file = "semiwrap/DifferentialDriveWheelSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelSpeeds", "wpi__math__DifferentialDriveWheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelAccelerations",
            yml_file = "semiwrap/DifferentialDriveWheelAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelAccelerations", "wpi__math__DifferentialDriveWheelAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "Kinematics",
            yml_file = "semiwrap/Kinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Kinematics.hpp",
            tmpl_class_names = [
                ("Kinematics_tmpl1", "DifferentialDriveKinematicsBase"),
                ("Kinematics_tmpl2", "MecanumDriveKinematicsBase"),
                ("Kinematics_tmpl3", "SwerveDrive2KinematicsBase"),
                ("Kinematics_tmpl4", "SwerveDrive3KinematicsBase"),
                ("Kinematics_tmpl5", "SwerveDrive4KinematicsBase"),
                ("Kinematics_tmpl6", "SwerveDrive6KinematicsBase"),
            ],
            trampolines = [
                ("wpi::math::Kinematics", "wpi__math__Kinematics.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveKinematics",
            yml_file = "semiwrap/MecanumDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveKinematics.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveKinematics", "wpi__math__MecanumDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveOdometry",
            yml_file = "semiwrap/MecanumDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveOdometry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveOdometry", "wpi__math__MecanumDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveOdometry3d",
            yml_file = "semiwrap/MecanumDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveOdometry3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveOdometry3d", "wpi__math__MecanumDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelPositions",
            yml_file = "semiwrap/MecanumDriveWheelPositions.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelPositions.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelPositions", "wpi__math__MecanumDriveWheelPositions.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelSpeeds",
            yml_file = "semiwrap/MecanumDriveWheelSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelSpeeds", "wpi__math__MecanumDriveWheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelAccelerations",
            yml_file = "semiwrap/MecanumDriveWheelAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelAccelerations", "wpi__math__MecanumDriveWheelAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "Odometry",
            yml_file = "semiwrap/Odometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Odometry.hpp",
            tmpl_class_names = [
                ("Odometry_tmpl1", "DifferentialDriveOdometryBase"),
                ("Odometry_tmpl2", "MecanumDriveOdometryBase"),
                ("Odometry_tmpl3", "SwerveDrive2OdometryBase"),
                ("Odometry_tmpl4", "SwerveDrive3OdometryBase"),
                ("Odometry_tmpl5", "SwerveDrive4OdometryBase"),
                ("Odometry_tmpl6", "SwerveDrive6OdometryBase"),
            ],
            trampolines = [
                ("wpi::math::Odometry", "wpi__math__Odometry.hpp"),
            ],
        ),
        struct(
            class_name = "Odometry3d",
            yml_file = "semiwrap/Odometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Odometry3d.hpp",
            tmpl_class_names = [
                ("Odometry3d_tmpl1", "DifferentialDriveOdometry3dBase"),
                ("Odometry3d_tmpl2", "MecanumDriveOdometry3dBase"),
                ("Odometry3d_tmpl3", "SwerveDrive2Odometry3dBase"),
                ("Odometry3d_tmpl4", "SwerveDrive3Odometry3dBase"),
                ("Odometry3d_tmpl5", "SwerveDrive4Odometry3dBase"),
                ("Odometry3d_tmpl6", "SwerveDrive6Odometry3dBase"),
            ],
            trampolines = [
                ("wpi::math::Odometry3d", "wpi__math__Odometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveKinematics",
            yml_file = "semiwrap/SwerveDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveKinematics.hpp",
            tmpl_class_names = [
                ("SwerveDriveKinematics_tmpl1", "SwerveDrive2Kinematics"),
                ("SwerveDriveKinematics_tmpl2", "SwerveDrive3Kinematics"),
                ("SwerveDriveKinematics_tmpl3", "SwerveDrive4Kinematics"),
                ("SwerveDriveKinematics_tmpl4", "SwerveDrive6Kinematics"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveKinematics", "wpi__math__SwerveDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveOdometry",
            yml_file = "semiwrap/SwerveDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveOdometry.hpp",
            tmpl_class_names = [
                ("SwerveDriveOdometry_tmpl1", "SwerveDrive2Odometry"),
                ("SwerveDriveOdometry_tmpl2", "SwerveDrive3Odometry"),
                ("SwerveDriveOdometry_tmpl3", "SwerveDrive4Odometry"),
                ("SwerveDriveOdometry_tmpl4", "SwerveDrive6Odometry"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveOdometry", "wpi__math__SwerveDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveOdometry3d",
            yml_file = "semiwrap/SwerveDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveOdometry3d.hpp",
            tmpl_class_names = [
                ("SwerveDriveOdometry3d_tmpl1", "SwerveDrive2Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl2", "SwerveDrive3Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl3", "SwerveDrive4Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl4", "SwerveDrive6Odometry3d"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveOdometry3d", "wpi__math__SwerveDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModulePosition",
            yml_file = "semiwrap/SwerveModulePosition.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModulePosition.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModulePosition", "wpi__math__SwerveModulePosition.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModuleState",
            yml_file = "semiwrap/SwerveModuleState.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModuleState.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModuleState", "wpi__math__SwerveModuleState.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModuleAcceleration",
            yml_file = "semiwrap/SwerveModuleAcceleration.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModuleAcceleration.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModuleAcceleration", "wpi__math__SwerveModuleAcceleration.hpp"),
            ],
        ),
        struct(
            class_name = "SimulatedAnnealing",
            yml_file = "semiwrap/SimulatedAnnealing.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/optimization/SimulatedAnnealing.hpp",
            tmpl_class_names = [
                ("SimulatedAnnealing_tmpl1", "SimulatedAnnealing"),
            ],
            trampolines = [
                ("wpi::math::SimulatedAnnealing", "wpi__math__SimulatedAnnealing.hpp"),
            ],
        ),
        struct(
            class_name = "TravelingSalesman",
            yml_file = "semiwrap/TravelingSalesman.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/path/TravelingSalesman.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TravelingSalesman", "wpi__math__TravelingSalesman.hpp"),
            ],
        ),
        struct(
            class_name = "CubicHermiteSpline",
            yml_file = "semiwrap/CubicHermiteSpline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/CubicHermiteSpline.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CubicHermiteSpline", "wpi__math__CubicHermiteSpline.hpp"),
            ],
        ),
        struct(
            class_name = "QuinticHermiteSpline",
            yml_file = "semiwrap/QuinticHermiteSpline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/QuinticHermiteSpline.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::QuinticHermiteSpline", "wpi__math__QuinticHermiteSpline.hpp"),
            ],
        ),
        struct(
            class_name = "Spline",
            yml_file = "semiwrap/Spline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/Spline.hpp",
            tmpl_class_names = [
                ("Spline_tmpl1", "Spline3"),
                ("Spline_tmpl2", "Spline5"),
            ],
            trampolines = [
                ("wpi::math::Spline", "wpi__math__Spline.hpp"),
                ("wpi::math::Spline::ControlVector", "wpi__math__Spline__ControlVector.hpp"),
            ],
        ),
        struct(
            class_name = "SplineHelper",
            yml_file = "semiwrap/SplineHelper.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/SplineHelper.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineHelper", "wpi__math__SplineHelper.hpp"),
            ],
        ),
        struct(
            class_name = "SplineParameterizer",
            yml_file = "semiwrap/SplineParameterizer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/SplineParameterizer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineParameterizer", "wpi__math__SplineParameterizer.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystem",
            yml_file = "semiwrap/LinearSystem.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/LinearSystem.hpp",
            tmpl_class_names = [
                ("LinearSystem_tmpl1", "LinearSystem_1_1_1"),
                ("LinearSystem_tmpl2", "LinearSystem_1_1_2"),
                ("LinearSystem_tmpl3", "LinearSystem_1_1_3"),
                ("LinearSystem_tmpl4", "LinearSystem_2_1_1"),
                ("LinearSystem_tmpl5", "LinearSystem_2_1_2"),
                ("LinearSystem_tmpl6", "LinearSystem_2_1_3"),
                ("LinearSystem_tmpl7", "LinearSystem_2_2_1"),
                ("LinearSystem_tmpl8", "LinearSystem_2_2_2"),
                ("LinearSystem_tmpl9", "LinearSystem_2_2_3"),
                ("LinearSystem_tmpl10", "LinearSystem_3_2_1"),
                ("LinearSystem_tmpl11", "LinearSystem_3_2_2"),
                ("LinearSystem_tmpl12", "LinearSystem_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::LinearSystem", "wpi__math__LinearSystem.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystemLoop",
            yml_file = "semiwrap/LinearSystemLoop.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/LinearSystemLoop.hpp",
            tmpl_class_names = [
                ("LinearSystemLoop_tmpl1", "LinearSystemLoop_1_1_1"),
                ("LinearSystemLoop_tmpl2", "LinearSystemLoop_2_1_1"),
                ("LinearSystemLoop_tmpl3", "LinearSystemLoop_2_1_2"),
                ("LinearSystemLoop_tmpl4", "LinearSystemLoop_2_2_2"),
                ("LinearSystemLoop_tmpl5", "LinearSystemLoop_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::LinearSystemLoop", "wpi__math__LinearSystemLoop.hpp"),
            ],
        ),
        struct(
            class_name = "DCMotor",
            yml_file = "semiwrap/DCMotor.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/plant/DCMotor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DCMotor", "wpi__math__DCMotor.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystemId",
            yml_file = "semiwrap/LinearSystemId.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/plant/LinearSystemId.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LinearSystemId", "wpi__math__LinearSystemId.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialSample",
            yml_file = "semiwrap/DifferentialSample.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/DifferentialSample.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialSample", "wpi__math__DifferentialSample.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialTrajectory",
            yml_file = "semiwrap/DifferentialTrajectory.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/DifferentialTrajectory.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialTrajectory", "wpi__math__DifferentialTrajectory.hpp"),
            ],
        ),
        struct(
            class_name = "ExponentialProfile",
            yml_file = "semiwrap/ExponentialProfile.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/ExponentialProfile.hpp",
            tmpl_class_names = [
                ("ExponentialProfile_tmpl1", "ExponentialProfileMeterVolts"),
            ],
            trampolines = [
                ("wpi::math::ExponentialProfile", "wpi__math__ExponentialProfile.hpp"),
                ("wpi::math::ExponentialProfile::Constraints", "wpi__math__ExponentialProfile__Constraints.hpp"),
                ("wpi::math::ExponentialProfile::State", "wpi__math__ExponentialProfile__State.hpp"),
                ("wpi::math::ExponentialProfile::ProfileTiming", "wpi__math__ExponentialProfile__ProfileTiming.hpp"),
            ],
        ),
        struct(
            class_name = "HolonomicTrajectory",
            yml_file = "semiwrap/HolonomicTrajectory.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/HolonomicTrajectory.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::HolonomicTrajectory", "wpi__math__HolonomicTrajectory.hpp"),
            ],
        ),
        struct(
            class_name = "SplineSample",
            yml_file = "semiwrap/SplineSample.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/SplineSample.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineSample", "wpi__math__SplineSample.hpp"),
            ],
        ),
        struct(
            class_name = "SplineTrajectory",
            yml_file = "semiwrap/SplineTrajectory.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/SplineTrajectory.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineTrajectory", "wpi__math__SplineTrajectory.hpp"),
            ],
        ),
        struct(
            class_name = "Trajectory",
            yml_file = "semiwrap/Trajectory.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/Trajectory.hpp",
            tmpl_class_names = [
                ("Trajectory_tmpl1", "SplineTrajectoryBase"),
                ("Trajectory_tmpl2", "DifferentialTrajectoryBase"),
                ("Trajectory_tmpl3", "HolonomicTrajectoryBase"),
            ],
            trampolines = [
                ("wpi::math::Trajectory", "wpi__math__Trajectory.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryConfig",
            yml_file = "semiwrap/TrajectoryConfig.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryConfig.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryConfig", "wpi__math__TrajectoryConfig.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryGenerator",
            yml_file = "semiwrap/TrajectoryGenerator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryGenerator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryGenerator", "wpi__math__TrajectoryGenerator.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryParameterizer",
            yml_file = "semiwrap/TrajectoryParameterizer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryParameterizer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryParameterizer", "wpi__math__TrajectoryParameterizer.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectorySample",
            yml_file = "semiwrap/TrajectorySample.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectorySample.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectorySample", "wpi__math__TrajectorySample.hpp"),
            ],
        ),
        struct(
            class_name = "TrapezoidProfile",
            yml_file = "semiwrap/TrapezoidProfile.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrapezoidProfile.hpp",
            tmpl_class_names = [
                ("TrapezoidProfile_tmpl1", "TrapezoidProfile"),
                ("TrapezoidProfile_tmpl2", "TrapezoidProfileRadians"),
            ],
            trampolines = [
                ("wpi::math::TrapezoidProfile", "wpi__math__TrapezoidProfile.hpp"),
                ("wpi::math::TrapezoidProfile::Constraints", "wpi__math__TrapezoidProfile__Constraints.hpp"),
                ("wpi::math::TrapezoidProfile::State", "wpi__math__TrapezoidProfile__State.hpp"),
            ],
        ),
        struct(
            class_name = "CentripetalAccelerationConstraint",
            yml_file = "semiwrap/CentripetalAccelerationConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/CentripetalAccelerationConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CentripetalAccelerationConstraint", "wpi__math__CentripetalAccelerationConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveKinematicsConstraint",
            yml_file = "semiwrap/DifferentialDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/DifferentialDriveKinematicsConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveKinematicsConstraint", "wpi__math__DifferentialDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveVoltageConstraint",
            yml_file = "semiwrap/DifferentialDriveVoltageConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/DifferentialDriveVoltageConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveVoltageConstraint", "wpi__math__DifferentialDriveVoltageConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "EllipticalRegionConstraint",
            yml_file = "semiwrap/EllipticalRegionConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/EllipticalRegionConstraint.hpp",
            tmpl_class_names = [
                ("EllipticalRegionConstraint_tmpl1", "EllipticalRegionConstraint"),
            ],
            trampolines = [
                ("wpi::math::EllipticalRegionConstraint", "wpi__math__EllipticalRegionConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "MaxVelocityConstraint",
            yml_file = "semiwrap/MaxVelocityConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/MaxVelocityConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MaxVelocityConstraint", "wpi__math__MaxVelocityConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveKinematicsConstraint",
            yml_file = "semiwrap/MecanumDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/MecanumDriveKinematicsConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveKinematicsConstraint", "wpi__math__MecanumDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "RectangularRegionConstraint",
            yml_file = "semiwrap/RectangularRegionConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/RectangularRegionConstraint.hpp",
            tmpl_class_names = [
                ("RectangularRegionConstraint_tmpl1", "RectangularRegionConstraint"),
            ],
            trampolines = [
                ("wpi::math::RectangularRegionConstraint", "wpi__math__RectangularRegionConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveKinematicsConstraint",
            yml_file = "semiwrap/SwerveDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/SwerveDriveKinematicsConstraint.hpp",
            tmpl_class_names = [
                ("SwerveDriveKinematicsConstraint_tmpl1", "SwerveDrive2KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl2", "SwerveDrive3KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl3", "SwerveDrive4KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl4", "SwerveDrive6KinematicsConstraint"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveKinematicsConstraint", "wpi__math__SwerveDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryConstraint",
            yml_file = "semiwrap/TrajectoryConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/TrajectoryConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryConstraint", "wpi__math__TrajectoryConstraint.hpp"),
                ("wpi::math::TrajectoryConstraint::MinMax", "wpi__math__TrajectoryConstraint__MinMax.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath.casters.pkl",
        dep_file = "wpimath.casters.d",
    )

    gen_libinit(
        name = "wpimath.gen_lib_init",
        output_file = "src/main/python/wpimath/_init__wpimath.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "wpimath.gen_pkgconf",
        libinit_py = "wpimath._init__wpimath",
        module_pkg_name = "wpimath._wpimath",
        output_file = "wpimath.pc",
        pkg_name = "wpimath",
        install_path = "src/main/python/wpimath",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_HEADER_GEN],
        libname = "_wpimath",
        output_file = "semiwrap_init.wpimath._wpimath.hpp",
    )

    run_header_gen(
        name = "wpimath",
        casters_pickle = "wpimath.casters.pkl",
        header_gen_config = WPIMATH_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath",
        install_path = "src/main/python/wpimath/",
        extension_name = "_wpimath",
        generated_srcs = [":wpimath.generated_srcs"],
        semiwrap_header = [":wpimath.gen_modinit_hpp"],
        deps = [
            ":wpimath.tmpl_hdrs",
            ":wpimath.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath.generated_files",
        srcs = [
            "wpimath.gen_modinit_hpp.gen",
            "wpimath.header_gen_files",
            "wpimath.gen_pkgconf",
            "wpimath.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def publish_library_casters():
    publish_casters(
        name = "publish_casters",
        caster_name = "wpimath-casters",
        output_json = "src/main/python/wpimath/wpimath-casters.pybind11.json",
        output_pc = "src/main/python/wpimath/wpimath-casters.pc",
        project_config = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
        typecasters_srcs = native.glob(["src/main/python/wpimath/_impl/src/**", "src/main/python/wpimath/_impl/src/type_casters/**"]),
    )

    cc_library(
        name = "wpimath-casters",
        hdrs = native.glob(["src/main/python/wpimath/_impl/src/*.h", "src/main/python/wpimath/_impl/src/type_casters/*.h"]),
        includes = ["src/main/python/wpimath/_impl/src", "src/main/python/wpimath/_impl/src/type_casters"],
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpimath.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/wpimath/wpimath.pc",
            "src/main/python/wpimath/wpimath-casters.pc",
            "src/main/python/wpimath/wpimath-casters.pybind11.json",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpimath/**"], exclude = ["src/main/python/wpimath/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/wpimath/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/wpimath/**/*.py"]) + [
            "src/main/python/wpimath/_init__wpimath.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpimath/_wpimath",
            ":wpimath.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//wpimath:robotpy-native-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/wpimath/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpimath:robotpy-native-wpimath.copy_headers",
        ],
        package_root_file = "src/main/python/wpimath/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
