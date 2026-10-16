// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

#include <imgui.h>

/**
 * Runs the datalog tool.
 *
 * @param saveDir Settings directory, or empty to use the platform default.
 * @param filenames Datalog files to initially open.
 */
void Application(std::string_view saveDir,
                 std::span<const std::string_view> filenames = {});

void SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0,
                      const ImVec2& pivot = ImVec2(0, 0));
void SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);
