/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimGui.h"

#include <algorithm>
#include <atomic>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_ProggyDotted.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <mockdata/DriverStationData.h>
#include <wpi/StringMap.h>
#include <wpi/raw_ostream.h>

using namespace halsimgui;

namespace {
struct WindowInfo {
  WindowInfo() = default;
  explicit WindowInfo(const char* name_) : name{name_} {}
  WindowInfo(const char* name_, std::function<void()> display_,
             ImGuiWindowFlags flags_)
      : name{name_}, display{std::move(display_)}, flags{flags_} {}

  std::string name;
  std::function<void()> display;
  ImGuiWindowFlags flags = 0;
  bool visible = true;
  bool enabled = true;
  ImGuiCond posCond = 0;
  ImGuiCond sizeCond = 0;
  ImVec2 pos;
  ImVec2 size;
  bool setPadding = false;
  ImVec2 padding;
};
}  // namespace

static std::atomic_bool gExit{false};
static GLFWwindow* gWindow;
static bool gWindowLoadedWidthHeight = false;
static int gWindowWidth = 1280;
static int gWindowHeight = 720;
static int gWindowMaximized = 0;
static int gWindowXPos = -1;
static int gWindowYPos = -1;
static std::vector<std::function<void()>> gInitializers;
static std::vector<std::function<void()>> gExecutors;
static std::vector<WindowInfo> gWindows;
static wpi::StringMap<int> gWindowMap;   // index into gWindows
static std::vector<int> gSortedWindows;  // index into gWindows
static std::vector<std::function<void()>> gOptionMenus;
static std::vector<std::function<void()>> gMenus;
static int gUserScale = 2;
static int gStyle = 0;
static constexpr int kScaledFontLevels = 9;
static ImFont* gScaledFont[kScaledFontLevels];
static bool gDisableOutputsOnDSDisable = true;

static void glfw_error_callback(int error, const char* description) {
  wpi::errs() << "GLFW Error " << error << ": " << description << '\n';
}

static void glfw_window_size_callback(GLFWwindow*, int width, int height) {
  if (!gWindowMaximized) {
    gWindowWidth = width;
    gWindowHeight = height;
  }
}

static void glfw_window_maximize_callback(GLFWwindow* window, int maximized) {
  gWindowMaximized = maximized;
}

static void glfw_window_pos_callback(GLFWwindow* window, int xpos, int ypos) {
  if (!gWindowMaximized) {
    gWindowXPos = xpos;
    gWindowYPos = ypos;
  }
}

// read/write open state to ini file
static void* SimWindowsReadOpen(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler,
                                const char* name) {
  if (wpi::StringRef{name} == "GLOBAL") return &gWindow;

  int index = gWindowMap.try_emplace(name, gWindows.size()).first->second;
  if (index == static_cast<int>(gWindows.size())) {
    gSortedWindows.push_back(index);
    gWindows.emplace_back(name);
    std::sort(gSortedWindows.begin(), gSortedWindows.end(),
              [](int a, int b) { return gWindows[a].name < gWindows[b].name; });
  }
  return &gWindows[index];
}

static void SimWindowsReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                               void* entry, const char* lineStr) {
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();

  if (entry == &gWindow) {
    int num;
    if (value.getAsInteger(10, num)) return;
    if (name == "width") {
      gWindowWidth = num;
      gWindowLoadedWidthHeight = true;
    } else if (name == "height") {
      gWindowHeight = num;
      gWindowLoadedWidthHeight = true;
    } else if (name == "maximized") {
      gWindowMaximized = num;
    } else if (name == "xpos") {
      gWindowXPos = num;
    } else if (name == "ypos") {
      gWindowYPos = num;
    } else if (name == "userScale") {
      gUserScale = num;
    } else if (name == "style") {
      gStyle = num;
    } else if (name == "disableOutputsOnDS") {
      gDisableOutputsOnDSDisable = num;
    }
    return;
  }

  auto element = static_cast<WindowInfo*>(entry);
  if (name == "visible") {
    int num;
    if (value.getAsInteger(10, num)) return;
    element->visible = num;
  } else if (name == "enabled") {
    int num;
    if (value.getAsInteger(10, num)) return;
    element->enabled = num;
  }
}

static void SimWindowsWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                               ImGuiTextBuffer* out_buf) {
  out_buf->appendf(
      "[SimWindow][GLOBAL]\nwidth=%d\nheight=%d\nmaximized=%d\n"
      "xpos=%d\nypos=%d\nuserScale=%d\nstyle=%d\ndisableOutputsOnDS=%d\n",
      gWindowWidth, gWindowHeight, gWindowMaximized, gWindowXPos, gWindowYPos,
      gUserScale, gStyle, gDisableOutputsOnDSDisable ? 1 : 0);
  for (auto&& window : gWindows)
    out_buf->appendf("[SimWindow][%s]\nvisible=%d\nenabled=%d\n\n",
                     window.name.c_str(), window.visible ? 1 : 0,
                     window.enabled ? 1 : 0);
}

static void UpdateStyle() {
  switch (gStyle) {
    case 0:
      ImGui::StyleColorsClassic();
      break;
    case 1:
      ImGui::StyleColorsDark();
      break;
    case 2:
      ImGui::StyleColorsLight();
      break;
  }
}

void HALSimGui::Add(std::function<void()> initialize) {
  if (initialize) gInitializers.emplace_back(std::move(initialize));
}

void HALSimGui::AddExecute(std::function<void()> execute) {
  if (execute) gExecutors.emplace_back(std::move(execute));
}

void HALSimGui::AddWindow(const char* name, std::function<void()> display,
                          int flags) {
  if (display) {
    int index = gWindowMap.try_emplace(name, gWindows.size()).first->second;
    if (index < static_cast<int>(gWindows.size())) {
      if (gWindows[index].display) {
        wpi::errs() << "halsim_gui: ignoring duplicate window '" << name
                    << "'\n";
      } else {
        gWindows[index].display = display;
        gWindows[index].flags = flags;
      }
      return;
    }
    gSortedWindows.push_back(index);
    gWindows.emplace_back(name, std::move(display),
                          static_cast<ImGuiWindowFlags>(flags));
    std::sort(gSortedWindows.begin(), gSortedWindows.end(),
              [](int a, int b) { return gWindows[a].name < gWindows[b].name; });
  }
}

void HALSimGui::AddMainMenu(std::function<void()> menu) {
  if (menu) gMenus.emplace_back(std::move(menu));
}

void HALSimGui::AddOptionMenu(std::function<void()> menu) {
  if (menu) gOptionMenus.emplace_back(std::move(menu));
}

void HALSimGui::SetWindowVisibility(const char* name,
                                    WindowVisibility visibility) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  switch (visibility) {
    case kHide:
      window.visible = false;
      window.enabled = true;
      break;
    case kShow:
      window.visible = true;
      window.enabled = true;
      break;
    case kDisabled:
      window.enabled = false;
      break;
  }
}

void HALSimGui::SetDefaultWindowPos(const char* name, float x, float y) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.posCond = ImGuiCond_FirstUseEver;
  window.pos = ImVec2{x, y};
}

void HALSimGui::SetDefaultWindowSize(const char* name, float width,
                                     float height) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.sizeCond = ImGuiCond_FirstUseEver;
  window.size = ImVec2{width, height};
}

void HALSimGui::SetWindowPadding(const char* name, float x, float y) {
  auto it = gWindowMap.find(name);
  if (it == gWindowMap.end()) return;
  auto& window = gWindows[it->second];
  window.setPadding = true;
  window.padding = ImVec2{x, y};
}

bool HALSimGui::AreOutputsDisabled() {
  return gDisableOutputsOnDSDisable && !HALSIM_GetDriverStationEnabled();
}

bool HALSimGui::Initialize() {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
  if (!glfwInit()) return false;

    // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
  glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+
#endif

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // Hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "SimWindow";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = SimWindowsReadOpen;
  iniHandler.ReadLineFn = SimWindowsReadLine;
  iniHandler.WriteAllFn = SimWindowsWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  for (auto&& initialize : gInitializers) {
    if (initialize) initialize();
  }

  // Load INI file
  ImGui::LoadIniSettingsFromDisk(io.IniFilename);

  // Set initial window settings
  glfwWindowHint(GLFW_MAXIMIZED, gWindowMaximized ? GLFW_TRUE : GLFW_FALSE);

  if (gWindowWidth == 0 || gWindowHeight == 0) {
    gWindowWidth = 1280;
    gWindowHeight = 720;
    gWindowLoadedWidthHeight = false;
  }

  float windowScale = 1.0;
  if (!gWindowLoadedWidthHeight) {
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    // get the primary monitor work area to see if we have a reasonable initial
    // window size; if not, maximize, and default scaling to smaller
    if (GLFWmonitor* primary = glfwGetPrimaryMonitor()) {
      int monWidth, monHeight;
      glfwGetMonitorWorkarea(primary, nullptr, nullptr, &monWidth, &monHeight);
      if (monWidth < gWindowWidth || monHeight < gWindowHeight) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        windowScale = (std::min)(monWidth * 1.0 / gWindowWidth,
                                 monHeight * 1.0 / gWindowHeight);
      }
    }
  }
  if (gWindowXPos != -1 && gWindowYPos != -1)
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  // Create window with graphics context
  gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight,
                             "Robot Simulation GUI", nullptr, nullptr);
  if (!gWindow) return false;

  if (!gWindowLoadedWidthHeight) {
    if (windowScale == 1.0)
      glfwGetWindowContentScale(gWindow, &windowScale, nullptr);
    wpi::outs() << "windowScale = " << windowScale;
    // force user scale if window scale is smaller
    if (windowScale <= 0.5)
      gUserScale = 0;
    else if (windowScale <= 0.75)
      gUserScale = 1;
    if (windowScale != 1.0) {
      // scale default window positions
      for (auto&& window : gWindows) {
        if ((window.posCond & ImGuiCond_FirstUseEver) != 0) {
          window.pos.x *= windowScale;
          window.pos.y *= windowScale;
        }
      }
    }
  }

  // Update window settings
  if (gWindowXPos != -1 && gWindowYPos != -1) {
    glfwSetWindowPos(gWindow, gWindowXPos, gWindowYPos);
    glfwShowWindow(gWindow);
  }

  // Set window callbacks
  glfwGetWindowSize(gWindow, &gWindowWidth, &gWindowHeight);
  glfwSetWindowSizeCallback(gWindow, glfw_window_size_callback);
  glfwSetWindowMaximizeCallback(gWindow, glfw_window_maximize_callback);
  glfwSetWindowPosCallback(gWindow, glfw_window_pos_callback);

  glfwMakeContextCurrent(gWindow);
  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader
  if (gl3wInit() != 0) {
    wpi::errs() << "Failed to initialize OpenGL loader!\n";
    return false;
  }

  // Setup Dear ImGui style
  UpdateStyle();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'misc/fonts/README.txt' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  // this range is based on 13px being the "nominal" 100% size and going from
  // ~0.5x (7px) to ~2.0x (25px)
  for (int i = 0; i < kScaledFontLevels; ++i) {
    float size = 7.0f + i * 3.0f;
    ImFontConfig cfg;
    std::snprintf(cfg.Name, sizeof(cfg.Name), "ProggyDotted-%d",
                  static_cast<int>(size));
    gScaledFont[i] = ImGui::AddFontProggyDotted(io, size, &cfg);
  }

  return true;
}

void HALSimGui::Main(void*) {
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(gWindow) && !gExit) {
    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Scale based on OS window content scaling
    float windowScale = 1.0;
    glfwGetWindowContentScale(gWindow, &windowScale, nullptr);
    // map to closest font size: 0 = 0.5x, 1 = 0.75x, 2 = 1.0x, 3 = 1.25x,
    // 4 = 1.5x, 5 = 1.75x, 6 = 2x
    int fontScale =
        std::clamp(gUserScale + static_cast<int>((windowScale - 1.0) * 4), 0,
                   kScaledFontLevels - 1);
    ImGui::GetIO().FontDefault = gScaledFont[fontScale];

    for (auto&& execute : gExecutors) {
      if (execute) execute();
    }

    {
      ImGui::BeginMainMenuBar();

      if (ImGui::BeginMenu("Options")) {
        ImGui::MenuItem("Disable outputs on DS disable", nullptr,
                        &gDisableOutputsOnDSDisable, true);
        for (auto&& menu : gOptionMenus) {
          if (menu) menu();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginMenu("Style")) {
          bool selected;
          selected = gStyle == 0;
          if (ImGui::MenuItem("Classic", nullptr, &selected, true)) {
            gStyle = 0;
            UpdateStyle();
          }
          selected = gStyle == 1;
          if (ImGui::MenuItem("Dark", nullptr, &selected, true)) {
            gStyle = 1;
            UpdateStyle();
          }
          selected = gStyle == 2;
          if (ImGui::MenuItem("Light", nullptr, &selected, true)) {
            gStyle = 2;
            UpdateStyle();
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Zoom")) {
          for (int i = 0; i < kScaledFontLevels && (25 * (i + 2)) <= 200; ++i) {
            char label[20];
            std::snprintf(label, sizeof(label), "%d%%", 25 * (i + 2));
            bool selected = gUserScale == i;
            bool enabled = (fontScale - gUserScale + i) >= 0 &&
                           (fontScale - gUserScale + i) < kScaledFontLevels;
            if (ImGui::MenuItem(label, nullptr, &selected, enabled))
              gUserScale = i;
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Window")) {
        for (auto&& windowIndex : gSortedWindows) {
          auto& window = gWindows[windowIndex];
          ImGui::MenuItem(window.name.c_str(), nullptr, &window.visible,
                          window.enabled);
        }
        ImGui::EndMenu();
      }

      for (auto&& menu : gMenus) {
        if (menu) menu();
      }

#if 0
      char str[64];
      std::snprintf(str, sizeof(str), "%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
      ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(str).x -
                      10);
      ImGui::Text("%s", str);
#endif
      ImGui::EndMainMenuBar();
    }

    for (auto&& window : gWindows) {
      if (window.display && window.visible && window.enabled) {
        if (window.posCond != 0)
          ImGui::SetNextWindowPos(window.pos, window.posCond);
        if (window.sizeCond != 0)
          ImGui::SetNextWindowSize(window.size, window.sizeCond);
        if (window.setPadding)
          ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, window.padding);
        if (ImGui::Begin(window.name.c_str(), &window.visible, window.flags))
          window.display();
        ImGui::End();
        if (window.setPadding) ImGui::PopStyleVar();
      }
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(gWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(gWindow);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(gWindow);
  glfwTerminate();
}

void HALSimGui::Exit(void*) { gExit = true; }

extern "C" {

void HALSIMGUI_Add(void* param, void (*initialize)(void*)) {
  if (initialize) {
    HALSimGui::Add([=] { initialize(param); });
  }
}

void HALSIMGUI_AddExecute(void* param, void (*execute)(void*)) {
  if (execute) {
    HALSimGui::AddExecute([=] { execute(param); });
  }
}

void HALSIMGUI_AddWindow(const char* name, void* param, void (*display)(void*),
                         int32_t flags) {
  if (display) {
    HALSimGui::AddWindow(name, [=] { display(param); }, flags);
  }
}

void HALSIMGUI_AddMainMenu(void* param, void (*menu)(void*)) {
  if (menu) {
    HALSimGui::AddMainMenu([=] { menu(param); });
  }
}

void HALSIMGUI_AddOptionMenu(void* param, void (*menu)(void*)) {
  if (menu) {
    HALSimGui::AddOptionMenu([=] { menu(param); });
  }
}

void HALSIMGUI_SetWindowVisibility(const char* name, int32_t visibility) {
  HALSimGui::SetWindowVisibility(
      name, static_cast<HALSimGui::WindowVisibility>(visibility));
}

void HALSIMGUI_SetDefaultWindowPos(const char* name, float x, float y) {
  HALSimGui::SetDefaultWindowPos(name, x, y);
}

void HALSIMGUI_SetDefaultWindowSize(const char* name, float width,
                                    float height) {
  HALSimGui::SetDefaultWindowSize(name, width, height);
}

void HALSIMGUI_SetWindowPadding(const char* name, float x, float y) {
  HALSimGui::SetDefaultWindowSize(name, x, y);
}

int HALSIMGUI_AreOutputsDisabled(void) {
  return HALSimGui::AreOutputsDisabled();
}

}  // extern "C"
