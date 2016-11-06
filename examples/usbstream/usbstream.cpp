#include "cscore.h"

#include <stdio.h>

int main() {
  cs::USBCamera camera{"usbcam", 1};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::MJPEGServer mjpegServer{"httpserver", 8080};
  mjpegServer.SetSource(camera);

  getchar();
}
