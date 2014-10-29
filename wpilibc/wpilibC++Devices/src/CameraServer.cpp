/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CameraServer.h"
#include "ErrorBase.h"
#include "WPIErrors.h"
#include "Utility.h"
#include "Timer.h"

#include <iostream>
#include <algorithm>
#include <algorithm>
#include <cstring>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

constexpr uint8_t CameraServer::kMagicNumber[];
CameraServer *CameraServer::s_instance = nullptr;

/**
 * Singleton getter.
 */
CameraServer *CameraServer::GetInstance() {
	if(s_instance == nullptr) {
		s_instance = new CameraServer;
	}

	return s_instance;
}

CameraServer::CameraServer() :
		m_serverThread(&CameraServer::serve, this),
		m_quality(50),
		m_autoCaptureStarted(false) {
}

/**
 * Manually change the image that is served by the MJPEG stream.  This can
 * be called to pass custom annotated images to the dashboard. Note that, for
 * 640x480 video, this method could take between 40 and 50 milliseconds to
 * complete.
 *
 * This shouldn't be called if {@link #StartAutomaticCapture} is called.
 *
 * @param image The IMAQ image to show on the dashboard
 */
void CameraServer::SetImage(Image const *image) {std::unique_lock<std::recursive_mutex> lock(m_imageMutex);

	/* Flatten the IMAQ image to a JPEG */
	uint32_t dataSize;
	uint8_t *data = (uint8_t *)imaqFlatten(image, IMAQ_FLATTEN_IMAGE, IMAQ_COMPRESSION_JPEG, 10 * m_quality, &dataSize);

	/* Find the start of the JPEG data */
	uint8_t *jpegData = data;
	while(jpegData[0] != 0xff || jpegData[1] != 0xd8) {
		jpegData++;
		dataSize--;

		wpi_assert(dataSize >= 2);
	}

	m_imageData.assign(dataSize, '\0');
	std::copy(jpegData, jpegData + dataSize, m_imageData.begin());

	/* Release the data from IMAQ */
	imaqDispose(data);

	m_newImageReady.notify_all();
}

/**
 * Start automatically capturing images to send to the dashboard.
 *
 * You should call this method to just see a camera feed on the dashboard
 * without doing any vision processing on the roboRIO. {@link #SetImage}
 * shouldn't be called after this is called.
 *
 * @param cameraName The name of the camera interface (e.g. "cam1")
 */
void CameraServer::StartAutomaticCapture(char const *cameraName) {
	if(m_autoCaptureStarted) {
		wpi_setWPIErrorWithContext(IncompatibleState, "Automatic capture has already been started");
		return;
	}

	/* Capturing images takes a lot of CPU time, since JPEG compression is
		done in software, so this is done in a new thread. */
	std::thread captureThread([cameraName, this] {
		IMAQdxSession session;
		IMAQdxError error;
		Image *frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);

		error = IMAQdxOpenCamera(cameraName, IMAQdxCameraControlModeController, &session);
		if(error != IMAQdxErrorSuccess) {
			wpi_setImaqErrorWithContext(error, "IMAQdxOpenCamera");
		}

		error = IMAQdxConfigureGrab(session);
		if(error != IMAQdxErrorSuccess) {
			wpi_setImaqErrorWithContext(error, "IMAQdxConfigureGrab");
		}

		error = IMAQdxStartAcquisition(session);
		if(error != IMAQdxErrorSuccess) {
			wpi_setImaqErrorWithContext(error, "IMAQdxStartAcquisition");
		}

		/* In an infinite loop, wait for an image from the camera, then sent
			it to the dashboard. */
		for(;;) {
			error = IMAQdxGrab(session, frame, true, NULL);
			if(error != IMAQdxErrorSuccess) {
				wpi_setImaqErrorWithContext(error, "IMAQdxGrab");
				break;
			}

			SetImage(frame);
		}

		/* If something went wrong, close the session */
		IMAQdxStopAcquisition(session);
		IMAQdxCloseCamera(session);
	});

	captureThread.detach();
}

/**
 * Set the quality of the compressed image sent to the dashboard
 *
 * @param quality The quality of the JPEG image, from 0 to 100
 */
void CameraServer::SetQuality(unsigned int quality) {
	if(quality > 100) {
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "JPEG quality should be from 0 to 100");
		return;
	}

	m_quality = quality;
}

/**
 * Get the quality of the compressed image sent to the dashboard
 *
 * @return The quality, from 0 to 100
 */
unsigned int CameraServer::GetQuality() const {
	return m_quality;
}

/**
 * Run the M-JPEG server.
 *
 * This function listens for a connection from the dashboard in a background
 * thread, then sends back the M-JPEG stream.
 */
void CameraServer::serve() {
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1) {
		wpi_setErrnoError();
	}

	int reuseAddr = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1) {
		wpi_setErrnoError();
	}

	sockaddr_in address, clientAddress;

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(kPort);

	if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
		wpi_setErrnoError();
	}

	if(listen(sock, 10) == -1) {
		wpi_setErrnoError();
	}

	for(;;) {
		socklen_t clientaddresslen = sizeof(clientAddress);
		int conn = accept(sock, (struct sockaddr *)&clientAddress, &clientaddresslen);

		/* Read the request from the dashboard */
		Request req;
		if(read(conn, &req, sizeof req) == -1) {
			wpi_setErrnoError();
			close(conn);
			continue;
		} else {
			req.fps = ntohl(req.fps);
			req.compression = ntohl(req.compression);
			req.size = ntohl(req.size);
		}

		/* Only the "hardware compression" mode is supported from C++ */
		if(req.compression != kHardwareCompression) {
			wpi_setWPIErrorWithContext(IncompatibleState, "Choose \"USB Camera HW\" on the dashboard");
			close(conn);
			continue;
		}

		/* The period that frames are sent is 1/FPS */
		auto period = std::chrono::microseconds(1000000) / req.fps;

		for(;;) {
			auto startTime = std::chrono::steady_clock::now();

			{
				std::unique_lock<std::recursive_mutex> lock(m_imageMutex);

				m_newImageReady.wait(lock);

				/* Send the magic number that indicates the beginning of a new image */
				if(write(conn, kMagicNumber, sizeof kMagicNumber) == -1) {
					break;
				}

				/* Send the size of this image */
				uint32_t size = htonl(m_imageData.size());
				if(write(conn, &size, sizeof size) == -1) {
					m_imageMutex.unlock();
					break;
				}

				/* Send the image data itself */
				if(write(conn, m_imageData.data(), m_imageData.size()) == -1) {
					break;
				}
			}

			/* Sleep long enough to maintain a constant framerate */
			std::this_thread::sleep_until(startTime + period);
		}

		close(conn);
	}

	close(sock);
}
