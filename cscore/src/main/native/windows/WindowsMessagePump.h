#pragma once

#include <thread>
#include <windows.h>
#include <functional>

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

namespace cs {
class WindowsMessagePump {
 public:
  WindowsMessagePump(std::function<void(HWND, UINT, WPARAM, LPARAM)> callback);
  ~WindowsMessagePump();

  HWND hwnd;
  std::function<void(HWND, UINT, WPARAM, LPARAM)> m_callback;

 private:
  void ThreadMain();

  std::thread m_mainThread;

};
}
