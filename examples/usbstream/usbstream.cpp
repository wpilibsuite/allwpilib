#include "cscore.h"

#include <stdio.h>

#include "llvm/raw_ostream.h"

int main() {
  llvm::outs() << "hostname: " << cs::GetHostname() << '\n';
  llvm::outs() << "IPv4 network addresses:\n";
  for (const auto& addr : cs::GetNetworkInterfaces())
    llvm::outs() << "  " << addr << '\n';
  cs::UsbCamera camera{"usbcam", 1};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::MjpegServer mjpegServer{"httpserver", 8081};
  mjpegServer.SetSource(camera);

  getchar();
}
