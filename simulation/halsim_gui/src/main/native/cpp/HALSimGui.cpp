/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
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
};
}  // namespace

static std::atomic_bool gExit{false};
static GLFWwindow* gWindow;
static std::vector<std::function<void()>> gInitializers;
static std::vector<std::function<void()>> gExecutors;
static std::vector<WindowInfo> gWindows;
static wpi::StringMap<int> gWindowMap;   // index into gWindows
static std::vector<int> gSortedWindows;  // index into gWindows
static std::vector<std::function<void()>> gOptionMenus;
static std::vector<std::function<void()>> gMenus;

static void glfw_error_callback(int error, const char* description) {
  wpi::errs() << "GLFW Error " << error << ": " << description << '\n';
}

// read/write open state to ini file
static void* SimWindowsReadOpen(ImGuiContext* ctx,
                                ImGuiSettingsHandler* handler,
                                const char* name) {
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
  auto element = static_cast<WindowInfo*>(entry);
  wpi::StringRef line{lineStr};
  auto [name, value] = line.split('=');
  name = name.trim();
  value = value.trim();
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
  for (auto&& window : gWindows)
    out_buf->appendf("[SimWindow][%s]\nvisible=%d\nenabled=%d\n\n",
                     window.name.c_str(), window.visible ? 1 : 0,
                     window.enabled ? 1 : 0);
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
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+
#endif

  // Create window with graphics context
  gWindow =
      glfwCreateWindow(1280, 720, "Robot Simulation GUI", nullptr, nullptr);
  if (!gWindow) return false;
  glfwMakeContextCurrent(gWindow);
  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader
  if (gl3wInit() != 0) {
    wpi::errs() << "Failed to initialize OpenGL loader!\n";
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

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

  // hook ini handler to save settings
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

    for (auto&& execute : gExecutors) {
      if (execute) execute();
    }

    {
      ImGui::BeginMainMenuBar();

      if (ImGui::BeginMenu("Options")) {
        for (auto&& menu : gOptionMenus) {
          if (menu) menu();
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
        if (ImGui::Begin(window.name.c_str(), &window.visible, window.flags))
          window.display();
        ImGui::End();
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

}  // extern "C"
