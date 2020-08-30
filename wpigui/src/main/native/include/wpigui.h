/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <imgui.h>

extern "C" struct GLFWwindow;

namespace wpi::gui {

/**
 * Creates GUI context.  Must be called prior to calling any other functions.
 */
void CreateContext();

/**
 * Destroys GUI context.
 */
void DestroyContext();

/**
 * Initializes the GUI.
 *
 * @param title main application window title
 * @param width main application window width
 * @param height main application window height
 */
bool Initialize(const char* title, int width, int height);

/**
 * Runs main GUI loop.  On some OS'es this must be called from the main thread.
 * Does not return until Exit() is called.
 */
void Main();

/**
 * Exits main GUI loop when current loop iteration finishes.
 * Safe to call from any thread, including from within main GUI loop.
 */
void Exit();

/**
 * Adds initializer to GUI.  The passed function is called once, immediately
 * after the GUI (both GLFW and Dear ImGui) are initialized in Initialize().
 * To have any effect, must be called prior to Initialize().
 *
 * @param initialize initialization function
 */
void AddInit(std::function<void()> initialize);

/**
 * Adds window scaler function.  The passed function is called once during
 * Initialize() if the window scale is not 1.0.  To have any effect, must
 * be called prior to Initialize().
 *
 * @param windowScaler window scaler function
 */
void AddWindowScaler(std::function<void(float scale)> windowScaler);

/**
 * Adds per-frame executor to GUI.  The passed function is called on each
 * Dear ImGui frame prior to any of the late execute functions.
 *
 * @param execute frame execution function
 */
void AddEarlyExecute(std::function<void()> execute);

/**
 * Adds per-frame executor to GUI.  The passed function is called on each
 * Dear ImGui frame after all of the early execute functions.
 *
 * @param execute frame execution function
 */
void AddLateExecute(std::function<void()> execute);

/**
 * Gets GLFW window handle.
 */
GLFWwindow* GetSystemWindow();

/**
 * Adds a font to the GUI.  The passed function is called during
 * initialization as many times as necessary to create a range of sizes.
 *
 * @param name font name
 * @param makeFont font creation / loader function
 * @return Font index for later use with GetFont()
 */
int AddFont(
    const char* name,
    std::function<ImFont*(ImGuiIO& io, float size, const ImFontConfig* cfg)>
        makeFont);

/**
 * Gets a font added with AddFont() with the appropriate font size for
 * the current scaling of the GUI.
 *
 * @param font font index returned by AddFont()
 * @return Font pointer
 */
ImFont* GetFont(int font);

enum Style { kStyleClassic = 0, kStyleDark, kStyleLight };

/**
 * Sets the ImGui style.  Using this function makes this setting persistent.
 *
 * @param style Style
 */
void SetStyle(Style style);

/**
 * Sets the clear (background) color.
 *
 * @param color Color
 */
void SetClearColor(ImVec4 color);

/**
 * Emits a View menu (e.g. for a main menu bar) that allows setting of
 * style and zoom.  Internally starts with ImGui::BeginMenu("View").
 */
void EmitViewMenu();

/**
 * Loads a texture from a file.
 *
 * @param filename filename
 * @param out_texture texture (output)
 * @param out_width image width (output)
 * @param out_height image height (output)
 * @return True on success, false on failure.
 */
bool LoadTextureFromFile(const char* filename, ImTextureID* out_texture,
                         int* out_width, int* out_height);

/**
 * Deletes a texture.
 *
 * @param texture texture
 */
void DeleteTexture(ImTextureID texture);

}  // namespace wpi::gui
