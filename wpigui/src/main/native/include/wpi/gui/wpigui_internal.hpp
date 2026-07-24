// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>

struct SDL_Surface;
struct SDL_GPUDevice;
struct SDL_Window;
union SDL_Event;

namespace wpi::gui {

struct SavedSettings {
  bool loadedWidthHeight = false;
  int width;
  int height;
  bool maximized = false;
  int xPos = -1;
  int yPos = -1;
  int userScale = 100;
  int style = 0;
  int fps = 120;
  std::string defaultFontName = "Proggy Dotted";
};

struct Context : public SavedSettings {
  std::atomic_bool exit{false};

  std::string title;
  int defaultWidth;
  int defaultHeight;
  bool isRendering{false};

  SDL_Window* window = nullptr;
  SDL_GPUDevice* gpuDevice = nullptr;

  std::function<void()> loadSettings;
  std::function<void()> loadIniSettings;
  std::function<void(bool exiting)> saveSettings;
  std::vector<std::function<void()>> initializers;
  std::vector<std::function<void(float scale)>> windowScalers;
  std::vector<std::function<void()>> exitHandlers;
  class FontMaker {
   public:
    FontMaker(
        std::string name,
        std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
            func)
        : name{std::move(name)}, func{std::move(func)} {}

    const std::string& GetName() const { return name; }
    ImFont* GetFont() const;

   private:
    std::string name;
    mutable ImFont* font = nullptr;
    std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
        func;
  };
  std::vector<FontMaker> makeFonts;

  ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::vector<std::function<void(SDL_Event& event)>> eventHandlers;
  std::vector<std::function<void()>> earlyExecutors;
  std::vector<std::function<void()>> lateExecutors;

  std::vector<SDL_Surface*> icons;

  std::string iniPath = "imgui.ini";
  bool resetOnExit = false;
};

extern Context* gContext;

}  // namespace wpi::gui
