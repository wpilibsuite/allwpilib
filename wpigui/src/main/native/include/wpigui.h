// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>

#include <imgui.h>

#if __has_include(<wpi/StringRef.h>)
#include <wpi/StringRef.h>
#define WPIGUI_HAS_STRINGREF
#endif

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
 * Adds an application icon.  Multiple icons (of different sizes) may be
 * set.  This must be called prior to initialization to have an effect.
 *
 * @param data image data
 * @param len image data length
 * @return False if image data could not be read
 */
bool AddIcon(const unsigned char* data, int len);

#ifdef WPIGUI_HAS_STRINGREF
inline bool AddIcon(wpi::StringRef data) {
  return AddIcon(data.bytes_begin(), data.size());
}
#endif

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
 * Configures a save file (.ini) in a platform specific location. On Windows,
 * the .ini is saved in %APPDATA%; on macOS the .ini is saved in
 * ~/Library/Preferences; on Linux the .ini is stored in $XDG_CONFIG_HOME or
 * ~/.config if the former is not defined. This must be called before
 * gui::Initialize().
 */
void ConfigurePlatformSaveFile(const std::string& name);

/**
 * Emits a View menu (e.g. for a main menu bar) that allows setting of
 * style and zoom.  Internally starts with ImGui::BeginMenu("View").
 */
void EmitViewMenu();

/**
 * Pixel formats for texture pixel data.
 */
enum PixelFormat { kPixelRGBA, kPixelBGRA };

/**
 * Creates a texture from pixel data.
 *
 * @param format pixel format
 * @param width image width
 * @param height image height
 * @param data pixel data
 * @return Texture
 */
ImTextureID CreateTexture(PixelFormat format, int width, int height,
                          const unsigned char* data);

/**
 * Updates a texture from pixel data.
 * The passed-in width and height must match the width and height of the
 * texture.
 *
 * @param texture texture
 * @param format pixel format
 * @param width texture width
 * @param height texture height
 * @param data pixel data
 */
void UpdateTexture(ImTextureID texture, PixelFormat format, int width,
                   int height, const unsigned char* data);

/**
 * Updates a texture from image data.
 * The pixel format of the texture must be RGBA.  The passed-in width and
 * height must match the width and height of the texture.  If the width and
 * height of the image differ from the passed-in width and height, a new
 * texture is created (note this may be inefficient).
 *
 * @param texture texture (pointer, may be updated)
 * @param width texture width
 * @param height texture height
 * @param data image data
 * @param len image data length
 *
 * @return True on success, false on failure.
 */
bool UpdateTextureFromImage(ImTextureID* texture, int width, int height,
                            const unsigned char* data, int len);

/**
 * Creates a texture from an image file.
 *
 * @param filename filename
 * @param out_texture texture (output)
 * @param out_width image width (output)
 * @param out_height image height (output)
 * @return True on success, false on failure.
 */
bool CreateTextureFromFile(const char* filename, ImTextureID* out_texture,
                           int* out_width, int* out_height);

/**
 * Creates a texture from image data.
 *
 * @param data image data
 * @param len image data length
 * @param out_texture texture (output)
 * @param out_width image width (output)
 * @param out_height image height (output)
 * @return True on success, false on failure.
 */
bool CreateTextureFromImage(const unsigned char* data, int len,
                            ImTextureID* out_texture, int* out_width,
                            int* out_height);

/**
 * Deletes a texture.
 *
 * @param texture texture
 */
void DeleteTexture(ImTextureID texture);

/**
 * RAII wrapper around ImTextureID.  Also keeps track of width, height, and
 * pixel format.
 */
class Texture {
 public:
  Texture() = default;

  /**
   * Constructs a texture from pixel data.
   *
   * @param format pixel format
   * @param width image width
   * @param height image height
   * @param data pixel data
   */
  Texture(PixelFormat format, int width, int height, const unsigned char* data)
      : m_format{format}, m_width{width}, m_height{height} {
    m_texture = CreateTexture(format, width, height, data);
  }

  Texture(const Texture&) = delete;
  Texture(Texture&& oth)
      : m_texture{oth.m_texture},
        m_format{oth.m_format},
        m_width{oth.m_width},
        m_height{oth.m_height} {
    oth.m_texture = 0;  // NOLINT
  }

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& oth) {
    if (m_texture) {
      DeleteTexture(m_texture);
    }
    m_texture = oth.m_texture;
    oth.m_texture = 0;  // NOLINT
    m_format = oth.m_format;
    m_width = oth.m_width;
    m_height = oth.m_height;
    return *this;
  }

  ~Texture() {
    if (m_texture) {
      DeleteTexture(m_texture);
    }
  }

  /**
   * Evaluates to true if the texture is valid.
   */
  explicit operator bool() const { return m_texture; }

  /**
   * Implicit conversion to ImTextureID.
   */
  operator ImTextureID() const { return m_texture; }  // NOLINT

  /**
   * Gets the texture pixel format.
   *
   * @return pixel format
   */
  PixelFormat GetFormat() const { return m_format; }

  /**
   * Gets the texture width.
   *
   * @return width
   */
  int GetWidth() const { return m_width; }

  /**
   * Gets the texture height.
   *
   * @return height
   */
  int GetHeight() const { return m_height; }

  /**
   * Updates the texture from pixel data.
   * The image data size and format is assumed to match that of the texture.
   *
   * @param format pixel format
   * @param data pixel data
   */
  void Update(const unsigned char* data) {
    UpdateTexture(m_texture, m_format, m_width, m_height, data);
  }

  /**
   * Updates the texture from image data.
   * The pixel format of the texture must be RGBA.  If the width and height of
   * the image differ from the texture width and height, a new texture is
   * created (note this may be inefficient).
   *
   * @param data image data
   * @param len image data length
   *
   * @return True on success, false on failure.
   */
  bool UpdateFromImage(const unsigned char* data, int len) {
    return UpdateTextureFromImage(&m_texture, m_width, m_height, data, len);
  }

  /**
   * Creates a texture by loading an image file.
   *
   * @param filename filename
   *
   * @return Texture, or invalid (empty) texture on failure.
   */
  static Texture CreateFromFile(const char* filename) {
    Texture texture;
    if (!CreateTextureFromFile(filename, &texture.m_texture, &texture.m_width,
                               &texture.m_height)) {
      return {};
    }
    return texture;
  }

  /**
   * Creates a texture from image data.
   *
   * @param data image data
   * @param len image data length
   *
   * @return Texture, or invalid (empty) texture on failure.
   */
  static Texture CreateFromImage(const unsigned char* data, int len) {
    Texture texture;
    if (!CreateTextureFromImage(data, len, &texture.m_texture, &texture.m_width,
                                &texture.m_height)) {
      return {};
    }
    return texture;
  }

 private:
  ImTextureID m_texture = nullptr;
  PixelFormat m_format = kPixelRGBA;
  int m_width = 0;
  int m_height = 0;
};

/**
 * Get square of distance between two ImVec2's.
 *
 * @param a first ImVec
 * @param b second ImVec
 *
 * @return Distance^2 between a and b.
 */
inline float GetDistSquared(const ImVec2& a, const ImVec2& b) {
  float deltaX = b.x - a.x;
  float deltaY = b.y - a.y;
  return deltaX * deltaX + deltaY * deltaY;
}

/**
 * Maximize fit in "window" while preserving aspect ratio.  Min and max
 * passed-in values are modified such that the object maximally fits.
 *
 * @param min upper left corner of window (modified to fit)
 * @param max lower right corner of window (modified to fit)
 * @param width width of object to fit
 * @param height height of object to fit
 */
void MaxFit(ImVec2* min, ImVec2* max, float width, float height);

}  // namespace wpi::gui
