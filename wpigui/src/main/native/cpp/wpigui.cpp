// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/gui/wpigui.hpp"

#include <stdint.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <thread>
#include <utility>

#define SDL_MAIN_HANDLED
#include <IconsFontAwesome6.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_DroidSans.h>
#include <imgui_FiraCodeRetina.h>
#include <imgui_FontAwesomeSolid.h>
#include <imgui_ProggyDotted.h>
#include <imgui_RobotoBold.h>
#include <imgui_RobotoCondensedBold.h>
#include <imgui_RobotoCondensedLight.h>
#include <imgui_RobotoCondensedRegular.h>
#include <imgui_RobotoLight.h>
#include <imgui_RobotoRegular.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#include <implot.h>
#include <stb_image.h>

#include "wpi/gui/wpigui_internal.hpp"

using namespace wpi::gui;

namespace wpi {

Context* gui::gContext;

namespace {
enum class RendererBackend { NONE, GPU, SDL_RENDERER };

struct RendererContext {
  SDL_GPUTransferBuffer* textureTransferBuffer = nullptr;
  uint32_t textureTransferBufferSize = 0;
  SDL_Renderer* sdlRenderer = nullptr;
  RendererBackend backend = RendererBackend::NONE;
  bool gpuWindowClaimed = false;
  bool imguiRendererInitialized = false;
};

RendererContext gRendererContext;

constexpr const char* WPIGUI_FORCE_RENDERER_ENV = "WPIGUI_FORCE_RENDERER";
constexpr const char* WPIGUI_SDL_GPU_DEBUG_ENV = "WPIGUI_SDL_GPU_DEBUG";

enum class RendererOverride { NONE, FORCE_2D, FORCE_3D };

static bool EqualsIgnoreCase(const char* lhs, const char* rhs) {
  while (*lhs != '\0' && *rhs != '\0') {
    if (std::tolower(static_cast<unsigned char>(*lhs)) !=
        std::tolower(static_cast<unsigned char>(*rhs))) {
      return false;
    }
    ++lhs;
    ++rhs;
  }
  return *lhs == '\0' && *rhs == '\0';
}

#if defined(__linux__)
static bool ContainsIgnoreCase(const std::string& value, const char* match) {
  const char* matchEnd = match + std::strlen(match);
  return std::search(value.begin(), value.end(), match, matchEnd,
                     [](char lhs, char rhs) {
                       return std::tolower(static_cast<unsigned char>(lhs)) ==
                              std::tolower(static_cast<unsigned char>(rhs));
                     }) != value.end();
}
#endif

static bool IsEnvironmentFlagEnabled(const char* name) {
  const char* value = std::getenv(name);
  if (!value || *value == '\0') {
    return false;
  }

  return !EqualsIgnoreCase(value, "0") && !EqualsIgnoreCase(value, "false") &&
         !EqualsIgnoreCase(value, "off") && !EqualsIgnoreCase(value, "no");
}

static RendererOverride GetRendererOverride() {
  const char* value = std::getenv(WPIGUI_FORCE_RENDERER_ENV);
  if (!value || *value == '\0' || EqualsIgnoreCase(value, "0") ||
      EqualsIgnoreCase(value, "false") || EqualsIgnoreCase(value, "off") ||
      EqualsIgnoreCase(value, "no")) {
    return RendererOverride::NONE;
  }

  if (EqualsIgnoreCase(value, "2d") || EqualsIgnoreCase(value, "sdl") ||
      EqualsIgnoreCase(value, "renderer") ||
      EqualsIgnoreCase(value, "sdlrenderer")) {
    return RendererOverride::FORCE_2D;
  }

  if (EqualsIgnoreCase(value, "3d") || EqualsIgnoreCase(value, "gpu") ||
      EqualsIgnoreCase(value, "sdlgpu")) {
    return RendererOverride::FORCE_3D;
  }

  std::fprintf(stderr,
               "%s has unsupported value \"%s\"; expected 2d or 3d. Ignoring "
               "renderer override.\n",
               WPIGUI_FORCE_RENDERER_ENV, value);
  return RendererOverride::NONE;
}

static bool IsSdlGpuDebugEnabled() {
  return IsEnvironmentFlagEnabled(WPIGUI_SDL_GPU_DEBUG_ENV);
}

static bool ApplyRendererOverride(RendererPreference* rendererPreference) {
  switch (GetRendererOverride()) {
    case RendererOverride::FORCE_2D:
      if (*rendererPreference == RendererPreference::REQUIRE_3D) {
        std::fprintf(stderr,
                     "%s=2d is set, but this wpigui application requires SDL "
                     "GPU.\n",
                     WPIGUI_FORCE_RENDERER_ENV);
        return false;
      }
      std::fprintf(stderr, "%s=2d is set; using SDL 2D renderer for wpigui.\n",
                   WPIGUI_FORCE_RENDERER_ENV);
      *rendererPreference = RendererPreference::PREFER_2D;
      return true;
    case RendererOverride::FORCE_3D:
      std::fprintf(stderr, "%s=3d is set; using SDL GPU renderer for wpigui.\n",
                   WPIGUI_FORCE_RENDERER_ENV);
      *rendererPreference = RendererPreference::REQUIRE_3D;
      return true;
    case RendererOverride::NONE:
      return true;
  }

  return true;
}

static bool ShouldUse2DRenderer(RendererPreference rendererPreference) {
  return rendererPreference == RendererPreference::PREFER_2D;
}

static void Update2DRendererVSync() {
  if (!gRendererContext.sdlRenderer) {
    return;
  }

  SDL_SetRenderVSync(gRendererContext.sdlRenderer, gContext->fps == 0 ? 1 : 0);
}

static void SetFPSInternal(int fps) {
  gContext->fps = fps;
  Update2DRendererVSync();
}

static SDL_GPUTextureFormat SDLGPUTextureFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA:
      return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    case PixelFormat::BGRA:
      return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    default:
      return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
  }
}

static SDL_PixelFormat SDLRendererTextureFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA:
      return SDL_PIXELFORMAT_RGBA32;
    case PixelFormat::BGRA:
      return SDL_PIXELFORMAT_BGRA32;
    default:
      return SDL_PIXELFORMAT_RGBA32;
  }
}

static bool UploadGPUTexture(SDL_GPUTexture* texture, int width, int height,
                             const unsigned char* data) {
  if (!texture || width <= 0 || height <= 0 || !data) {
    return texture != nullptr;
  }

  uint64_t uploadSize = static_cast<uint64_t>(width) * height * 4;
  if (uploadSize > (std::numeric_limits<uint32_t>::max)() - 1024) {
    return false;
  }

  if (gRendererContext.textureTransferBufferSize < uploadSize) {
    SDL_ReleaseGPUTransferBuffer(gContext->gpuDevice,
                                 gRendererContext.textureTransferBuffer);

    uint32_t transferSize = static_cast<uint32_t>(uploadSize + 1024);
    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = transferSize;
    gRendererContext.textureTransferBuffer =
        SDL_CreateGPUTransferBuffer(gContext->gpuDevice, &transferBufferInfo);
    if (!gRendererContext.textureTransferBuffer) {
      gRendererContext.textureTransferBufferSize = 0;
      return false;
    }
    gRendererContext.textureTransferBufferSize = transferSize;
  }

  void* texturePtr = SDL_MapGPUTransferBuffer(
      gContext->gpuDevice, gRendererContext.textureTransferBuffer, true);
  if (!texturePtr) {
    return false;
  }
  std::memcpy(texturePtr, data, static_cast<size_t>(uploadSize));
  SDL_UnmapGPUTransferBuffer(gContext->gpuDevice,
                             gRendererContext.textureTransferBuffer);

  SDL_GPUTextureTransferInfo transferInfo = {};
  transferInfo.offset = 0;
  transferInfo.transfer_buffer = gRendererContext.textureTransferBuffer;

  SDL_GPUTextureRegion textureRegion = {};
  textureRegion.texture = texture;
  textureRegion.w = static_cast<uint32_t>(width);
  textureRegion.h = static_cast<uint32_t>(height);
  textureRegion.d = 1;

  SDL_GPUCommandBuffer* commandBuffer =
      SDL_AcquireGPUCommandBuffer(gContext->gpuDevice);
  if (!commandBuffer) {
    return false;
  }
  SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
  SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);
  SDL_EndGPUCopyPass(copyPass);
  return SDL_SubmitGPUCommandBuffer(commandBuffer);
}

static bool UploadRendererTexture(SDL_Texture* texture, int width, int height,
                                  const unsigned char* data) {
  if (!texture || width <= 0 || height <= 0 || !data) {
    return texture != nullptr;
  }

  return SDL_UpdateTexture(texture, nullptr, data, width * 4);
}

static bool Get2DRendererFramebufferSize(int* width, int* height) {
  if (gContext->window &&
      SDL_GetWindowSizeInPixels(gContext->window, width, height)) {
    return true;
  }

  return SDL_GetRenderOutputSize(gRendererContext.sdlRenderer, width, height);
}

static void UpdateFramebufferScale(ImDrawData* drawData,
                                   uint32_t swapchainWidth,
                                   uint32_t swapchainHeight) {
  if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f ||
      swapchainWidth == 0 || swapchainHeight == 0) {
    return;
  }

  drawData->FramebufferScale =
      ImVec2{swapchainWidth / drawData->DisplaySize.x,
             swapchainHeight / drawData->DisplaySize.y};
}

static bool Update2DRendererScale(ImDrawData* drawData) {
  if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) {
    return false;
  }

  int framebufferWidth = 0;
  int framebufferHeight = 0;
  if (!Get2DRendererFramebufferSize(&framebufferWidth, &framebufferHeight) ||
      framebufferWidth <= 0 || framebufferHeight <= 0) {
    return false;
  }

  UpdateFramebufferScale(drawData, static_cast<uint32_t>(framebufferWidth),
                         static_cast<uint32_t>(framebufferHeight));
  return SDL_SetRenderScale(gRendererContext.sdlRenderer,
                            drawData->FramebufferScale.x,
                            drawData->FramebufferScale.y);
}

static bool CreateSDLWindow(SDL_WindowFlags windowFlags) {
  gContext->window = SDL_CreateWindow(gContext->title.c_str(), gContext->width,
                                      gContext->height, windowFlags);
  if (!gContext->window) {
    std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

static void DestroySDLWindow() {
  if (gContext && gContext->window) {
    SDL_DestroyWindow(gContext->window);
    gContext->window = nullptr;
  }
}

static void DestroySDL2DRenderer() {
  if (gRendererContext.sdlRenderer) {
    SDL_DestroyRenderer(gRendererContext.sdlRenderer);
    gRendererContext.sdlRenderer = nullptr;
  }
}

static bool TryCreateSDLWindowAnd2DRenderer(SDL_WindowFlags windowFlags,
                                            std::string* error) {
  bool created = SDL_CreateWindowAndRenderer(
      gContext->title.c_str(), gContext->width, gContext->height, windowFlags,
      &gContext->window, &gRendererContext.sdlRenderer);
  if (!created && error) {
    *error = SDL_GetError();
  }

  if (!created) {
    DestroySDL2DRenderer();
    DestroySDLWindow();
    return false;
  }

  if (!gRendererContext.sdlRenderer) {
    if (error) {
      *error = "SDL 2D renderer creation returned a null renderer";
    }
    DestroySDL2DRenderer();
    DestroySDLWindow();
    return false;
  }

  return true;
}

#if defined(__linux__)
static bool IsVideoDriverAvailable(const char* name) {
  int driverCount = SDL_GetNumVideoDrivers();
  for (int i = 0; i < driverCount; ++i) {
    const char* driverName = SDL_GetVideoDriver(i);
    if (driverName && EqualsIgnoreCase(driverName, name)) {
      return true;
    }
  }
  return false;
}

static bool IsWaylandFramebufferFailure(const std::string& error) {
  const char* videoDriver = SDL_GetCurrentVideoDriver();
  return videoDriver && EqualsIgnoreCase(videoDriver, "wayland") &&
         ContainsIgnoreCase(error, "window framebuffer support not available");
}

static float GetPrimaryDisplayContentScale() {
  SDL_DisplayID primary = SDL_GetPrimaryDisplay();
  if (primary == 0) {
    return 0.0f;
  }

  return SDL_GetDisplayContentScale(primary);
}

static void SetX11ScalingFactorHint(float scale) {
  if (scale <= 1.0f || SDL_GetHint(SDL_HINT_VIDEO_X11_SCALING_FACTOR)) {
    return;
  }
  if (scale > 10.0f) {
    scale = 10.0f;
  }

  char scaleString[16];
  std::snprintf(scaleString, sizeof(scaleString), "%.2f", scale);
  SDL_SetHintWithPriority(SDL_HINT_VIDEO_X11_SCALING_FACTOR, scaleString,
                          SDL_HINT_NORMAL);
}

static bool ReinitializeVideoWithX11() {
  if (!IsVideoDriverAvailable("x11")) {
    return false;
  }
  float x11Scale = GetPrimaryDisplayContentScale();

  std::fprintf(stderr,
               "SDL Wayland 2D renderer framebuffer support is not "
               "available; retrying with the x11 video driver.\n");

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SetX11ScalingFactorHint(x11Scale);
  SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "x11", SDL_HINT_OVERRIDE);
  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    std::fprintf(stderr, "SDL_InitSubSystem(SDL_INIT_VIDEO) failed: %s\n",
                 SDL_GetError());
    return false;
  }

  return true;
}
#endif

static bool CreateSDLWindowAnd2DRenderer(SDL_WindowFlags windowFlags) {
  std::string error;
  if (TryCreateSDLWindowAnd2DRenderer(windowFlags, &error)) {
    return true;
  }

#if defined(__linux__)
  if (IsWaylandFramebufferFailure(error) && ReinitializeVideoWithX11() &&
      TryCreateSDLWindowAnd2DRenderer(windowFlags, &error)) {
    return true;
  }
#endif

  std::fprintf(stderr, "SDL_CreateWindowAndRenderer failed: %s\n",
               error.c_str());
  return false;
}

static void ShutdownGPURenderer() {
  if (gContext && gContext->gpuDevice) {
    SDL_WaitForGPUIdle(gContext->gpuDevice);
  }

  if (gRendererContext.imguiRendererInitialized) {
    ImGui_ImplSDLGPU3_Shutdown();
    gRendererContext.imguiRendererInitialized = false;
  }

  if (gContext && gContext->gpuDevice) {
    SDL_ReleaseGPUTransferBuffer(gContext->gpuDevice,
                                 gRendererContext.textureTransferBuffer);
  }
  gRendererContext.textureTransferBuffer = nullptr;
  gRendererContext.textureTransferBufferSize = 0;

  if (gRendererContext.gpuWindowClaimed && gContext && gContext->window &&
      gContext->gpuDevice) {
    SDL_ReleaseWindowFromGPUDevice(gContext->gpuDevice, gContext->window);
  }
  gRendererContext.gpuWindowClaimed = false;

  if (gContext && gContext->gpuDevice) {
    SDL_DestroyGPUDevice(gContext->gpuDevice);
    gContext->gpuDevice = nullptr;
  }
}

static void Shutdown2DRenderer() {
  if (gRendererContext.imguiRendererInitialized) {
    ImGui_ImplSDLRenderer3_Shutdown();
    gRendererContext.imguiRendererInitialized = false;
  }

  DestroySDL2DRenderer();
}

static bool TryInitGPURenderer() {
  gContext->gpuDevice = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC |
          SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL |
          SDL_GPU_SHADERFORMAT_METALLIB,
      IsSdlGpuDebugEnabled(), nullptr);
  if (!gContext->gpuDevice) {
    std::fprintf(stderr, "SDL_CreateGPUDevice failed: %s\n", SDL_GetError());
    return false;
  }

  if (!SDL_ClaimWindowForGPUDevice(gContext->gpuDevice, gContext->window)) {
    std::fprintf(stderr, "SDL_ClaimWindowForGPUDevice failed: %s\n",
                 SDL_GetError());
    ShutdownGPURenderer();
    return false;
  }
  gRendererContext.gpuWindowClaimed = true;
  SDL_SetGPUSwapchainParameters(gContext->gpuDevice, gContext->window,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                SDL_GPU_PRESENTMODE_VSYNC);

  if (!ImGui_ImplSDL3_InitForSDLGPU(gContext->window)) {
    ShutdownGPURenderer();
    return false;
  }

  ImGui_ImplSDLGPU3_InitInfo initInfo = {};
  initInfo.Device = gContext->gpuDevice;
  initInfo.ColorTargetFormat =
      SDL_GetGPUSwapchainTextureFormat(gContext->gpuDevice, gContext->window);
  initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
  initInfo.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
  initInfo.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
  if (!ImGui_ImplSDLGPU3_Init(&initInfo)) {
    ImGui_ImplSDL3_Shutdown();
    ShutdownGPURenderer();
    return false;
  }

  gRendererContext.imguiRendererInitialized = true;
  gRendererContext.backend = RendererBackend::GPU;
  return true;
}

static bool TryInit2DRenderer() {
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    std::fprintf(stderr,
                 "SDL 2D renderer does not support ImGui "
                 "multi-viewports; disabling viewports.\n");
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
  }

  if (!gRendererContext.sdlRenderer) {
    std::fprintf(stderr, "SDL 2D renderer was not created\n");
    return false;
  }

  Update2DRendererVSync();

  if (!ImGui_ImplSDL3_InitForSDLRenderer(gContext->window,
                                         gRendererContext.sdlRenderer)) {
    Shutdown2DRenderer();
    return false;
  }

  if (!ImGui_ImplSDLRenderer3_Init(gRendererContext.sdlRenderer)) {
    ImGui_ImplSDL3_Shutdown();
    Shutdown2DRenderer();
    return false;
  }

  gRendererContext.imguiRendererInitialized = true;
  gRendererContext.backend = RendererBackend::SDL_RENDERER;
  return true;
}

static void NewRendererFrame() {
  switch (gRendererContext.backend) {
    case RendererBackend::GPU:
      ImGui_ImplSDLGPU3_NewFrame();
      break;
    case RendererBackend::SDL_RENDERER:
      ImGui_ImplSDLRenderer3_NewFrame();
      break;
    default:
      break;
  }
}

static void BuildImGuiFrame() {
  ImGui_ImplSDL3_NewFrame();
  // Start the Dear ImGui frame
  ImGui::NewFrame();

  for (size_t i = 0; i < gContext->earlyExecutors.size(); ++i) {
    auto& execute = gContext->earlyExecutors[i];
    if (execute) {
      execute();
    }
  }

  for (size_t i = 0; i < gContext->lateExecutors.size(); ++i) {
    auto& execute = gContext->lateExecutors[i];
    if (execute) {
      execute();
    }
  }

  // Rendering
  ImGui::Render();
}

static void RenderGPUFrame(ImDrawData* drawData) {
  bool isMinimized =
      drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f;
  if (isMinimized) {
    return;
  }

  SDL_GPUCommandBuffer* commandBuffer =
      SDL_AcquireGPUCommandBuffer(gContext->gpuDevice);
  if (!commandBuffer) {
    return;
  }

  SDL_GPUTexture* swapchainTexture = nullptr;
  uint32_t swapchainWidth = 0;
  uint32_t swapchainHeight = 0;
  if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, gContext->window,
                                             &swapchainTexture, &swapchainWidth,
                                             &swapchainHeight)) {
    SDL_CancelGPUCommandBuffer(commandBuffer);
    return;
  }

  if (swapchainTexture) {
    UpdateFramebufferScale(drawData, swapchainWidth, swapchainHeight);
    ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

    SDL_GPUColorTargetInfo targetInfo = {};
    targetInfo.texture = swapchainTexture;
    targetInfo.clear_color =
        SDL_FColor{gContext->clearColor.x, gContext->clearColor.y,
                   gContext->clearColor.z, gContext->clearColor.w};
    targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    targetInfo.store_op = SDL_GPU_STOREOP_STORE;
    targetInfo.mip_level = 0;
    targetInfo.layer_or_depth_plane = 0;
    targetInfo.cycle = false;
    SDL_GPURenderPass* renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);

    ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);

    SDL_EndGPURenderPass(renderPass);
  }

  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  SDL_SubmitGPUCommandBuffer(commandBuffer);
}

static void Render2DFrame(ImDrawData* drawData) {
  if (!Update2DRendererScale(drawData)) {
    return;
  }

  SDL_SetRenderDrawColorFloat(gRendererContext.sdlRenderer,
                              gContext->clearColor.x, gContext->clearColor.y,
                              gContext->clearColor.z, gContext->clearColor.w);
  SDL_RenderClear(gRendererContext.sdlRenderer);
  ImGui_ImplSDLRenderer3_RenderDrawData(drawData, gRendererContext.sdlRenderer);
  SDL_RenderPresent(gRendererContext.sdlRenderer);
}

static bool InitRenderer(SDL_WindowFlags windowFlags,
                         RendererPreference rendererPreference) {
  if (gRendererContext.sdlRenderer) {
    return TryInit2DRenderer();
  }

  if (!TryInitGPURenderer()) {
    if (rendererPreference == RendererPreference::REQUIRE_3D) {
      std::fprintf(stderr,
                   "SDL GPU is required for this wpigui application; not "
                   "falling back to the SDL 2D renderer.\n");
      return false;
    }

    std::fprintf(stderr, "Falling back to SDL 2D renderer for wpigui.\n");
    DestroySDLWindow();
    if (!CreateSDLWindowAnd2DRenderer(windowFlags)) {
      return false;
    }
    if (!TryInit2DRenderer()) {
      return false;
    }
  }

  return true;
}

static void RenderFrame(bool skipRender) {
  NewRendererFrame();
  BuildImGuiFrame();

  if (skipRender) {
    return;
  }

  ImDrawData* drawData = ImGui::GetDrawData();
  switch (gRendererContext.backend) {
    case RendererBackend::GPU:
      RenderGPUFrame(drawData);
      break;
    case RendererBackend::SDL_RENDERER:
      Render2DFrame(drawData);
      break;
    default:
      break;
  }
}

static void ShutdownRenderer() {
  switch (gRendererContext.backend) {
    case RendererBackend::GPU:
      ShutdownGPURenderer();
      break;
    case RendererBackend::SDL_RENDERER:
      Shutdown2DRenderer();
      break;
    default:
      break;
  }
  gRendererContext.backend = RendererBackend::NONE;
}
}  // namespace

static double GetTime() {
  static const double frequency = SDL_GetPerformanceFrequency();
  return static_cast<double>(SDL_GetPerformanceCounter()) / frequency;
}

static void UpdateWindowSize(int width, int height) {
  if (!gContext->maximized) {
    gContext->width = width;
    gContext->height = height;
  }
}

static void UpdateWindowPos(int xpos, int ypos) {
  if (!gContext->maximized) {
    gContext->xPos = xpos;
    gContext->yPos = ypos;
  }
}

static void HandleWindowEvent(const SDL_WindowEvent& event) {
  if (!gContext->window ||
      event.windowID != SDL_GetWindowID(gContext->window)) {
    return;
  }

  switch (event.type) {
    case SDL_EVENT_WINDOW_RESIZED:
      UpdateWindowSize(event.data1, event.data2);
      break;
    case SDL_EVENT_WINDOW_MAXIMIZED:
      gContext->maximized = true;
      break;
    case SDL_EVENT_WINDOW_RESTORED:
      gContext->maximized = false;
      SDL_GetWindowSize(gContext->window, &gContext->width, &gContext->height);
      SDL_GetWindowPosition(gContext->window, &gContext->xPos, &gContext->yPos);
      break;
    case SDL_EVENT_WINDOW_MOVED:
      UpdateWindowPos(event.data1, event.data2);
      break;
    default:
      break;
  }
}

static void* IniReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                         const char* name) {
  if (std::strcmp(name, "GLOBAL") != 0) {
    return nullptr;
  }
  return static_cast<SavedSettings*>(gContext);
}

static void IniReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        void* entry, const char* lineStr) {
  auto impl = static_cast<SavedSettings*>(entry);
  const char* value = std::strchr(lineStr, '=');
  if (!value) {
    return;
  }
  ++value;
  int num = std::atoi(value);
  if (std::strncmp(lineStr, "width=", 6) == 0) {
    impl->width = num;
    impl->loadedWidthHeight = true;
  } else if (std::strncmp(lineStr, "height=", 7) == 0) {
    impl->height = num;
    impl->loadedWidthHeight = true;
  } else if (std::strncmp(lineStr, "maximized=", 10) == 0) {
    impl->maximized = num;
  } else if (std::strncmp(lineStr, "xpos=", 5) == 0) {
    impl->xPos = num;
  } else if (std::strncmp(lineStr, "ypos=", 5) == 0) {
    impl->yPos = num;
  } else if (std::strncmp(lineStr, "userScaling=", 12) == 0) {
    impl->userScale = num;
  } else if (std::strncmp(lineStr, "style=", 6) == 0) {
    impl->style = num;
  } else if (std::strncmp(lineStr, "font=", 5) == 0) {
    impl->defaultFontName = &lineStr[5];
  } else if (std::strncmp(lineStr, "fps=", 4) == 0) {
    impl->fps = num;
  }
}

static void IniWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                        ImGuiTextBuffer* out_buf) {
  if (!gContext) {
    return;
  }
  out_buf->appendf(
      "[MainWindow][GLOBAL]\nwidth=%d\nheight=%d\nmaximized=%d\n"
      "xpos=%d\nypos=%d\nuserScaling=%d\nstyle=%d\nfont=%s\nfps=%d\n\n",
      gContext->width, gContext->height, gContext->maximized ? 1 : 0,
      gContext->xPos, gContext->yPos, gContext->userScale, gContext->style,
      gContext->defaultFontName.c_str(), gContext->fps);
}

void gui::CreateContext() {
  gContext = new Context;
  AddDefaultFont("Proggy Dotted",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontProggyDotted(io, size, cfg);
                 });
  AddDefaultFont("Droid Sans",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontDroidSans(io, size, cfg);
                 });
  AddDefaultFont("Fira Code Retina",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontFiraCodeRetina(io, size, cfg);
                 });
  AddDefaultFont("Roboto Light",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoLight(io, size, cfg);
                 });
  AddDefaultFont("Roboto Regular",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoRegular(io, size, cfg);
                 });
  AddDefaultFont("Roboto Bold",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoBold(io, size, cfg);
                 });
  AddDefaultFont("Roboto Condensed Light",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoCondensedLight(io, size, cfg);
                 });
  AddDefaultFont("Roboto Condensed Regular",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoCondensedRegular(io, size, cfg);
                 });
  AddDefaultFont("Roboto Condensed Bold",
                 [](ImGuiIO& io, float size, const ImFontConfig* cfg) {
                   return ImGui::AddFontRobotoCondensedBold(io, size, cfg);
                 });
}

void gui::DestroyContext() {
  ShutdownRenderer();
  delete gContext;
  gContext = nullptr;
}

Context* gui::GetCurrentContext() {
  return gContext;
}

void gui::SetCurrentContext(Context* context) {
  gContext = context;
}

ImFont* Context::FontMaker::GetFont() const {
  if (!font) {
    auto& io = ImGui::GetIO();

    ImFontConfig cfg;
    std::snprintf(cfg.Name, sizeof(cfg.Name), "%s", name.c_str());
    font = func(io, 0, &cfg);

    // Merge font awesome solid
    ImFontConfig icons_cfg;
    icons_cfg.MergeMode = true;
    icons_cfg.PixelSnapH = true;
    ImGui::AddFontFontAwesomeSolid(io, 0, &icons_cfg);
  }
  return font;
}

static void UpdateFontScale() {
  // Scale based on OS window content scaling
  float windowScale = 1.0;
  if (gContext->window) {
    windowScale = SDL_GetWindowDisplayScale(gContext->window);
    if (windowScale <= 0.0f) {
      windowScale = 1.0f;
    }
  }
  float fontScale = gContext->userScale / 100.0 * windowScale;
  if (fontScale < 0.5) {
    fontScale = 0.5;
  }
  ImGui::GetStyle().FontSizeBase = 10;
  ImGui::GetStyle().FontScaleDpi = fontScale;
}

static void ShutdownGuiSystem(bool deleteSettingsFile) {
  for (auto&& shutdown : gContext->exitHandlers) {
    if (shutdown) {
      shutdown();
    }
  }
  gContext->exitHandlers.clear();

  bool shutdownSDLPlatform = gRendererContext.backend != RendererBackend::NONE;
  ShutdownRenderer();
  if (shutdownSDLPlatform) {
    ImGui_ImplSDL3_Shutdown();
  }

  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  if (deleteSettingsFile) {
    std::remove(gContext->iniPath.c_str());
  }

  DestroySDLWindow();

  SDL_Quit();
}

static void CleanupFailedInitialize() {
  ShutdownGuiSystem(false);
}

bool gui::Initialize(const char* title, int width, int height,
                     RendererPreference rendererPreference,
                     ImGuiConfigFlags configFlags) {
  gContext->title = title;
  gContext->width = width;
  gContext->height = height;
  gContext->defaultWidth = width;
  gContext->defaultHeight = height;

  if (!ApplyRendererOverride(&rendererPreference)) {
    return false;
  }
  bool use2DRenderer = ShouldUse2DRenderer(rendererPreference);

  // Setup SDL
  SDL_SetMainReady();
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#elif defined(SDL_HINT_IME_IMPLEMENTED_UI)
  SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition,candidates");
#endif
  SDL_SetHint("SDL_HIDAPI_LIBUSB", "1");
  SDL_SetHint("SDL_HIDAPI_LIBUSB_GAMECUBE", "1");
  SDL_SetHint("SDL_HIDAPI_LIBUSB_WHITELIST", "1");
  SDL_SetHint("SDL_JOYSTICK_HIDAPI", "1");
  SDL_SetHint("SDL_JOYSTICK_ENHANCED_REPORTS", "auto");
  SDL_SetHint("SDL_JOYSTICK_HIDAPI_STEAM", "1");
  SDL_SetHint("SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");
  SDL_SetHint("SDL_JOYSTICK_LINUX_DEADZONES", "1");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)) {
    std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= configFlags;

  // Hook ini handler to save settings
  ImGuiSettingsHandler iniHandler;
  iniHandler.TypeName = "MainWindow";
  iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
  iniHandler.ReadOpenFn = IniReadOpen;
  iniHandler.ReadLineFn = IniReadLine;
  iniHandler.WriteAllFn = IniWriteAll;
  ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);

  if (gContext->loadSettings) {
    gContext->loadSettings();
    io.IniFilename = nullptr;
  } else {
    io.IniFilename = gContext->iniPath.c_str();
  }

  for (auto&& initialize : gContext->initializers) {
    if (initialize) {
      initialize();
    }
  }

  // Load INI file
  if (gContext->loadIniSettings) {
    gContext->loadIniSettings();
  } else if (io.IniFilename) {
    ImGui::LoadIniSettingsFromDisk(io.IniFilename);
  }

  // Set initial window settings
  if (gContext->width == 0 || gContext->height == 0) {
    gContext->width = gContext->defaultWidth;
    gContext->height = gContext->defaultHeight;
    gContext->loadedWidthHeight = false;
  }

  float windowScale = 1.0;
  bool maximizeWindow = gContext->maximized;
  if (!gContext->loadedWidthHeight) {
    // get the primary monitor work area to see if we have a reasonable initial
    // window size; if not, maximize, and default scaling to smaller
    SDL_DisplayID primary = SDL_GetPrimaryDisplay();
    if (primary != 0) {
      SDL_Rect bounds;
      if (SDL_GetDisplayUsableBounds(primary, &bounds)) {
        if (bounds.w < gContext->width || bounds.h < gContext->height) {
          maximizeWindow = true;
          windowScale = (std::min)(bounds.w * 1.0 / gContext->width,
                                   bounds.h * 1.0 / gContext->height);
        }
      }
    }
  }

  SDL_WindowFlags windowFlags =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  if (gContext->xPos != -1 && gContext->yPos != -1) {
    windowFlags |= SDL_WINDOW_HIDDEN;
  }
  if (maximizeWindow) {
    windowFlags |= SDL_WINDOW_MAXIMIZED;
  }

  if (use2DRenderer) {
    if (!CreateSDLWindowAnd2DRenderer(windowFlags)) {
      CleanupFailedInitialize();
      return false;
    }
  } else if (!CreateSDLWindow(windowFlags)) {
    CleanupFailedInitialize();
    return false;
  }

  if (!gContext->loadedWidthHeight) {
    // force user scale if window scale is smaller
    if (windowScale <= 0.5) {
      gContext->userScale = 0;
    } else if (windowScale <= 0.75) {
      gContext->userScale = 1;
    }
    if (windowScale != 1.0) {
      for (auto&& func : gContext->windowScalers) {
        func(windowScale);
      }
    }
  }

  if (!InitRenderer(windowFlags, rendererPreference)) {
    CleanupFailedInitialize();
    return false;
  }

  // Update window settings
  if (gContext->xPos != -1 && gContext->yPos != -1) {
    // check to make sure the position isn't off-screen
    bool found = false;
    int displayCount = 0;
    SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
    if (displays) {
      for (int i = 0; i < displayCount; ++i) {
        SDL_Rect bounds;
        if (!SDL_GetDisplayUsableBounds(displays[i], &bounds)) {
          continue;
        }
        if (gContext->xPos >= bounds.x &&
            gContext->xPos < (bounds.x + bounds.w) &&
            gContext->yPos >= bounds.y &&
            gContext->yPos < (bounds.y + bounds.h)) {
          found = true;
          break;
        }
      }
      SDL_free(displays);
    }
    if (found) {
      SDL_SetWindowPosition(gContext->window, gContext->xPos, gContext->yPos);
    }
    SDL_ShowWindow(gContext->window);
  }

  if (gContext->maximized && !maximizeWindow) {
    SDL_MaximizeWindow(gContext->window);
  }

  // Update window settings
  SDL_GetWindowSize(gContext->window, &gContext->width, &gContext->height);

  // Set icons
  if (!gContext->icons.empty()) {
    auto baseIt =
        std::min_element(gContext->icons.begin(), gContext->icons.end(),
                         [](SDL_Surface* a, SDL_Surface* b) {
                           int aDist = (a->w > 32 ? a->w - 32 : 32 - a->w) +
                                       (a->h > 32 ? a->h - 32 : 32 - a->h);
                           int bDist = (b->w > 32 ? b->w - 32 : 32 - b->w) +
                                       (b->h > 32 ? b->h - 32 : 32 - b->h);
                           return aDist < bDist;
                         });
    std::iter_swap(gContext->icons.begin(), baseIt);
    SDL_Surface* icon = gContext->icons.front();
    for (size_t i = 1; i < gContext->icons.size(); ++i) {
      SDL_AddSurfaceAlternateImage(icon, gContext->icons[i]);
      SDL_DestroySurface(gContext->icons[i]);
    }
    SDL_SetWindowIcon(gContext->window, icon);
    SDL_DestroySurface(icon);
    gContext->icons.clear();
  }

  // Setup Dear ImGui style
  SetStyle(static_cast<Style>(gContext->style));

  // Load Fonts
  UpdateFontScale();

  return true;
}

void gui::Main() {
  // Main loop
  while (!gContext->exit) {
    double startTime = GetTime();

    // Poll and handle events (inputs, window resize, etc.)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      for (auto&& handler : gContext->eventHandlers) {
        if (handler) {
          handler(event);
        }
      }
      ImGui_ImplSDL3_ProcessEvent(&event);

      if (event.type == SDL_EVENT_QUIT) {
        gContext->exit = true;
      } else if (event.type >= SDL_EVENT_WINDOW_FIRST &&
                 event.type <= SDL_EVENT_WINDOW_LAST) {
        HandleWindowEvent(event.window);
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
            event.window.windowID == SDL_GetWindowID(gContext->window)) {
          gContext->exit = true;
        }
      }
    }

    bool windowMinimized =
        (SDL_GetWindowFlags(gContext->window) & SDL_WINDOW_MINIMIZED) != 0;

    gContext->isRendering = true;
    UpdateFontScale();
    RenderFrame(windowMinimized);
    gContext->isRendering = false;

    auto& io = ImGui::GetIO();

    // custom saving
    if (gContext->saveSettings) {
      if (io.WantSaveIniSettings) {
        gContext->saveSettings(false);
        io.WantSaveIniSettings = false;  // reset flag
      }
    }

    if (windowMinimized) {
      SDL_Delay(10);
    } else if (gContext->fps != 0) {
      // if FPS limiting, sleep until next frame time
      double sleepTime = (1.0 / gContext->fps) - (GetTime() - startTime);
      if (sleepTime > 1e-6) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(static_cast<int64_t>(sleepTime * 1e6)));
      }
    }
  }

  // Save (if custom save)
  if (gContext->saveSettings) {
    gContext->saveSettings(true);
  }

  ShutdownGuiSystem(!gContext->saveSettings && gContext->resetOnExit);
}

ImTextureID gui::CreateTexture(PixelFormat format, int width, int height,
                               const unsigned char* data) {
  if (gRendererContext.backend == RendererBackend::NONE || width <= 0 ||
      height <= 0) {
    return ImTextureID_Invalid;
  }

  if (gRendererContext.backend == RendererBackend::GPU) {
    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDLGPUTextureFormat(format);
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = static_cast<uint32_t>(width);
    textureInfo.height = static_cast<uint32_t>(height);
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;

    SDL_GPUTexture* texture =
        SDL_CreateGPUTexture(gContext->gpuDevice, &textureInfo);
    if (!texture) {
      return ImTextureID_Invalid;
    }

    if (!UploadGPUTexture(texture, width, height, data)) {
      SDL_ReleaseGPUTexture(gContext->gpuDevice, texture);
      return ImTextureID_Invalid;
    }

    return static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(texture));
  } else if (gRendererContext.backend == RendererBackend::SDL_RENDERER) {
    SDL_Texture* texture = SDL_CreateTexture(
        gRendererContext.sdlRenderer, SDLRendererTextureFormat(format),
        SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
      return ImTextureID_Invalid;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);

    if (!UploadRendererTexture(texture, width, height, data)) {
      SDL_DestroyTexture(texture);
      return ImTextureID_Invalid;
    }

    return static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(texture));
  }

  return ImTextureID_Invalid;
}

void gui::UpdateTexture(ImTextureID texture, PixelFormat, int width, int height,
                        const unsigned char* data) {
  if (texture == ImTextureID_Invalid ||
      gRendererContext.backend == RendererBackend::NONE) {
    return;
  }

  if (gRendererContext.backend == RendererBackend::GPU) {
    UploadGPUTexture(reinterpret_cast<SDL_GPUTexture*>(texture), width, height,
                     data);
  } else if (gRendererContext.backend == RendererBackend::SDL_RENDERER) {
    UploadRendererTexture(reinterpret_cast<SDL_Texture*>(texture), width,
                          height, data);
  }
}

void gui::DeleteTexture(ImTextureID texture) {
  if (texture == ImTextureID_Invalid ||
      gRendererContext.backend == RendererBackend::NONE) {
    return;
  }

  if (gRendererContext.backend == RendererBackend::GPU) {
    SDL_ReleaseGPUTexture(gContext->gpuDevice,
                          reinterpret_cast<SDL_GPUTexture*>(texture));
  } else if (gRendererContext.backend == RendererBackend::SDL_RENDERER) {
    SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(texture));
  }
}

void gui::Exit() {
  if (!gContext) {
    return;
  }
  gContext->exit = true;
}

void gui::AddInit(std::function<void()> initialize) {
  if (initialize) {
    gContext->initializers.emplace_back(std::move(initialize));
  }
}

void gui::AddWindowScaler(std::function<void(float scale)> windowScaler) {
  if (windowScaler) {
    gContext->windowScalers.emplace_back(std::move(windowScaler));
  }
}

void gui::AddExit(std::function<void()> shutdown) {
  if (shutdown) {
    gContext->exitHandlers.emplace_back(std::move(shutdown));
  }
}

void gui::AddEarlyExecute(std::function<void()> execute) {
  if (execute) {
    gContext->earlyExecutors.emplace_back(std::move(execute));
  }
}

void gui::AddLateExecute(std::function<void()> execute) {
  if (execute) {
    gContext->lateExecutors.emplace_back(std::move(execute));
  }
}

void gui::AddEventHandler(std::function<void(SDL_Event& event)> handler) {
  if (handler) {
    gContext->eventHandlers.emplace_back(std::move(handler));
  }
}

void gui::ConfigureCustomSaveSettings(std::function<void()> load,
                                      std::function<void()> loadIni,
                                      std::function<void(bool)> save) {
  gContext->loadSettings = load;
  gContext->loadIniSettings = loadIni;
  gContext->saveSettings = save;
}

SDL_Window* gui::GetSystemWindow() {
  return gContext->window;
}

bool gui::AddIcon(const unsigned char* data, int len) {
  // Load from memory
  int width;
  int height;
  unsigned char* pixels =
      stbi_load_from_memory(data, len, &width, &height, nullptr, 4);
  if (!pixels) {
    return false;
  }

  SDL_Surface* surface =
      SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
  if (!surface) {
    stbi_image_free(pixels);
    return false;
  }

  if (!SDL_LockSurface(surface)) {
    SDL_DestroySurface(surface);
    stbi_image_free(pixels);
    return false;
  }

  auto* dst = static_cast<unsigned char*>(surface->pixels);
  for (int y = 0; y < height; ++y) {
    std::memcpy(dst + y * surface->pitch, pixels + y * width * 4, width * 4);
  }

  SDL_UnlockSurface(surface);
  stbi_image_free(pixels);
  gContext->icons.emplace_back(surface);
  return true;
}

void gui::AddDefaultFont(
    const char* name,
    std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
        makeFont) {
  if (makeFont) {
    gContext->makeFonts.emplace_back(name, std::move(makeFont));
  }
}

// https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
static void StyleColorsDeepDark() {
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding = ImVec2(8.00f, 8.00f);
  style.FramePadding = ImVec2(5.00f, 2.00f);
  style.CellPadding = ImVec2(6.00f, 6.00f);
  style.ItemSpacing = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
  style.IndentSpacing = 25;
  style.ScrollbarSize = 15;
  style.GrabMinSize = 10;
  style.WindowBorderSize = 1;
  style.ChildBorderSize = 1;
  style.PopupBorderSize = 1;
  style.FrameBorderSize = 1;
  style.TabBorderSize = 1;
  style.WindowRounding = 7;
  style.ChildRounding = 4;
  style.FrameRounding = 3;
  style.PopupRounding = 4;
  style.ScrollbarRounding = 9;
  style.GrabRounding = 3;
  style.LogSliderDeadzone = 4;
  style.TabRounding = 4;
}

void gui::SetStyle(Style style) {
  gContext->style = static_cast<int>(style);
  switch (style) {
    case Style::CLASSIC:
      ImGui::StyleColorsClassic();
      break;
    case Style::DARK:
      ImGui::StyleColorsDark();
      break;
    case Style::LIGHT:
      ImGui::StyleColorsLight();
      break;
    case Style::DEEP_DARK:
      StyleColorsDeepDark();
      break;
  }
}

void gui::SetFPS(int fps) {
  SetFPSInternal(fps);
}

void gui::SetClearColor(ImVec4 color) {
  gContext->clearColor = color;
}

std::string gui::GetPlatformSaveFileDir() {
#if defined(_MSC_VER)
  const char* env = std::getenv("APPDATA");
  if (env) {
    return env + std::string("/");
  }
#elif defined(__APPLE__)
  const char* env = std::getenv("HOME");
  if (env) {
    return env + std::string("/Library/Preferences/");
  }
#else
  const char* xdg = std::getenv("XDG_CONFIG_HOME");
  const char* env = std::getenv("HOME");
  if (xdg) {
    return xdg + std::string("/");
  } else if (env) {
    return env + std::string("/.config/");
  }
#endif
  return "";
}

void gui::ConfigurePlatformSaveFile(const std::string& name) {
  gContext->iniPath = GetPlatformSaveFileDir() + name;
}

void gui::EmitViewMenu() {
  if (ImGui::BeginMenu("View")) {
    if (ImGui::BeginMenu("Style")) {
      bool selected;
      selected = gContext->style == static_cast<int>(Style::CLASSIC);
      if (ImGui::MenuItem("Classic", nullptr, &selected, true)) {
        SetStyle(Style::CLASSIC);
      }
      selected = gContext->style == static_cast<int>(Style::DARK);
      if (ImGui::MenuItem("Dark", nullptr, &selected, true)) {
        SetStyle(Style::DARK);
      }
      selected = gContext->style == static_cast<int>(Style::LIGHT);
      if (ImGui::MenuItem("Light", nullptr, &selected, true)) {
        SetStyle(Style::LIGHT);
      }
      selected = gContext->style == static_cast<int>(Style::DEEP_DARK);
      if (ImGui::MenuItem("Deep Dark", nullptr, &selected, true)) {
        SetStyle(Style::DEEP_DARK);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Font")) {
      for (auto&& makeFont : gContext->makeFonts) {
        auto& name = makeFont.GetName();
        bool selected = gContext->defaultFontName == name;
        if (ImGui::MenuItem(name.c_str(), nullptr, &selected)) {
          ImGui::GetIO().FontDefault = makeFont.GetFont();
          gContext->defaultFontName = name;
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Zoom")) {
      for (int i = 50; i <= 200; i += 25) {
        char label[20];
        std::snprintf(label, sizeof(label), "%d%%", i);
        bool selected = gContext->userScale == i;
        if (ImGui::MenuItem(label, nullptr, &selected)) {
          gContext->userScale = i;
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Frame Rate")) {
      bool selected;
      selected = gContext->fps == 0;
      if (ImGui::MenuItem("vsync", nullptr, &selected)) {
        SetFPSInternal(0);
      }
      selected = gContext->fps == 30;
      if (ImGui::MenuItem("30 fps", nullptr, &selected)) {
        SetFPSInternal(30);
      }
      selected = gContext->fps == 60;
      if (ImGui::MenuItem("60 fps", nullptr, &selected)) {
        SetFPSInternal(60);
      }
      selected = gContext->fps == 120;
      if (ImGui::MenuItem("120 fps", nullptr, &selected)) {
        SetFPSInternal(120);
      }
      ImGui::EndMenu();
    }

    if (!gContext->saveSettings) {
      ImGui::MenuItem("Reset UI on Exit?", nullptr, &gContext->resetOnExit);
    }
    ImGui::EndMenu();
  }
}

bool gui::UpdateTextureFromImage(ImTextureID* texture, int width, int height,
                                 const unsigned char* data, int len) {
  // Load from memory
  int width2 = 0;
  int height2 = 0;
  unsigned char* imgData =
      stbi_load_from_memory(data, len, &width2, &height2, nullptr, 4);
  if (!imgData) {
    return false;
  }

  if (width2 == width && height2 == height) {
    UpdateTexture(*texture, PixelFormat::RGBA, width2, height2, imgData);
  } else {
    *texture = CreateTexture(PixelFormat::RGBA, width2, height2, imgData);
  }

  stbi_image_free(imgData);

  return true;
}

bool gui::CreateTextureFromFile(const char* filename, ImTextureID* out_texture,
                                int* out_width, int* out_height) {
  // Load from file
  int width = 0;
  int height = 0;
  unsigned char* data = stbi_load(filename, &width, &height, nullptr, 4);
  if (!data) {
    return false;
  }

  *out_texture = CreateTexture(PixelFormat::RGBA, width, height, data);
  if (out_width) {
    *out_width = width;
  }
  if (out_height) {
    *out_height = height;
  }

  stbi_image_free(data);

  return true;
}

bool gui::CreateTextureFromImage(const unsigned char* data, int len,
                                 ImTextureID* out_texture, int* out_width,
                                 int* out_height) {
  // Load from memory
  int width = 0;
  int height = 0;
  unsigned char* imgData =
      stbi_load_from_memory(data, len, &width, &height, nullptr, 4);
  if (!imgData) {
    return false;
  }

  *out_texture = CreateTexture(PixelFormat::RGBA, width, height, imgData);
  if (out_width) {
    *out_width = width;
  }
  if (out_height) {
    *out_height = height;
  }

  stbi_image_free(imgData);

  return true;
}

void gui::MaxFit(ImVec2* min, ImVec2* max, float width, float height) {
  float destWidth = max->x - min->x;
  float destHeight = max->y - min->y;
  if (width == 0 || height == 0) {
    return;
  }
  if (destWidth * height > destHeight * width) {
    float outputWidth = width * destHeight / height;
    min->x += (destWidth - outputWidth) / 2;
    max->x -= (destWidth - outputWidth) / 2;
  } else {
    float outputHeight = height * destWidth / width;
    min->y += (destHeight - outputHeight) / 2;
    max->y -= (destHeight - outputHeight) / 2;
  }
}

}  // namespace wpi
