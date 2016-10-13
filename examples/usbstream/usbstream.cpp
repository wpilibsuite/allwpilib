#include "cameraserver.h"

#include <thread>

int main() {
  cs::USBCamera camera{"usbcam", 1};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::HTTPSink httpServer{"httpserver", 8080};
  httpServer.SetSource(camera);
  for (;;) std::this_thread::sleep_for(std::chrono::seconds(1));
}
