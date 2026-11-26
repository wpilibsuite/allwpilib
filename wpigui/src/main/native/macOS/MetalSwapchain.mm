// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <SDL3/SDL.h>

namespace wpi::gui {

void ConfigureMetalSwapchain(SDL_Window* window) {
  auto properties = SDL_GetWindowProperties(window);
  auto nativeWindow = (__bridge NSWindow*)SDL_GetPointerProperty(
      properties, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
  auto metalViewTag = SDL_GetNumberProperty(
      properties, SDL_PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER, 0);
  if (!nativeWindow || metalViewTag == 0) {
    return;
  }

  NSView* view = [nativeWindow.contentView viewWithTag:metalViewTag];
  if (![view.layer isKindOfClass:[CAMetalLayer class]]) {
    return;
  }

  // SDL's frames-in-flight limit does not reduce Core Animation's drawable
  // pool. Use two drawables instead of the default three to reduce latency.
  CAMetalLayer* layer = (CAMetalLayer*)view.layer;
  layer.maximumDrawableCount = 2;
}

}  // namespace wpi::gui
