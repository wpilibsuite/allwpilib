// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <imgui.h>

void SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0,
                      const ImVec2& pivot = ImVec2(0, 0));
void SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);
