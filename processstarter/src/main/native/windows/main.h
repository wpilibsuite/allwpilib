// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <windows.h>

#include <filesystem>
#include <string>

int StartExeTool(std::filesystem::path& ExePath);
int StartJavaTool(std::filesystem::path& ExePath);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPTSTR pCmdLine, int nCmdShow);
