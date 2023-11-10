// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/other/Canvas2D.h"
#include "networktables/MultiSubscriber.h"
#include "networktables/NetworkTableListener.h"
#include "wpi/struct/DynamicStruct.h"
#include <networktables/NetworkTableInstance.h>
#include <ntcore_cpp.h>
#include <span>

namespace glass {
class NTCanvas2DModel : public Canvas2DModel {
public:
  static constexpr const char *kType = "Canvas2d";

  explicit NTCanvas2DModel(std::string_view path);
  NTCanvas2DModel(nt::NetworkTableInstance inst, std::string_view path);
  ~NTCanvas2DModel() override;

  const char *GetPath() const { return m_path.c_str(); }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

  std::span<const Canvas2DLineModel *> GetLines() override;

private:
  std::string m_path;
  nt::NetworkTableInstance m_inst;
  nt::MultiSubscriber m_tableSub;
  nt::NetworkTableListenerPoller m_poller;
};
} // namespace glass
