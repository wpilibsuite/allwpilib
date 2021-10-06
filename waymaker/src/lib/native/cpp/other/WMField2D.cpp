// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/WMField2D.h"
#include "frc/spline/SplineHelper.h"
#include "frc/trajectory/TrajectoryGenerator.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <fmt/format.h>
#include <wpi/Endian.h>
#include <wpi/MathExtras.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <waymaker/Waypoint.h>

using namespace glass;


class WMField2DModel::ObjectModel : public FieldObjectModel {
 public:
  
  ObjectModel(std::string_view name) : m_name{name} {}

  const char* GetName() const override { return m_name.c_str(); }

  void Update() override {}
  bool Exists() override { return exists; }
  bool IsReadOnly() override { return false; }

  wpi::span<const frc::Pose2d> GetPoses() override;
  void SetPoses(wpi::span<const frc::Pose2d> poses) override;
  void SetPose(size_t i, frc::Pose2d pose) override;
  void SetPosition(size_t i, frc::Translation2d pos) override;
  void SetRotation(size_t i, frc::Rotation2d rot) override;
  wpi::span<const frc::Pose2d> GetSpline();
  void SetSplineType(int type) override;
  int GetSplineType() override { return m_splineType; }
   private:
  void UpdateSpline();
    int m_splineType;
  std::vector<frc::Pose2d> m_spline;
  std::string m_name;
  bool exists;
  std::vector<frc::Pose2d> m_poses;
};

void WMField2DModel::ObjectModel::SetSplineType(
    int type) {
  m_splineType = type;
}

wpi::span<const frc::Pose2d> WMField2DModel::ObjectModel::GetPoses() {
  return m_poses;
}

void WMField2DModel::ObjectModel::SetPoses(wpi::span<const frc::Pose2d> poses) {
  m_poses.assign(poses.begin(), poses.end());
}

void WMField2DModel::ObjectModel::SetPose(size_t i, frc::Pose2d pose) {
  if (i < m_poses.size() && i >= 0 ) {
    m_poses[i] = pose;
  }
}

void WMField2DModel::ObjectModel::SetPosition(size_t i,
                                              frc::Translation2d pos) {
  if (i < m_poses.size() && i >= 0) {
    m_poses[i]=  frc::Pose2d{pos, m_poses[i].Rotation()};
  }
}

void WMField2DModel::ObjectModel::SetRotation(size_t i, frc::Rotation2d rot) {
  if (i < m_poses.size() && i >= 0) {
    m_poses[i] = frc::Pose2d{m_poses[i].Translation(), rot};
  }
}

WMField2DModel::WMField2DModel(std::string_view path) {
  m_nameValue = path;
  exists = true;
}

WMField2DModel::~WMField2DModel() = default;


bool WMField2DModel::IsReadOnly() {
  return false;
}

FieldObjectModel* WMField2DModel::AddFieldObject(std::string_view name) {
  auto fullName = name;
  auto [it, match] = Find(fullName);
  if (!match) {
    it = m_objects.emplace(it, std::make_unique<ObjectModel>(fullName));
  }
  return it->get();
}

void WMField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(name);
  if (match) {
    m_objects.erase(it);
  }
}

void WMField2DModel::ForEachFieldObject(
    wpi::function_ref<void(FieldObjectModel& model, std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      func(*obj, obj->GetName());
    }
  }
}

std::pair<WMField2DModel::Objects::iterator, bool> WMField2DModel::Find(
    std::string_view fullName) {
  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), fullName,
      [](const auto& e, std::string_view name) { return e->GetName() < name; });
  return {it, it != m_objects.end() && (*it)->GetName() == fullName};
}

wpi::span<const frc::Pose2d> WMField2DModel::ObjectModel::GetSpline() {
  UpdateSpline();
  return m_spline;
}

void WMField2DModel::ObjectModel::UpdateSpline() {
  std::vector<frc::SplineParameterizer::PoseWithCurvature> points;
  std::vector<frc::Pose2d> returnedPoints;
  if (m_poses.size() > 1) {
    try {
        switch (m_splineType) { case SplineType::kQuintic:
            points = frc::TrajectoryGenerator::SplinePointsFromSplines(
                frc::SplineHelper::QuinticSplinesFromWaypoints(m_poses));
            break;
          case SplineType::kCubic:
          default:
            frc::Pose2d start = m_poses[0];
            frc::Pose2d end =
                m_poses[m_poses.size() - 1];
            std::vector<frc::Translation2d> waypoints;
            if (m_poses.size() >= 3) {
              waypoints.resize(m_poses.size() - 2);
              std::transform(m_poses.begin() + 1, m_poses.end()-1,
                             waypoints.begin(),
                             [&](auto& point) { return point.Translation(); });
            }
           
            
            auto [startCV, endCV] =
                frc::SplineHelper::CubicControlVectorsFromWaypoints(
                    start, waypoints, end);
            points = frc::TrajectoryGenerator::SplinePointsFromSplines(
                frc::SplineHelper::CubicSplinesFromControlVectors(startCV, waypoints, endCV));
            break;

      }
      
    } catch (frc::SplineParameterizer::MalformedSplineException& e) {
      printf(e.what());
    }
    for (auto& point : points) {
      returnedPoints.push_back(point.first);
    }
    m_spline = returnedPoints;
  }
}

