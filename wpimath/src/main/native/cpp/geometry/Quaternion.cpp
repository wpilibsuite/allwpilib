// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Quaternion.h"

#include <wpi/json.h>

using namespace frc;

Quaternion::Quaternion(double w, double x, double y, double z)
    : m_r{w}, m_v{x, y, z} {}

Quaternion Quaternion::operator+(const Quaternion& other) const {
  return Quaternion{
    m_r + other.m_r,
    m_v(0) + other.m_v(0),
    m_v(1) + other.m_v(1),
    m_v(2) + other.m_v(2),
  };
}

Quaternion Quaternion::operator-(const Quaternion& other) const {
  return Quaternion{
    m_r - other.m_r,
    m_v(0) - other.m_v(0),
    m_v(1) - other.m_v(1),
    m_v(2) - other.m_v(2),
  };
}

Quaternion Quaternion::operator*(const double other) const {
 return Quaternion{
  m_r * other,
  m_v(0) * other,
  m_v(1) * other,
  m_v(2) * other,
 };
}

Quaternion Quaternion::operator/(const double other) const {
 return Quaternion{
  m_r / other,
  m_v(0) / other,
  m_v(1) / other,
  m_v(2) / other,
 };
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
  // https://en.wikipedia.org/wiki/Quaternion#Scalar_and_vector_parts
  const auto& r1 = m_r;
  const auto& v1 = m_v;
  const auto& r2 = other.m_r;
  const auto& v2 = other.m_v;

  // v₁ x v₂
  Eigen::Vector3d cross{v1(1) * v2(2) - v2(1) * v1(2),
                        v2(0) * v1(2) - v1(0) * v2(2),
                        v1(0) * v2(1) - v2(0) * v1(1)};

  // v = r₁v₂ + r₂v₁ + v₁ x v₂
  Eigen::Vector3d v = r1 * v2 + r2 * v1 + cross;

  return Quaternion{// r = r₁r₂ − v₁ ⋅ v₂
                    r1 * r2 - v1.dot(v2),
                    // v = r₁v₂ + r₂v₁ + v₁ x v₂
                    v(0), v(1), v(2)};
}

bool Quaternion::operator==(const Quaternion& other) const {
  return std::abs(W() * other.W() + m_v.dot(other.m_v)) > 1.0 - 1E-9;
}

Quaternion Quaternion::Conjugate() const {
  return Quaternion{W(), -X(), -Y(), -Z()};
}

Quaternion Quaternion::Inverse() const {
  double norm = Norm();
  return Quaternion{W(), -X(), -Y(), -Z()} / (norm * norm);
}

double Quaternion::Norm() const {
  return std::sqrt(W() * W() + m_v.dot(m_v));
}

Quaternion Quaternion::Normalize() const {
  double norm = Norm();
  if (norm == 0.0) {
    return Quaternion{};
  } else {
    return Quaternion{W(), X(), Y(), Z()} / norm;
  }
}

Quaternion Quaternion::Pow(const double other) const {
  return (Log() * other).Exp();
}

Quaternion Quaternion::Exp(const Quaternion& other) const {
  return other.Exp() * *this;
}

Quaternion Quaternion::Exp() const {
  // q = s(scalar) + v(vector)

  //exp(s)
  double scalar = std::exp(m_r);

  // ||v||
  double axial_magnitude = std::sqrt(X() * X() + Y() * Y() + Z() * Z());
  // cos(||v||)
  double cosine = std::cos(axial_magnitude);

  double axial_scalar;

  if (axial_magnitude < 1e-9) {
    // Taylor series of sin(x) near x=0: 1 - x^2 / 6 + x^4 / 120 + O(n^6)
    axial_scalar = 1 - std::pow(axial_magnitude, 2) / 6 + std::pow(axial_magnitude, 4) / 120;
  } else {
    axial_scalar = std::sin(axial_magnitude) / axial_magnitude;
  }

  // exp(s) * (cos(||v||) + v * sin(||v||) / ||v||)
  return Quaternion(
    cosine * scalar,
    X() * axial_scalar * scalar,
    Y() * axial_scalar * scalar,
    Z() * axial_scalar * scalar
  );
}

Quaternion Quaternion::Log(const Quaternion& other) const {
  return (other * Inverse()).Log();
}

Quaternion Quaternion::Log() const {
  double scalar = std::log(Norm());

  Eigen::Vector3d rvec = Normalize().ToRotationVector();

  return Quaternion{scalar, rvec(0), rvec(1), rvec(2)};
}

double Quaternion::W() const {
  return m_r;
}

double Quaternion::X() const {
  return m_v(0);
}

double Quaternion::Y() const {
  return m_v(1);
}

double Quaternion::Z() const {
  return m_v(2);
}

Eigen::Vector3d Quaternion::ToRotationVector() const {
  // See equation (31) in "Integrating Generic Sensor Fusion Algorithms with
  // Sound State Representation through Encapsulation of Manifolds"
  //
  // https://arxiv.org/pdf/1107.1119.pdf
  double norm = m_v.norm();

  if (norm < 1e-9) {
    return (2.0 / W() - 2.0 / 3.0 * norm * norm / (W() * W() * W())) * m_v;
  } else {
    if (W() < 0.0) {
      return 2.0 * std::atan2(-norm, -W()) / norm * m_v;
    } else {
      return 2.0 * std::atan2(norm, W()) / norm * m_v;
    }
  }
}

void frc::to_json(wpi::json& json, const Quaternion& quaternion) {
  json = wpi::json{{"W", quaternion.W()},
                   {"X", quaternion.X()},
                   {"Y", quaternion.Y()},
                   {"Z", quaternion.Z()}};
}

void frc::from_json(const wpi::json& json, Quaternion& quaternion) {
  quaternion =
      Quaternion{json.at("W").get<double>(), json.at("X").get<double>(),
                 json.at("Y").get<double>(), json.at("Z").get<double>()};
}
