/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "nivision.h"
#include "NIIMAQdx.h"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

/**
 * Class that runs a TCP server that serves an M-JPEG stream to the dashboard.
 */
class CameraServer : public ErrorBase {
	static constexpr uint16_t kPort = 1180;
	static constexpr uint8_t kMagicNumber[] = { 0x01, 0x00, 0x00, 0x00 };
	static constexpr uint32_t kSize640x480 = 0;
	static constexpr uint32_t kSize320x240 = 1;
	static constexpr uint32_t kSize160x120 = 2;
	static constexpr int32_t kHardwareCompression = -1;
	static constexpr char const *kDefaultCameraName = "cam0";

public:
	static CameraServer *GetInstance();

	void SetImage(Image const *image);

	void StartAutomaticCapture(char const *cameraName = kDefaultCameraName);

	void SetQuality(unsigned int quality);
	unsigned int GetQuality() const;

protected:
	CameraServer();
	void serve();

	std::thread m_serverThread;
	std::recursive_mutex m_imageMutex;
	std::condition_variable_any m_newImageReady;
	std::vector<uint8_t> m_imageData;
	unsigned int m_quality;
	bool m_autoCaptureStarted;

	struct Request {
		uint32_t fps;
		int32_t compression;
		uint32_t size;
	};

	static CameraServer *s_instance;
};

