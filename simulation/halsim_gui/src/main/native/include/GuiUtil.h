/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <GL/gl3w.h>
#include <imgui.h>
#include <wpi/Twine.h>

namespace halsimgui {

bool LoadTextureFromFile(const wpi::Twine& filename, GLuint* out_texture,
                         int* out_width, int* out_height);

// get distance^2 between two ImVec's
inline float GetDistSquared(const ImVec2& a, const ImVec2& b) {
  float deltaX = b.x - a.x;
  float deltaY = b.y - a.y;
  return deltaX * deltaX + deltaY * deltaY;
}

// maximize fit while preserving aspect ratio
void MaxFit(ImVec2* min, ImVec2* max, float width, float height);

}  // namespace halsimgui
