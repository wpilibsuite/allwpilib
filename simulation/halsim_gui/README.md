# halsim_gui — Simulation GUI

Provides the graphical simulation GUI for WPILib robot programs, including
driver station controls, joystick management, and hardware visualization.

## Joystick / Controller Support

The simulation GUI reads physical game controllers and presents them as
**System Joysticks** that can be dragged onto **Robot Joystick** slots.
Controllers are read via GLFW's platform-native HID backend.

### macOS: GCController Fallback

On macOS, Apple's Game Controller framework exclusively claims certain
controllers (notably Xbox Wireless Controllers), preventing GLFW's IOKit
HID backend from reading their input. GLFW detects the device but reports
0 axes and 0 buttons.

To work around this, the sim GUI includes a GCController-based fallback
(`GCGamepadData.mm`) that reads input via Apple's framework when GLFW
cannot. The fallback:

- **Only activates for broken GLFW devices** — controllers that GLFW
  handles correctly (e.g. PS5 DualSense) are unaffected and continue
  to use the GLFW path.
- **Deduplicates automatically** — if both GLFW and GCController see
  the same device, only the working version is shown in the System
  Joysticks list.
- **Maps to FRC standard layout** — axes and buttons are mapped to
  match the FRC Driver Station's XInput-style ordering.

#### Known Limitations

- **Focus required**: GCController stops delivering input updates when
  the sim GUI window does not have focus. A warning is printed to stderr
  when this occurs. Controllers handled by GLFW (e.g. PS5 DualSense)
  are not affected by this limitation.
- **Discovery delay**: GCController takes approximately 1–2 seconds
  after startup to detect connected controllers. During this time, only
  GLFW-compatible controllers appear in the System Joysticks list.
