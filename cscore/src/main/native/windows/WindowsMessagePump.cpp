/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WindowsMessagePump.h"

#include <ks.h>
#include <ksmedia.h>
#include <mfapi.h>
#include <mfidl.h>
#include <windows.h>
#include <windowsx.h>

#include <memory>

#include <Dbt.h>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")

namespace cs {

static LRESULT CALLBACK pWndProc(HWND hwnd, UINT uiMsg, WPARAM wParam,
                                 LPARAM lParam) {
  WindowsMessagePump* pumpContainer;
  // Our "this" parameter is passed only during WM_CREATE
  // If it is create, store in our user parameter
  // Otherwise grab from our user parameter
  if (uiMsg == WM_CREATE) {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    pumpContainer =
        reinterpret_cast<WindowsMessagePump*>(pCreate->lpCreateParams);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pumpContainer);
    SetWindowPos(hwnd, HWND_MESSAGE, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  } else {
    pumpContainer = reinterpret_cast<WindowsMessagePump*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }

  // Run the callback
  bool hasCalledBack = false;
  LRESULT result;

  if (pumpContainer) {
    hasCalledBack = true;
    result = pumpContainer->m_callback(hwnd, uiMsg, wParam, lParam);
  }

  // Handle a close message
  if (uiMsg == WM_CLOSE) {
    return HANDLE_WM_CLOSE(hwnd, 0, 0, [](HWND) { PostQuitMessage(0); });
  }

  // Return message, otherwise return the base handler
  if (hasCalledBack) {
    return result;
  }
  return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

namespace {
struct ClassHolder {
  HINSTANCE current_instance;
  WNDCLASSEX wx;
  const char* class_name = "DUMMY_CLASS";
  ClassHolder() {
    current_instance = (HINSTANCE)GetModuleHandle(NULL);
    wx = {};
    wx.cbSize = sizeof(WNDCLASSEX);
    wx.lpfnWndProc = pWndProc;  // function which will handle messages
    wx.hInstance = current_instance;
    wx.lpszClassName = class_name;
    RegisterClassEx(&wx);
  }
  ~ClassHolder() { UnregisterClass(class_name, current_instance); }
};
}  // namespace

static std::shared_ptr<ClassHolder> GetClassHolder() {
  static std::shared_ptr<ClassHolder> clsHolder =
      std::make_shared<ClassHolder>();
  return clsHolder;
}

WindowsMessagePump::WindowsMessagePump(
    std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> callback) {
  m_callback = callback;
  auto handle = CreateEvent(NULL, true, false, NULL);
  m_mainThread = std::thread([=]() { ThreadMain(handle); });
  auto waitResult = WaitForSingleObject(handle, 1000);
  if (waitResult == WAIT_OBJECT_0) {
    CloseHandle(handle);
  }
}

WindowsMessagePump::~WindowsMessagePump() {
  auto res = SendMessage(hwnd, WM_CLOSE, NULL, NULL);
  if (m_mainThread.joinable()) m_mainThread.join();
}

void WindowsMessagePump::ThreadMain(HANDLE eventHandle) {
  // Initialize COM
  CoInitializeEx(0, COINIT_MULTITHREADED);
  // Initialize MF
  MFStartup(MF_VERSION);

  auto classHolder = GetClassHolder();
  hwnd = CreateWindowEx(0, classHolder->class_name, "dummy_name", 0, 0, 0, 0, 0,
                        HWND_MESSAGE, NULL, NULL, this);

  // Register for device notifications
  HDEVNOTIFY g_hdevnotify = NULL;
  HDEVNOTIFY g_hdevnotify2 = NULL;

  DEV_BROADCAST_DEVICEINTERFACE di = {0};
  di.dbcc_size = sizeof(di);
  di.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  di.dbcc_classguid = KSCATEGORY_CAPTURE;

  g_hdevnotify =
      RegisterDeviceNotification(hwnd, &di, DEVICE_NOTIFY_WINDOW_HANDLE);

  DEV_BROADCAST_DEVICEINTERFACE di2 = {0};
  di2.dbcc_size = sizeof(di2);
  di2.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  di2.dbcc_classguid = KSCATEGORY_VIDEO_CAMERA;

  g_hdevnotify2 =
      RegisterDeviceNotification(hwnd, &di2, DEVICE_NOTIFY_WINDOW_HANDLE);

  SetEvent(eventHandle);

  MSG Msg;
  while (GetMessage(&Msg, NULL, 0, 0) > 0) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
  UnregisterDeviceNotification(g_hdevnotify);
  UnregisterDeviceNotification(g_hdevnotify2);

  MFShutdown();
  CoUninitialize();
}

}  // namespace cs
