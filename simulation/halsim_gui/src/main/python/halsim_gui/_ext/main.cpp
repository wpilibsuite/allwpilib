
#include <semiwrap_init.halsim_gui._ext._halsim_gui_ext.hpp>

#include <functional>

#include <wpi/halsim/gui/HALSimGuiExt.hpp>
#include <wpi/hal/Extensions.h>

std::function<void()> g_gui_exit;

SEMIWRAP_PYBIND11_MODULE(m) {

  initWrapper(m);

  m.def("_kill_on_signal", []() {
    HAL_RegisterExtensionListener(
        nullptr, [](void *, const char *name, void *data) {
          std::string_view name_view{name};
          if (name_view == HALSIMGUI_EXT_GUIEXIT) {
            g_gui_exit = (halsimgui::GuiExitFn)data;
          } else if (name_view == HALSIMGUI_EXT_ADDGUILATEEXECUTE) {
            auto AddGuiLateExecute = (halsimgui::AddGuiLateExecuteFn)data;
            AddGuiLateExecute([] {
              py::gil_scoped_acquire gil;
              if (PyErr_CheckSignals() == -1) {
                
                // If a python signal has been triggered, the GUI needs to exit. It's
                // not safe to throw an exception here on all platforms so we just
                // assume that the only eventual caller of this function is HAL_RunMain,
                // and our wrapper around that function will check if a python error is
                // set and throw from there.
                //
                // Reference: https://github.com/wpilibsuite/allwpilib/issues/8528
                if (g_gui_exit) {
                  g_gui_exit();
                } else {
                  throw py::error_already_set();
                }
              }
            });
          }
        });
  });
}