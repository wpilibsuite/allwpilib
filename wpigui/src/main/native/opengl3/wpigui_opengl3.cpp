/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdio>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#include "wpigui.h"
#include "wpigui_internal.h"

using namespace wpi::gui;

namespace wpi {

void gui::PlatformCreateContext() {}

void gui::PlatformDestroyContext() {}

void gui::PlatformGlfwInitHints() {}

void gui::PlatformGlfwWindowHints() {
  // Decide GL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
  glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
#else
  // GL 3.0 + GLSL 130
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+
#endif

  // enable 4xMSAA
  glfwWindowHint(GLFW_SAMPLES, 4);
}

bool gui::PlatformInitRenderer() {
  glfwMakeContextCurrent(gContext->window);

  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader
  if (gl3wInit() != 0) {
    std::fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return false;
  }

  // Turn on multisampling
  glEnable(GL_MULTISAMPLE);

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(gContext->window, true);

  // Decide GLSL versions
#if __APPLE__
  const char* glsl_version = "#version 150";
#else
  const char* glsl_version = "#version 130";
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);

  return true;
}

void gui::PlatformRenderFrame() {
  ImGui_ImplOpenGL3_NewFrame();

  CommonRenderFrame();

  int display_w, display_h;
  glfwGetFramebufferSize(gContext->window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(gContext->clearColor.x, gContext->clearColor.y,
               gContext->clearColor.z, gContext->clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(gContext->window);
}

void gui::PlatformShutdown() { ImGui_ImplOpenGL3_Shutdown(); }

bool gui::LoadTextureFromFile(const char* filename, ImTextureID* out_texture,
                              int* out_width, int* out_height) {
  // Load from file
  int width = 0;
  int height = 0;
  unsigned char* data = stbi_load(filename, &width, &height, nullptr, 4);
  if (!data) return false;

  // Create a OpenGL texture identifier
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  *out_texture = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(texture));
  if (out_width) *out_width = width;
  if (out_height) *out_height = height;

  return true;
}

void gui::DeleteTexture(ImTextureID texture) {
  GLuint glTexture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
  if (glTexture != 0) glDeleteTextures(1, &glTexture);
}

}  // namespace wpi
