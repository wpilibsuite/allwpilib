/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/AxisCamera.h"

#include "WPIErrors.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <Timer.h>
#include <iostream>
#include <sstream>

static const unsigned int kMaxPacketSize = 1536;
static const unsigned int kImageBufferAllocationIncrement = 1000;

static const std::string kWhiteBalanceStrings[] = {
    "auto",         "hold",         "fixed_outdoor1", "fixed_outdoor2",
    "fixed_indoor", "fixed_fluor1", "fixed_fluor2",
};

static const std::string kExposureControlStrings[] = {
    "auto", "hold", "flickerfree50", "flickerfree60",
};

static const std::string kResolutionStrings[] = {
    "640x480", "480x360", "320x240", "240x180", "176x144", "160x120",
};

static const std::string kRotationStrings[] = {
    "0", "180",
};

/**
 * AxisCamera constructor
 * @param cameraHost The host to find the camera at, typically an IP address
 */
AxisCamera::AxisCamera(std::string const &cameraHost)
    : m_cameraHost(cameraHost) {
  m_captureThread = std::thread(&AxisCamera::Capture, this);
}

AxisCamera::~AxisCamera() {
  m_done = true;
  m_captureThread.join();
}

/*
 * Return true if the latest image from the camera has not been retrieved by
 * calling GetImage() yet.
 * @return true if the image has not been retrieved yet.
 */
bool AxisCamera::IsFreshImage() const { return m_freshImage; }

/**
 * Get an image from the camera and store it in the provided image.
 * @param image The imaq image to store the result in. This must be an HSL or
 * RGB image.
 * @return 1 upon success, zero on a failure
 */
int AxisCamera::GetImage(Image *image) {
  if (m_imageData.size() == 0) {
    return 0;
  }

  std::lock_guard<priority_mutex> lock(m_imageDataMutex);

  Priv_ReadJPEGString_C(image, m_imageData.data(), m_imageData.size());

  m_freshImage = false;

  return 1;
}

/**
 * Get an image from the camera and store it in the provided image.
 * @param image The image to store the result in. This must be an HSL or RGB
 * image
 * @return 1 upon success, zero on a failure
 */
int AxisCamera::GetImage(ColorImage *image) {
  return GetImage(image->GetImaqImage());
}

/**
 * Instantiate a new image object and fill it with the latest image from the
 * camera.
 *
 * The returned pointer is owned by the caller and is their responsibility to
 * delete.
 * @return a pointer to an HSLImage object
 */
HSLImage *AxisCamera::GetImage() {
  auto image = new HSLImage();
  GetImage(image);
  return image;
}

/**
 * Copy an image into an existing buffer.
 * This copies an image into an existing buffer rather than creating a new image
 * in memory. That way a new image is only allocated when the image being copied
 * is
 * larger than the destination.
 * This method is called by the PCVideoServer class.
 * @param imageData The destination image.
 * @param numBytes The size of the destination image.
 * @return 0 if failed (no source image or no memory), 1 if success.
 */
int AxisCamera::CopyJPEG(char **destImage, unsigned int &destImageSize,
                         unsigned int &destImageBufferSize) {
  std::lock_guard<priority_mutex> lock(m_imageDataMutex);
  if (destImage == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "destImage must not be nullptr");
    return 0;
  }

  if (m_imageData.size() == 0) return 0;  // if no source image

  if (destImageBufferSize <
      m_imageData.size())  // if current destination buffer too small
  {
    if (*destImage != nullptr) delete[] * destImage;
    destImageBufferSize = m_imageData.size() + kImageBufferAllocationIncrement;
    *destImage = new char[destImageBufferSize];
    if (*destImage == nullptr) return 0;
  }
  // copy this image into destination buffer
  if (*destImage == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "*destImage must not be nullptr");
  }

  std::copy(m_imageData.begin(), m_imageData.end(), *destImage);
  destImageSize = m_imageData.size();
  ;
  return 1;
}

/**
 * Request a change in the brightness of the camera images.
 * @param brightness valid values 0 .. 100
 */
void AxisCamera::WriteBrightness(int brightness) {
  if (brightness < 0 || brightness > 100) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "Brightness must be from 0 to 100");
    return;
  }

  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_brightness != brightness) {
    m_brightness = brightness;
    m_parametersDirty = true;
  }
}

/**
 * @return The configured brightness of the camera images
 */
int AxisCamera::GetBrightness() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_brightness;
}

/**
 * Request a change in the white balance on the camera.
 * @param whiteBalance Valid values from the <code>WhiteBalance</code> enum.
 */
void AxisCamera::WriteWhiteBalance(AxisCamera::WhiteBalance whiteBalance) {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_whiteBalance != whiteBalance) {
    m_whiteBalance = whiteBalance;
    m_parametersDirty = true;
  }
}

/**
 * @return The configured white balances of the camera images
 */
AxisCamera::WhiteBalance AxisCamera::GetWhiteBalance() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_whiteBalance;
}

/**
 * Request a change in the color level of the camera images.
 * @param colorLevel valid values are 0 .. 100
 */
void AxisCamera::WriteColorLevel(int colorLevel) {
  if (colorLevel < 0 || colorLevel > 100) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "Color level must be from 0 to 100");
    return;
  }

  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_colorLevel != colorLevel) {
    m_colorLevel = colorLevel;
    m_parametersDirty = true;
  }
}

/**
 * @return The configured color level of the camera images
 */
int AxisCamera::GetColorLevel() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_colorLevel;
}

/**
 * Request a change in the camera's exposure mode.
 * @param exposureControl A mode to write in the <code>Exposure</code> enum.
 */
void AxisCamera::WriteExposureControl(
    AxisCamera::ExposureControl exposureControl) {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_exposureControl != exposureControl) {
    m_exposureControl = exposureControl;
    m_parametersDirty = true;
  }
}

/**
 * @return The configured exposure control mode of the camera
 */
AxisCamera::ExposureControl AxisCamera::GetExposureControl() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_exposureControl;
}

/**
 * Request a change in the exposure priority of the camera.
 * @param exposurePriority Valid values are 0, 50, 100.
 * 0 = Prioritize image quality
 * 50 = None
 * 100 = Prioritize frame rate
 */
void AxisCamera::WriteExposurePriority(int exposurePriority) {
  if (exposurePriority != 0 && exposurePriority != 50 &&
      exposurePriority != 100) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "Exposure priority must be from 0, 50, or 100");
    return;
  }

  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_exposurePriority != exposurePriority) {
    m_exposurePriority = exposurePriority;
    m_parametersDirty = true;
  }
}

/**
 * @return The configured exposure priority of the camera
 */
int AxisCamera::GetExposurePriority() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_exposurePriority;
}

/**
 * Write the maximum frames per second that the camera should send
 * Write 0 to send as many as possible.
 * @param maxFPS The number of frames the camera should send in a second,
 * exposure permitting.
 */
void AxisCamera::WriteMaxFPS(int maxFPS) {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_maxFPS != maxFPS) {
    m_maxFPS = maxFPS;
    m_parametersDirty = true;
    m_streamDirty = true;
  }
}

/**
 * @return The configured maximum FPS of the camera
 */
int AxisCamera::GetMaxFPS() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_maxFPS;
}

/**
 * Write resolution value to camera.
 * @param resolution The camera resolution value to write to the camera.
 */
void AxisCamera::WriteResolution(AxisCamera::Resolution resolution) {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_resolution != resolution) {
    m_resolution = resolution;
    m_parametersDirty = true;
    m_streamDirty = true;
  }
}

/**
 * @return The configured resolution of the camera (not necessarily the same
 * resolution as the most recent image, if it was changed recently.)
 */
AxisCamera::Resolution AxisCamera::GetResolution() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_resolution;
}

/**
 * Write the rotation value to the camera.
 * If you mount your camera upside down, use this to adjust the image for you.
 * @param rotation The angle to rotate the camera
 * (<code>AxisCamera::Rotation::k0</code>
 * or <code>AxisCamera::Rotation::k180</code>)
 */
void AxisCamera::WriteRotation(AxisCamera::Rotation rotation) {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_rotation != rotation) {
    m_rotation = rotation;
    m_parametersDirty = true;
    m_streamDirty = true;
  }
}

/**
 * @return The configured rotation mode of the camera
 */
AxisCamera::Rotation AxisCamera::GetRotation() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_rotation;
}

/**
 * Write the compression value to the camera.
 * @param compression Values between 0 and 100.
 */
void AxisCamera::WriteCompression(int compression) {
  if (compression < 0 || compression > 100) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "Compression must be from 0 to 100");
    return;
  }

  std::lock_guard<priority_mutex> lock(m_parametersMutex);

  if (m_compression != compression) {
    m_compression = compression;
    m_parametersDirty = true;
    m_streamDirty = true;
  }
}

/**
 * @return The configured compression level of the camera
 */
int AxisCamera::GetCompression() {
  std::lock_guard<priority_mutex> lock(m_parametersMutex);
  return m_compression;
}

/**
 * Method called in the capture thread to receive images from the camera
 */
void AxisCamera::Capture() {
  int consecutiveErrors = 0;

  // Loop on trying to setup the camera connection. This happens in a background
  // thread so it shouldn't effect the operation of user programs.
  while (!m_done) {
    std::string requestString =
        "GET /mjpg/video.mjpg HTTP/1.1\n"
        "User-Agent: HTTPStreamClient\n"
        "Connection: Keep-Alive\n"
        "Cache-Control: no-cache\n"
        "Authorization: Basic RlJDOkZSQw==\n\n";
    m_captureMutex.lock();
    m_cameraSocket = CreateCameraSocket(requestString, consecutiveErrors > 5);
    if (m_cameraSocket != -1) {
      ReadImagesFromCamera();
      consecutiveErrors = 0;
    } else {
      consecutiveErrors++;
    }
    m_captureMutex.unlock();
    Wait(0.5);
  }
}

/**
 * This function actually reads the images from the camera.
 */
void AxisCamera::ReadImagesFromCamera() {
  char *imgBuffer = nullptr;
  int imgBufferLength = 0;

  // TODO: these recv calls must be non-blocking. Otherwise if the camera
  // fails during a read, the code hangs and never retries when the camera comes
  // back up.

  int counter = 2;
  while (!m_done) {
    char initialReadBuffer[kMaxPacketSize] = "";
    char intermediateBuffer[1];
    char *trailingPtr = initialReadBuffer;
    int trailingCounter = 0;
    while (counter) {
      // TODO: fix me... this cannot be the most efficient way to approach this,
      // reading one byte at a time.
      if (recv(m_cameraSocket, intermediateBuffer, 1, 0) == -1) {
        wpi_setErrnoErrorWithContext("Failed to read image header");
        close(m_cameraSocket);
        return;
      }
      strncat(initialReadBuffer, intermediateBuffer, 1);
      // trailingCounter ensures that we start looking for the 4 byte string
      // after
      // there is at least 4 bytes total. Kind of obscure.
      // look for 2 blank lines (\r\n)
      if (nullptr != strstr(trailingPtr, "\r\n\r\n")) {
        --counter;
      }
      if (++trailingCounter >= 4) {
        trailingPtr++;
      }
    }
    counter = 1;
    char *contentLength = strstr(initialReadBuffer, "Content-Length: ");
    if (contentLength == nullptr) {
      wpi_setWPIErrorWithContext(IncompatibleMode,
                                 "No content-length token found in packet");
      close(m_cameraSocket);
      if (imgBuffer) delete[] imgBuffer;
      return;
    }
    contentLength = contentLength + 16;    // skip past "content length"
    int readLength = atol(contentLength);  // get the image byte count

    // Make sure buffer is large enough
    if (imgBufferLength < readLength) {
      if (imgBuffer) delete[] imgBuffer;
      imgBufferLength = readLength + kImageBufferAllocationIncrement;
      imgBuffer = new char[imgBufferLength];
      if (imgBuffer == nullptr) {
        imgBufferLength = 0;
        continue;
      }
    }

    // Read the image data for "Content-Length" bytes
    int bytesRead = 0;
    int remaining = readLength;
    while (bytesRead < readLength) {
      int bytesThisRecv =
          recv(m_cameraSocket, &imgBuffer[bytesRead], remaining, 0);
      bytesRead += bytesThisRecv;
      remaining -= bytesThisRecv;
    }

    // Update image
    {
      std::lock_guard<priority_mutex> lock(m_imageDataMutex);

      m_imageData.assign(imgBuffer, imgBuffer + imgBufferLength);
      m_freshImage = true;
    }

    if (WriteParameters()) {
      break;
    }
  }

  close(m_cameraSocket);
}

/**
 * Send a request to the camera to set all of the parameters.  This is called
 * in the capture thread between each frame. This strategy avoids making lots
 * of redundant HTTP requests, accounts for failed initial requests, and
 * avoids blocking calls in the main thread unless necessary.
 *
 * This method does nothing if no parameters have been modified since it last
 * completely successfully.
 *
 * @return <code>true</code> if the stream should be restarted due to a
 * parameter changing.
 */
bool AxisCamera::WriteParameters() {
  if (m_parametersDirty) {
    std::stringstream request;
    request << "GET /axis-cgi/admin/param.cgi?action=update";

    m_parametersMutex.lock();
    request << "&ImageSource.I0.Sensor.Brightness=" << m_brightness;
    request << "&ImageSource.I0.Sensor.WhiteBalance="
            << kWhiteBalanceStrings[m_whiteBalance];
    request << "&ImageSource.I0.Sensor.ColorLevel=" << m_colorLevel;
    request << "&ImageSource.I0.Sensor.Exposure="
            << kExposureControlStrings[m_exposureControl];
    request << "&ImageSource.I0.Sensor.ExposurePriority=" << m_exposurePriority;
    request << "&Image.I0.Stream.FPS=" << m_maxFPS;
    request << "&Image.I0.Appearance.Resolution="
            << kResolutionStrings[m_resolution];
    request << "&Image.I0.Appearance.Compression=" << m_compression;
    request << "&Image.I0.Appearance.Rotation=" << kRotationStrings[m_rotation];
    m_parametersMutex.unlock();

    request << " HTTP/1.1" << std::endl;
    request << "User-Agent: HTTPStreamClient" << std::endl;
    request << "Connection: Keep-Alive" << std::endl;
    request << "Cache-Control: no-cache" << std::endl;
    request << "Authorization: Basic RlJDOkZSQw==" << std::endl;
    request << std::endl;

    int socket = CreateCameraSocket(request.str(), false);
    if (socket == -1) {
      wpi_setErrnoErrorWithContext("Error setting camera parameters");
    } else {
      close(socket);
      m_parametersDirty = false;

      if (m_streamDirty) {
        m_streamDirty = false;
        return true;
      }
    }
  }

  return false;
}

/**
 * Create a socket connected to camera
 * Used to create a connection to the camera for both capturing images and
 * setting parameters.
 * @param requestString The initial request string to send upon successful
 * connection.
 * @param setError If true, rais an error if there's a problem creating the
 * connection.
 * This is only enabled after several unsucessful connections, so a single one
 * doesn't
 * cause an error message to be printed if it immediately recovers.
 * @return -1 if failed, socket handle if successful.
 */
int AxisCamera::CreateCameraSocket(std::string const &requestString,
                                   bool setError) {
  struct addrinfo *address = nullptr;
  int camSocket;

  /* create socket */
  if ((camSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    if (setError)
      wpi_setErrnoErrorWithContext("Failed to create the camera socket");
    return -1;
  }

  if (getaddrinfo(m_cameraHost.c_str(), "80", nullptr, &address) == -1) {
    if (setError) {
      wpi_setErrnoErrorWithContext("Failed to create the camera socket");
      close(camSocket);
    }
    return -1;
  }

  /* connect to server */
  if (connect(camSocket, address->ai_addr, address->ai_addrlen) == -1) {
    if (setError)
      wpi_setErrnoErrorWithContext("Failed to connect to the camera");
    freeaddrinfo(address);
    close(camSocket);
    return -1;
  }

  freeaddrinfo(address);

  int sent = send(camSocket, requestString.c_str(), requestString.size(), 0);
  if (sent == -1) {
    if (setError)
      wpi_setErrnoErrorWithContext("Failed to send a request to the camera");
    close(camSocket);
    return -1;
  }

  return camSocket;
}
