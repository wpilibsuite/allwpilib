// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"

#include "wpi/glass/other/Mechanism2D.hpp"

namespace glass {

class NTMechanism2DModel : public Mechanism2DModel {
 public:
  static constexpr const char* kType = "Mechanism2d";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTMechanism2DModel(std::string_view path);
  NTMechanism2DModel(nt::NetworkTableInstance inst, std::string_view path);
  ~NTMechanism2DModel() override;

  const char* GetPath() const { return m_path.c_str(); }
  const char* GetName() const { return m_nameValue.c_str(); }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

  frc::Translation2d GetDimensions() const override {
    return m_dimensionsValue;
  }
  ImU32 GetBackgroundColor() const override { return m_bgColorValue; }
  void ForEachRoot(
      wpi::function_ref<void(MechanismRootModel& model)> func) override;

 private:
  nt::NetworkTableInstance m_inst;
  std::string m_path;
  nt::MultiSubscriber m_tableSub;
  nt::Topic m_nameTopic;
  nt::Topic m_dimensionsTopic;
  nt::Topic m_bgColorTopic;
  nt::NetworkTableListenerPoller m_poller;

  std::string m_nameValue;
  frc::Translation2d m_dimensionsValue;
  ImU32 m_bgColorValue = 0;

  class NTMechanismObjectModel;
  class NTMechanismGroupImpl final {
   public:
    NTMechanismGroupImpl(nt::NetworkTableInstance inst, std::string_view path,
                         std::string_view name)
        : m_inst{inst}, m_path{path}, m_name{name} {}

    const char* GetName() const { return m_name.c_str(); }
    void ForEachObject(
        wpi::function_ref<void(MechanismObjectModel& model)> func);

    void NTUpdate(const nt::Event& event, std::string_view name);

   protected:
    nt::NetworkTableInstance m_inst;
    std::string m_path;
    std::string m_name;
    std::vector<std::unique_ptr<NTMechanismObjectModel>> m_objects;
  };

  class NTMechanismObjectModel final : public MechanismObjectModel {
   public:
    NTMechanismObjectModel(nt::NetworkTableInstance inst, std::string_view path,
                           std::string_view name)
        : m_group{inst, path, name},
          m_typeTopic{inst.GetTopic(fmt::format("{}/.type", path))},
          m_colorTopic{inst.GetTopic(fmt::format("{}/color", path))},
          m_weightTopic{inst.GetTopic(fmt::format("{}/weight", path))},
          m_angleTopic{inst.GetTopic(fmt::format("{}/angle", path))},
          m_lengthTopic{inst.GetTopic(fmt::format("{}/length", path))} {}

    const char* GetName() const final { return m_group.GetName(); }
    void ForEachObject(
        wpi::function_ref<void(MechanismObjectModel& model)> func) final {
      m_group.ForEachObject(func);
    }

    const char* GetType() const final { return m_typeValue.c_str(); }
    ImU32 GetColor() const final { return m_colorValue; }
    double GetWeight() const final { return m_weightValue; }
    frc::Rotation2d GetAngle() const final { return m_angleValue; }
    units::meter_t GetLength() const final { return m_lengthValue; }

    bool NTUpdate(const nt::Event& event, std::string_view name);

   private:
    NTMechanismGroupImpl m_group;

    nt::Topic m_typeTopic;
    nt::Topic m_colorTopic;
    nt::Topic m_weightTopic;
    nt::Topic m_angleTopic;
    nt::Topic m_lengthTopic;

    std::string m_typeValue;
    ImU32 m_colorValue = IM_COL32_WHITE;
    double m_weightValue = 1.0;
    frc::Rotation2d m_angleValue;
    units::meter_t m_lengthValue = 0.0_m;
  };

  class RootModel final : public MechanismRootModel {
   public:
    RootModel(nt::NetworkTableInstance inst, std::string_view path,
              std::string_view name)
        : m_group{inst, path, name},
          m_xTopic{inst.GetTopic(fmt::format("{}/x", path))},
          m_yTopic{inst.GetTopic(fmt::format("{}/y", path))} {}

    const char* GetName() const final { return m_group.GetName(); }
    void ForEachObject(
        wpi::function_ref<void(MechanismObjectModel& model)> func) final {
      m_group.ForEachObject(func);
    }

    bool NTUpdate(const nt::Event& event, std::string_view childName);

    frc::Translation2d GetPosition() const override { return m_pos; };

   private:
    NTMechanismGroupImpl m_group;
    nt::Topic m_xTopic;
    nt::Topic m_yTopic;
    frc::Translation2d m_pos;
  };

  std::vector<std::unique_ptr<RootModel>> m_roots;
};

}  // namespace glass
