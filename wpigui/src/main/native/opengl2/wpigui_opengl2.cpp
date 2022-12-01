// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

#include "wpigui.h"
#include "wpigui_internal.h"

using namespace wpi::gui;

static bool gPlatformValid = false;

namespace wpi {

void gui::PlatformCreateContext() {}

void gui::PlatformDestroyContext() {}

void gui::PlatformGlfwInitHints() {}

void gui::PlatformGlfwWindowHints() {
  // GL 2.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // enable 4xMSAA
  glfwWindowHint(GLFW_SAMPLES, 4);
}

bool gui::PlatformInitRenderer() {
  glfwMakeContextCurrent(gContext->window);

  glfwSwapInterval(1);  // Enable vsync

  // Turn on multisampling
  glEnable(GL_MULTISAMPLE);

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(gContext->window, true);

  ImGui_ImplOpenGL2_Init();

  gPlatformValid = true;
  return true;
}

void gui::PlatformRenderFrame() {
  if (gContext->reloadFonts) {
    ImGui_ImplOpenGL2_DestroyFontsTexture();
    ImGui_ImplOpenGL2_CreateFontsTexture();
    gContext->reloadFonts = false;
  }
  ImGui_ImplOpenGL2_NewFrame();

  CommonRenderFrame();

  int display_w, display_h;
  glfwGetFramebufferSize(gContext->window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(gContext->clearColor.x, gContext->clearColor.y,
               gContext->clearColor.z, gContext->clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(gContext->window);
}

void gui::PlatformShutdown() {
  gPlatformValid = false;
  ImGui_ImplOpenGL2_Shutdown();
}

void gui::PlatformFramebufferSizeChanged(int width, int height) {}

static inline GLenum GLPixelFormat(PixelFormat format) {
  switch (format) {
    case kPixelRGBA:
      return GL_RGBA;
    case kPixelBGRA:
      return GL_BGRA;
    default:
      return GL_RGBA;
  }
}

ImTextureID gui::CreateTexture(PixelFormat format, int width, int height,
                               const unsigned char* data) {
  if (!gPlatformValid) {
    return nullptr;
  }

  // Create a OpenGL texture identifier
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               GLPixelFormat(format), GL_UNSIGNED_BYTE, data);

  return reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(texture));
}

void gui::UpdateTexture(ImTextureID texture, PixelFormat format, int width,
                        int height, const unsigned char* data) {
  GLuint glTexture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
  if (glTexture == 0) {
    return;
  }
  glBindTexture(GL_TEXTURE_2D, glTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GLPixelFormat(format),
                  GL_UNSIGNED_BYTE, data);
}

void gui::DeleteTexture(ImTextureID texture) {
  if (!gPlatformValid) {
    return;
  }
  GLuint glTexture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
  if (glTexture != 0) {
    glDeleteTextures(1, &glTexture);
  }
}

}  // namespace wpi
