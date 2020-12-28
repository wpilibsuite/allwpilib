// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <d3d11.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx11.h>

#include "wpigui.h"
#include "wpigui_internal.h"

#pragma comment(lib, "d3d11.lib")

using namespace wpi::gui;

namespace {
struct PlatformContext {
  ID3D11Device* pd3dDevice = nullptr;
  ID3D11DeviceContext* pd3dDeviceContext = nullptr;
  IDXGISwapChain* pSwapChain = nullptr;
  ID3D11RenderTargetView* mainRenderTargetView = nullptr;
};
}  // namespace

static PlatformContext* gPlatformContext;
static bool gPlatformValid = false;

static void CreateRenderTarget() {
  ID3D11Texture2D* pBackBuffer;
  gPlatformContext->pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  gPlatformContext->pd3dDevice->CreateRenderTargetView(
      pBackBuffer, nullptr, &gPlatformContext->mainRenderTargetView);
  pBackBuffer->Release();
}

static bool CreateDeviceD3D(HWND hWnd) {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(
          nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
          featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
          &gPlatformContext->pSwapChain, &gPlatformContext->pd3dDevice,
          &featureLevel, &gPlatformContext->pd3dDeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

static void CleanupRenderTarget() {
  if (gPlatformContext->mainRenderTargetView) {
    gPlatformContext->mainRenderTargetView->Release();
    gPlatformContext->mainRenderTargetView = nullptr;
  }
}

static void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (gPlatformContext->pSwapChain) {
    gPlatformContext->pSwapChain->Release();
    gPlatformContext->pSwapChain = nullptr;
  }
  if (gPlatformContext->pd3dDeviceContext) {
    gPlatformContext->pd3dDeviceContext->Release();
    gPlatformContext->pd3dDeviceContext = nullptr;
  }
  if (gPlatformContext->pd3dDevice) {
    gPlatformContext->pd3dDevice->Release();
    gPlatformContext->pd3dDevice = nullptr;
  }
}

namespace wpi {

void gui::PlatformCreateContext() {
  gPlatformContext = new PlatformContext;
}

void gui::PlatformDestroyContext() {
  CleanupDeviceD3D();
  delete gPlatformContext;
  gPlatformContext = nullptr;
}

void gui::PlatformGlfwInitHints() {}

void gui::PlatformGlfwWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

bool gui::PlatformInitRenderer() {
  // Initialize Direct3D
  if (!CreateDeviceD3D(glfwGetWin32Window(gContext->window))) {
    CleanupDeviceD3D();
    return false;
  }

  ImGui_ImplGlfw_InitForOpenGL(gContext->window, true);
  ImGui_ImplDX11_Init(gPlatformContext->pd3dDevice,
                      gPlatformContext->pd3dDeviceContext);

  gPlatformValid = true;
  return true;
}

void gui::PlatformRenderFrame() {
  ImGui_ImplDX11_NewFrame();

  CommonRenderFrame();

  gPlatformContext->pd3dDeviceContext->OMSetRenderTargets(
      1, &gPlatformContext->mainRenderTargetView, nullptr);
  gPlatformContext->pd3dDeviceContext->ClearRenderTargetView(
      gPlatformContext->mainRenderTargetView,
      reinterpret_cast<float*>(&gContext->clearColor));
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  gPlatformContext->pSwapChain->Present(1, 0);  // Present with vsync
  // gPlatformContext->pSwapChain->Present(0, 0);  // Present without vsync
}

void gui::PlatformShutdown() {
  gPlatformValid = false;
  ImGui_ImplDX11_Shutdown();
}

void gui::PlatformFramebufferSizeChanged(int width, int height) {
  if (gPlatformContext && gPlatformContext->pd3dDevice) {
    CleanupRenderTarget();
    gPlatformContext->pSwapChain->ResizeBuffers(0, width, height,
                                                DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
  }
}

static inline DXGI_FORMAT DXPixelFormat(PixelFormat format) {
  switch (format) {
    case kPixelRGBA:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case kPixelBGRA:
      return DXGI_FORMAT_B8G8R8A8_UNORM;
    default:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
  }
}

ImTextureID gui::CreateTexture(PixelFormat format, int width, int height,
                               const unsigned char* data) {
  if (!gPlatformValid) {
    return nullptr;
  }

  // Create texture
  D3D11_TEXTURE2D_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;

  ID3D11Texture2D* pTexture = nullptr;
  D3D11_SUBRESOURCE_DATA subResource;
  subResource.pSysMem = data;
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  gPlatformContext->pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

  // Create texture view
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  ZeroMemory(&srvDesc, sizeof(srvDesc));
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = desc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  ID3D11ShaderResourceView* srv;
  gPlatformContext->pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc,
                                                         &srv);
  pTexture->Release();

  return srv;
}

void gui::UpdateTexture(ImTextureID texture, PixelFormat, int width, int height,
                        const unsigned char* data) {
  if (!texture) {
    return;
  }

  D3D11_BOX box;
  box.front = 0;
  box.back = 1;
  box.left = 0;
  box.right = width;
  box.top = 0;
  box.bottom = height;

  ID3D11Resource* resource = nullptr;
  static_cast<ID3D11ShaderResourceView*>(texture)->GetResource(&resource);

  if (resource) {
    gPlatformContext->pd3dDeviceContext->UpdateSubresource(
        resource, 0, &box, data, width * 4, width * height * 4);

    resource->Release();
  }
}

void gui::DeleteTexture(ImTextureID texture) {
  if (!gPlatformValid) {
    return;
  }
  if (texture)
    static_cast<ID3D11ShaderResourceView*>(texture)->Release();
}

}  // namespace wpi
