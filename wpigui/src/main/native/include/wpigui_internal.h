// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace wpi::gui {

struct SavedSettings {
  bool loadedWidthHeight = false;
  int width;
  int height;
  int maximized = 0;
  int xPos = -1;
  int yPos = -1;
  int userScale = 2;
  int style = 0;
};

struct Font {
  static constexpr int kScaledLevels = 9;
  ImFont* scaled[kScaledLevels];
};

struct Context : public SavedSettings {
  std::atomic_bool exit{false};

  std::string title;
  int defaultWidth;
  int defaultHeight;

  GLFWwindow* window = nullptr;

  std::vector<std::function<void()>> initializers;
  std::vector<std::function<void(float scale)>> windowScalers;
  std::vector<std::pair<
      const char*,
      std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>>>
      makeFonts;

  ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::vector<std::function<void()>> earlyExecutors;
  std::vector<std::function<void()>> lateExecutors;

  int fontScale = 2;  // updated by main loop
  std::vector<Font> fonts;

  std::vector<GLFWimage> icons;

  std::string iniPath = "imgui.ini";
};

extern Context* gContext;

void PlatformCreateContext();
void PlatformDestroyContext();
void PlatformGlfwInitHints();
void PlatformGlfwWindowHints();
bool PlatformInitRenderer();
void PlatformRenderFrame();
void PlatformShutdown();
void PlatformFramebufferSizeChanged(int width, int height);

void CommonRenderFrame();

}  // namespace wpi::gui
