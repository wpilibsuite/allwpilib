// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import <GameController/GameController.h>

#include <cstring>
#include <string>
#include <vector>

#include "GCGamepadData.h"

// All functions must be called from the sim GUI main thread (no mutex needed).
// GCGamepad_Refresh() is called once per update cycle to snapshot the current
// GCController state into stable slots that never reorder or shrink.
//
// Input values are read via [capture] snapshot in GCGamepad_GetState() for
// consistent reads each frame.
//
// Known limitation: GCController stops delivering input updates when the
// application does not have focus. GLFW's IOKit HID backend does not have
// this restriction, so controllers that GLFW handles directly (e.g. PS5
// DualSense) continue to work when unfocused.

namespace {
struct ControllerSlot {
  GCController* controller = nil;  // nil when disconnected
  std::string name;
  std::string guid;
  bool connected = false;
};

std::vector<ControllerSlot> g_slots;
}  // namespace

extern "C" {

void GCGamepad_Init(void) {
  [GCController startWirelessControllerDiscoveryWithCompletionHandler:nil];
  GCGamepad_Refresh();
}

void GCGamepad_Refresh(void) {
  // Process pending GCController events through the run loop.
  [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode
                        beforeDate:[NSDate date]];

  NSArray<GCController*>* current = [GCController controllers];

  // Mark all slots as disconnected (keep slots alive for pointer stability)
  for (auto& slot : g_slots) {
    slot.connected = false;
  }

  for (GCController* controller in current) {
    if (controller.extendedGamepad == nil) {
      continue;
    }

    NSString* vendorName = controller.vendorName;
    std::string name =
        vendorName ? [vendorName UTF8String] : "Unknown Controller";
    NSString* category = controller.productCategory;
    NSString* guidSource =
        [NSString stringWithFormat:@"GC_%@_%@", category ?: @"Unknown",
                                   vendorName ?: @"Unknown"];
    std::string guid = [guidSource UTF8String];

    // Find existing slot by pointer identity (still connected)
    int foundIdx = -1;
    for (int i = 0; i < static_cast<int>(g_slots.size()); ++i) {
      if (g_slots[i].controller == controller) {
        foundIdx = i;
        break;
      }
    }

    // If not found by pointer, try matching a disconnected slot by GUID
    // (handles reconnection of the same physical device)
    if (foundIdx < 0) {
      for (int i = 0; i < static_cast<int>(g_slots.size()); ++i) {
        if (!g_slots[i].connected && g_slots[i].guid == guid) {
          foundIdx = i;
          break;
        }
      }
    }

    if (foundIdx >= 0) {
      g_slots[foundIdx].controller = controller;
      g_slots[foundIdx].name = name;
      g_slots[foundIdx].guid = guid;
      g_slots[foundIdx].connected = true;
    } else {
      ControllerSlot slot;
      slot.controller = controller;
      slot.name = name;
      slot.guid = guid;
      slot.connected = true;
      g_slots.push_back(slot);
    }
  }

  // Clear controller pointers for disconnected slots
  for (auto& slot : g_slots) {
    if (!slot.connected) {
      slot.controller = nil;
    }
  }
}

int GCGamepad_GetCount(void) {
  return static_cast<int>(g_slots.size());
}

bool GCGamepad_IsConnected(int index) {
  if (index < 0 || index >= static_cast<int>(g_slots.size())) {
    return false;
  }
  return g_slots[index].connected;
}

const char* GCGamepad_GetName(int index) {
  if (index < 0 || index >= static_cast<int>(g_slots.size())) {
    return nullptr;
  }
  return g_slots[index].name.c_str();
}

const char* GCGamepad_GetGUID(int index) {
  if (index < 0 || index >= static_cast<int>(g_slots.size())) {
    return nullptr;
  }
  return g_slots[index].guid.c_str();
}

bool GCGamepad_GetState(int index, float* axes, int* axisCount,
                        unsigned char* buttons, int* buttonCount,
                        int* hatAngle) {
  if (index < 0 || index >= static_cast<int>(g_slots.size())) {
    return false;
  }

  if (!g_slots[index].connected) {
    return false;
  }

  // Capture a snapshot of the current physical state for a consistent read.
  GCExtendedGamepad* gamepad = g_slots[index].controller.extendedGamepad;
  if (gamepad == nil) {
    return false;
  }
  GCExtendedGamepad* snapshot = [gamepad capture];

  // Axes in FRC standard order: LX, LY, LT(0-1), RT(0-1), RX, RY
  axes[0] = snapshot.leftThumbstick.xAxis.value;
  axes[1] = -snapshot.leftThumbstick.yAxis.value;  // Invert Y (down = positive)
  axes[2] = snapshot.leftTrigger.value;
  axes[3] = snapshot.rightTrigger.value;
  axes[4] = snapshot.rightThumbstick.xAxis.value;
  axes[5] = -snapshot.rightThumbstick.yAxis.value;  // Invert Y
  *axisCount = 6;

  // Buttons in GLFW gamepad standard order
  buttons[0] = snapshot.buttonA.pressed ? 1 : 0;
  buttons[1] = snapshot.buttonB.pressed ? 1 : 0;
  buttons[2] = snapshot.buttonX.pressed ? 1 : 0;
  buttons[3] = snapshot.buttonY.pressed ? 1 : 0;
  buttons[4] = snapshot.leftShoulder.pressed ? 1 : 0;
  buttons[5] = snapshot.rightShoulder.pressed ? 1 : 0;
  buttons[6] = snapshot.buttonOptions.pressed ? 1 : 0;   // Back/Select
  buttons[7] = snapshot.buttonMenu.pressed ? 1 : 0;      // Start
  buttons[8] = snapshot.buttonHome.pressed ? 1 : 0;      // Guide
  buttons[9] = snapshot.leftThumbstickButton.pressed ? 1 : 0;   // L3
  buttons[10] = snapshot.rightThumbstickButton.pressed ? 1 : 0;  // R3
  *buttonCount = 11;

  // D-pad to hat angle
  bool up = snapshot.dpad.up.pressed;
  bool down = snapshot.dpad.down.pressed;
  bool left = snapshot.dpad.left.pressed;
  bool right = snapshot.dpad.right.pressed;

  if (up && right) {
    *hatAngle = 45;
  } else if (right && down) {
    *hatAngle = 135;
  } else if (down && left) {
    *hatAngle = 225;
  } else if (left && up) {
    *hatAngle = 315;
  } else if (up) {
    *hatAngle = 0;
  } else if (right) {
    *hatAngle = 90;
  } else if (down) {
    *hatAngle = 180;
  } else if (left) {
    *hatAngle = 270;
  } else {
    *hatAngle = -1;
  }

  return true;
}

}  // extern "C"
