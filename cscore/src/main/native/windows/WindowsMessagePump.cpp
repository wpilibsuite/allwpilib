#include "WindowsMessagePump.h"

#include <windows.h>
#include <windowsx.h>
#include <mfidl.h>
#include <mfapi.h>

#include <Dbt.h>
#include <ks.h>
#include <ksmedia.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")

namespace cs {

// Storing pointer globally to use from our responce proc
static WindowsMessagePump* messagePumpStore;

void HandleDeviceChange(HWND, WPARAM, LPARAM) {

}

LRESULT CALLBACK pWndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
  switch (uiMsg) {
    case WM_DEVICECHANGE:
      HandleDeviceChange(hwnd, wParam, lParam);
      break;
    HANDLE_MSG(hwnd, WM_CLOSE, [](HWND){ PostQuitMessage(0); });
  }
  return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

WindowsMessagePump::WindowsMessagePump() {
  messagePumpStore = this;
  mainThread = std::thread([=](){ ThreadMain(); });
}

WindowsMessagePump::~WindowsMessagePump() {
  SendMessage(hwnd, WM_CLOSE, NULL, NULL);
  if (mainThread.joinable()) mainThread.join();
}

void WindowsMessagePump::ThreadMain() {
  // Initialize COM
  CoInitializeEx(0, COINIT_MULTITHREADED);
  // Initialize MF
  MFStartup(MF_VERSION);

  HINSTANCE current_instance = (HINSTANCE)GetModuleHandle(NULL);
  static const char* class_name = "DUMMY_CLASS";
  WNDCLASSEX wx = {};
  wx.cbSize = sizeof(WNDCLASSEX);
  wx.lpfnWndProc = pWndProc;        // function which will handle messages
  wx.hInstance = current_instance;
  wx.lpszClassName = class_name;
  if (RegisterClassEx(&wx)) {
    hwnd = CreateWindowEx(0, class_name, "dummy_name", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    // Register for device notifications
    HDEVNOTIFY  g_hdevnotify = NULL;

    DEV_BROADCAST_DEVICEINTERFACE di = { 0 };
    di.dbcc_size = sizeof(di);
    di.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    di.dbcc_classguid = KSCATEGORY_CAPTURE;

    g_hdevnotify = RegisterDeviceNotification(
      hwnd,
      &di,
      DEVICE_NOTIFY_WINDOW_HANDLE
    );


    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    UnregisterDeviceNotification(g_hdevnotify);
    UnregisterClass(class_name, current_instance);

  }

  MFShutdown();
  CoUninitialize();
}

WindowsMessagePump& GetMessagePump() {
  static WindowsMessagePump messagePump;
  return messagePump;
}

}
