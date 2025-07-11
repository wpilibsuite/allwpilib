// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <filesystem>

int StartExeTool(std::filesystem::path& exePath);
int StartJavaTool(std::filesystem::path& exePath);
int main(int argc, char* argv[]);
