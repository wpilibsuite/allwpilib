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
  bool maximized = false;
  int xPos = -1;
  int yPos = -1;
  int userScale = 2;
  int style = 0;
  int fps = 120;
  std::string defaultFontName = "Proggy Dotted";
};

constexpr int kFontScaledLevels = 9;

struct Context : public SavedSettings {
  std::atomic_bool exit{false};

  std::string title;
  int defaultWidth;
  int defaultHeight;
  bool isPlatformRendering{false};

  GLFWwindow* window = nullptr;

  std::function<void()> loadSettings;
  std::function<void()> loadIniSettings;
  std::function<void(bool exiting)> saveSettings;
  std::vector<std::function<void()>> initializers;
  std::vector<std::function<void(float scale)>> windowScalers;
  struct FontMaker {
    FontMaker(
        std::string name, bool defaultOnly,
        std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
            func)
        : name{std::move(name)},
          defaultOnly{defaultOnly},
          func{std::move(func)} {}

    std::string name;
    bool defaultOnly;
    std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
        func;
  };
  std::vector<FontMaker> makeFonts;

  ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::vector<std::function<void()>> earlyExecutors;
  std::vector<std::function<void()>> lateExecutors;

  int fontScale = 2;  // updated by main loop
  std::vector<ImFont*> fonts;

  std::vector<GLFWimage> icons;

  std::string iniPath = "imgui.ini";
  bool resetOnExit = false;

  bool reloadFonts = false;  // reload fonts in next PlatformRenderFrame()
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
