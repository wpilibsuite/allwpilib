// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTCanvas2D.h"

#include <fmt/format.h>
#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

using namespace glass;

NTCanvas2DModel::NTCanvas2DModel(std::string_view path)
    : NTCanvas2DModel(nt::NetworkTableInstance::GetDefault(), path) {
}

NTCanvas2DModel::NTCanvas2DModel(nt::NetworkTableInstance inst, std::string_view path)
    : m_path{fmt::format("{}/", path)}, m_inst{inst}, m_tableSub{inst, {{m_path}}, {.periodic = 0.05, .sendAll = true}} {
}

NTCanvas2DModel::~NTCanvas2DModel() = default;

void NTCanvas2DModel::Update() {
}
