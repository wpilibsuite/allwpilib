# wpigui

A [Dear Imgui](https://github.com/ocornut/imgui) wrapper that handles platform-specifics like the rendering API and adds some convenience functions. Bootstrap an app with three lines:

```c++
wpi::gui::CreateContext();
wpi::gui::Initialize("Hello World", 1024, 768);
wpi::gui::Main();
```
