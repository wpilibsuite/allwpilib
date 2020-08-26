/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_metal.h>
#include <stb_image.h>

#include "wpigui.h"
#include "wpigui_internal.h"

using namespace wpi::gui;

namespace {
struct PlatformContext {
  CAMetalLayer* layer;
  MTLRenderPassDescriptor *renderPassDescriptor;
  id <MTLCommandQueue> commandQueue;
};
}  // namespace

static PlatformContext* gPlatformContext;

namespace wpi {

void gui::PlatformCreateContext() {
  gPlatformContext = new PlatformContext;
}

void gui::PlatformDestroyContext() {
  delete gPlatformContext;
  gPlatformContext = nullptr;
}

void gui::PlatformGlfwInitHints() {}

void gui::PlatformGlfwWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

bool gui::PlatformInitRenderer() {
  id <MTLDevice> device = MTLCreateSystemDefaultDevice();
  gPlatformContext->commandQueue = [device newCommandQueue];

  ImGui_ImplGlfw_InitForOpenGL(gContext->window, true);
  ImGui_ImplMetal_Init(device);

  NSWindow *nswin = glfwGetCocoaWindow(gContext->window);
  gPlatformContext->layer = [CAMetalLayer layer];
  gPlatformContext->layer.device = device;
  gPlatformContext->layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  nswin.contentView.layer = gPlatformContext->layer;
  nswin.contentView.wantsLayer = YES;

  gPlatformContext->renderPassDescriptor = [MTLRenderPassDescriptor new];

  return true;
}

void gui::PlatformRenderFrame() {
  @autoreleasepool
  {
    int width, height;
    glfwGetFramebufferSize(gContext->window, &width, &height);
    gPlatformContext->layer.drawableSize = CGSizeMake(width, height);
    id<CAMetalDrawable> drawable = [gPlatformContext->layer nextDrawable];

    id<MTLCommandBuffer> commandBuffer = [gPlatformContext->commandQueue commandBuffer];
    auto renderPassDescriptor = gPlatformContext->renderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(gContext->clearColor.x, gContext->clearColor.y, gContext->clearColor.z, gContext->clearColor.w);
    renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [renderEncoder pushDebugGroup:@"WPI GUI"];

    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);

    CommonRenderFrame();

    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

    [renderEncoder popDebugGroup];
    [renderEncoder endEncoding];

    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];
  }
}

void gui::PlatformShutdown() {
  ImGui_ImplMetal_Shutdown();
}

bool gui::LoadTextureFromFile(const char* filename, ImTextureID* out_texture,
                              int* out_width, int* out_height) {
  // Load from file
  int width = 0;
  int height = 0;
  unsigned char* data = stbi_load(filename, &width, &height, nullptr, 4);
  if (!data) return false;

  MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
  textureDescriptor.usage = MTLTextureUsageShaderRead;
#if TARGET_OS_OSX
  textureDescriptor.storageMode = MTLStorageModeManaged;
#else
  textureDescriptor.storageMode = MTLStorageModeShared;
#endif
  id <MTLTexture> texture = [gPlatformContext->layer.device newTextureWithDescriptor:textureDescriptor];
  [texture replaceRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0 withBytes:data bytesPerRow:width * 4];

  *out_texture = (__bridge_retained void *)texture;
  *out_width = width;
  *out_height = height;
  stbi_image_free(data);

  return true;
}

void gui::DeleteTexture(ImTextureID texture) {
  if (!texture) return;
  id <MTLTexture> mtlTexture = (__bridge_transfer id <MTLTexture>)texture;
  (void)mtlTexture;
}

}  // namespace wpi
