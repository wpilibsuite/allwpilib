/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <windows.h>

#include <functional>
#include <thread>

namespace cs {
class WindowsMessagePump {
 public:
  WindowsMessagePump(
      std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> callback);
  ~WindowsMessagePump();

  friend LRESULT CALLBACK pWndProc(HWND hwnd, UINT uiMsg, WPARAM wParam,
                                   LPARAM lParam);

  template <typename RetVal = LRESULT, typename FirstParam = WPARAM,
            typename SecondParam = LPARAM>
  RetVal SendWindowMessage(UINT msg, FirstParam wParam, SecondParam lParam) {
    static_assert(sizeof(FirstParam) <= sizeof(WPARAM),
                  "First Parameter Does Not Fit");
    static_assert(sizeof(SecondParam) <= sizeof(LPARAM),
                  "Second Parameter Does Not Fit");
    static_assert(sizeof(RetVal) <= sizeof(LRESULT),
                  "Return Value Does Not Fit");
    WPARAM firstToSend = 0;
    LPARAM secondToSend = 0;
    std::memcpy(&firstToSend, &wParam, sizeof(FirstParam));
    std::memcpy(&secondToSend, &lParam, sizeof(SecondParam));
    LRESULT result = SendMessage(hwnd, msg, firstToSend, secondToSend);
    RetVal toReturn;
    std::memset(&toReturn, 0, sizeof(RetVal));
    std::memcpy(&toReturn, &result, sizeof(RetVal));
    return toReturn;
  }

  template <typename FirstParam = WPARAM, typename SecondParam = LPARAM>
  BOOL PostWindowMessage(UINT msg, FirstParam wParam, SecondParam lParam) {
    static_assert(sizeof(FirstParam) <= sizeof(WPARAM),
                  "First Parameter Does Not Fit");
    static_assert(sizeof(SecondParam) <= sizeof(LPARAM),
                  "Second Parameter Does Not Fit");
    WPARAM firstToSend = 0;
    LPARAM secondToSend = 0;
    std::memcpy(&firstToSend, &wParam, sizeof(FirstParam));
    std::memcpy(&secondToSend, &lParam, sizeof(SecondParam));
    return PostMessage(hwnd, msg, firstToSend, secondToSend);
  }

 private:
  void ThreadMain(HANDLE eventHandle);

  HWND hwnd;
  std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> m_callback;

  std::thread m_mainThread;
};
}  // namespace cs
