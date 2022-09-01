// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/system/plant/LinearSystemId.h"

using namespace frc;

LinearSystem<2, 1, 1> LinearSystemId::ElevatorSystem(DCMotor motor,
                                                     units::kilogram_t mass,
                                                     units::meter_t radius,
                                                     double G) {
  if (mass <= 0_kg) {
    throw std::domain_error("mass must be greater than zero.");
  }
  if (radius <= 0_m) {
    throw std::domain_error("radius must be greater than zero.");
  }
  if (G <= 0.0) {
    throw std::domain_error("G must be greater than zero.");
  }

  Matrixd<2, 2> A{
      {0.0, 1.0},
      {0.0, (-std::pow(G, 2) * motor.Kt /
             (motor.R * units::math::pow<2>(radius) * mass * motor.Kv))
                .value()}};
  Matrixd<2, 1> B{0.0, (G * motor.Kt / (motor.R * radius * mass)).value()};
  Matrixd<1, 2> C{1.0, 0.0};
  Matrixd<1, 1> D{0.0};

  return LinearSystem<2, 1, 1>(A, B, C, D);
}

LinearSystem<2, 1, 1> LinearSystemId::SingleJointedArmSystem(
    DCMotor motor, units::kilogram_square_meter_t J, double G) {
  if (J <= 0_kg_sq_m) {
    throw std::domain_error("J must be greater than zero.");
  }
  if (G <= 0.0) {
    throw std::domain_error("G must be greater than zero.");
  }

  Matrixd<2, 2> A{
      {0.0, 1.0},
      {0.0, (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()}};
  Matrixd<2, 1> B{0.0, (G * motor.Kt / (motor.R * J)).value()};
  Matrixd<1, 2> C{1.0, 0.0};
  Matrixd<1, 1> D{0.0};

  return LinearSystem<2, 1, 1>(A, B, C, D);
}

LinearSystem<2, 2, 2> LinearSystemId::IdentifyDrivetrainSystem(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_mps) kVAngular, decltype(1_V / 1_mps_sq) kAAngular) {
  if (kVLinear <= decltype(kVLinear){0}) {
    throw std::domain_error("Kv,linear must be greater than zero.");
  }
  if (kALinear <= decltype(kALinear){0}) {
    throw std::domain_error("Ka,linear must be greater than zero.");
  }
  if (kVAngular <= decltype(kVAngular){0}) {
    throw std::domain_error("Kv,angular must be greater than zero.");
  }
  if (kAAngular <= decltype(kAAngular){0}) {
    throw std::domain_error("Ka,angular must be greater than zero.");
  }

  double A1 = -(kVLinear.value() / kALinear.value() +
                kVAngular.value() / kAAngular.value());
  double A2 = -(kVLinear.value() / kALinear.value() -
                kVAngular.value() / kAAngular.value());
  double B1 = 1.0 / kALinear.value() + 1.0 / kAAngular.value();
  double B2 = 1.0 / kALinear.value() - 1.0 / kAAngular.value();

  Matrixd<2, 2> A = 0.5 * Matrixd<2, 2>{{A1, A2}, {A2, A1}};
  Matrixd<2, 2> B = 0.5 * Matrixd<2, 2>{{B1, B2}, {B2, B1}};
  Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
  Matrixd<2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

  return LinearSystem<2, 2, 2>(A, B, C, D);
}

LinearSystem<2, 2, 2> LinearSystemId::IdentifyDrivetrainSystem(
    decltype(1_V / 1_mps) kVLinear, decltype(1_V / 1_mps_sq) kALinear,
    decltype(1_V / 1_rad_per_s) kVAngular,
    decltype(1_V / 1_rad_per_s_sq) kAAngular, units::meter_t trackwidth) {
  if (kVLinear <= decltype(kVLinear){0}) {
    throw std::domain_error("Kv,linear must be greater than zero.");
  }
  if (kALinear <= decltype(kALinear){0}) {
    throw std::domain_error("Ka,linear must be greater than zero.");
  }
  if (kVAngular <= decltype(kVAngular){0}) {
    throw std::domain_error("Kv,angular must be greater than zero.");
  }
  if (kAAngular <= decltype(kAAngular){0}) {
    throw std::domain_error("Ka,angular must be greater than zero.");
  }
  if (trackwidth <= 0_m) {
    throw std::domain_error("r must be greater than zero.");
  }

  // We want to find a factor to include in Kv,angular that will convert
  // `u = Kv,angular omega` to `u = Kv,angular v`.
  //
  // v = omega r
  // omega = v/r
  // omega = 1/r v
  // omega = 1/(trackwidth/2) v
  // omega = 2/trackwidth v
  //
  // So multiplying by 2/trackwidth converts the angular gains from V/(rad/s)
  // to V/(m/s).
  return IdentifyDrivetrainSystem(kVLinear, kALinear,
                                  kVAngular * 2.0 / trackwidth * 1_rad,
                                  kAAngular * 2.0 / trackwidth * 1_rad);
}

LinearSystem<1, 1, 1> LinearSystemId::FlywheelSystem(
    DCMotor motor, units::kilogram_square_meter_t J, double G) {
  if (J <= 0_kg_sq_m) {
    throw std::domain_error("J must be greater than zero.");
  }
  if (G <= 0.0) {
    throw std::domain_error("G must be greater than zero.");
  }

  Matrixd<1, 1> A{
      (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()};
  Matrixd<1, 1> B{(G * motor.Kt / (motor.R * J)).value()};
  Matrixd<1, 1> C{1.0};
  Matrixd<1, 1> D{0.0};

  return LinearSystem<1, 1, 1>(A, B, C, D);
}

LinearSystem<2, 1, 2> LinearSystemId::DCMotorSystem(
    DCMotor motor, units::kilogram_square_meter_t J, double G) {
  if (J <= 0_kg_sq_m) {
    throw std::domain_error("J must be greater than zero.");
  }
  if (G <= 0.0) {
    throw std::domain_error("G must be greater than zero.");
  }

  Matrixd<2, 2> A{
      {0.0, 1.0},
      {0.0, (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).value()}};
  Matrixd<2, 1> B{0.0, (G * motor.Kt / (motor.R * J)).value()};
  Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
  Matrixd<2, 1> D{0.0, 0.0};

  return LinearSystem<2, 1, 2>(A, B, C, D);
}

LinearSystem<2, 2, 2> LinearSystemId::DrivetrainVelocitySystem(
    const DCMotor& motor, units::kilogram_t mass, units::meter_t r,
    units::meter_t rb, units::kilogram_square_meter_t J, double G) {
  if (mass <= 0_kg) {
    throw std::domain_error("mass must be greater than zero.");
  }
  if (r <= 0_m) {
    throw std::domain_error("r must be greater than zero.");
  }
  if (rb <= 0_m) {
    throw std::domain_error("rb must be greater than zero.");
  }
  if (J <= 0_kg_sq_m) {
    throw std::domain_error("J must be greater than zero.");
  }
  if (G <= 0.0) {
    throw std::domain_error("G must be greater than zero.");
  }

  auto C1 = -std::pow(G, 2) * motor.Kt /
            (motor.Kv * motor.R * units::math::pow<2>(r));
  auto C2 = G * motor.Kt / (motor.R * r);

  Matrixd<2, 2> A{{((1 / mass + units::math::pow<2>(rb) / J) * C1).value(),
                   ((1 / mass - units::math::pow<2>(rb) / J) * C1).value()},
                  {((1 / mass - units::math::pow<2>(rb) / J) * C1).value(),
                   ((1 / mass + units::math::pow<2>(rb) / J) * C1).value()}};
  Matrixd<2, 2> B{{((1 / mass + units::math::pow<2>(rb) / J) * C2).value(),
                   ((1 / mass - units::math::pow<2>(rb) / J) * C2).value()},
                  {((1 / mass - units::math::pow<2>(rb) / J) * C2).value(),
                   ((1 / mass + units::math::pow<2>(rb) / J) * C2).value()}};
  Matrixd<2, 2> C{{1.0, 0.0}, {0.0, 1.0}};
  Matrixd<2, 2> D{{0.0, 0.0}, {0.0, 0.0}};

  return LinearSystem<2, 2, 2>(A, B, C, D);
}
