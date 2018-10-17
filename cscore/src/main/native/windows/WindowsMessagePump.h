#pragma once

#include <thread>
#include <windows.h>

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
  WindowsMessagePump();
  ~WindowsMessagePump();

 private:
  void ThreadMain();
  std::thread mainThread;
  HWND hwnd;
};

WindowsMessagePump& GetMessagePump();
}
