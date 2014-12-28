/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once

#include <thread>
#include <string>
#include <mutex>

#include "ErrorBase.h"
#include "Vision/ColorImage.h"
#include "Vision/HSLImage.h"
#include "nivision.h"

/**
 * Axis M1011 network camera
 */
class AxisCamera: public ErrorBase
{
public:
	enum WhiteBalance
	{
		kWhiteBalance_Automatic,
		kWhiteBalance_Hold,
		kWhiteBalance_FixedOutdoor1,
		kWhiteBalance_FixedOutdoor2,
		kWhiteBalance_FixedIndoor,
		kWhiteBalance_FixedFluorescent1,
		kWhiteBalance_FixedFluorescent2
	};

	enum ExposureControl
	{
		kExposureControl_Automatic,
		kExposureControl_Hold,
		kExposureControl_FlickerFree50Hz,
		kExposureControl_FlickerFree60Hz
	};

	enum Resolution
	{
		kResolution_640x480,
		kResolution_480x360,
		kResolution_320x240,
		kResolution_240x180,
		kResolution_176x144,
		kResolution_160x120,
	};

	enum Rotation
	{
		kRotation_0, kRotation_180
	};

	explicit AxisCamera(std::string const& cameraHost);
	virtual ~AxisCamera();

	bool IsFreshImage() const;

	int GetImage(Image *image);
	int GetImage(ColorImage *image);
	HSLImage *GetImage();
	int CopyJPEG(char **destImage, unsigned int &destImageSize, unsigned int &destImageBufferSize);

	void WriteBrightness(int brightness);
	int GetBrightness();

	void WriteWhiteBalance(WhiteBalance whiteBalance);
	WhiteBalance GetWhiteBalance();

	void WriteColorLevel(int colorLevel);
	int GetColorLevel();

	void WriteExposureControl(ExposureControl exposureControl);
	ExposureControl GetExposureControl();

	void WriteExposurePriority(int exposurePriority);
	int GetExposurePriority();

	void WriteMaxFPS(int maxFPS);
	int GetMaxFPS();

	void WriteResolution(Resolution resolution);
	Resolution GetResolution();

	void WriteCompression(int compression);
	int GetCompression();

	void WriteRotation(Rotation rotation);
	Rotation GetRotation();

private:
	std::thread m_captureThread;
	std::string m_cameraHost;
	int m_cameraSocket;
	std::mutex m_captureMutex;

	std::mutex m_imageDataMutex;
	std::vector<uint8_t> m_imageData;
	bool m_freshImage;

	int m_brightness;
	WhiteBalance m_whiteBalance;
	int m_colorLevel;
	ExposureControl m_exposureControl;
	int m_exposurePriority;
	int m_maxFPS;
	Resolution m_resolution;
	int m_compression;
	Rotation m_rotation;
	bool m_parametersDirty;
	bool m_streamDirty;
	std::mutex m_parametersMutex;

	bool m_done;

	void Capture();
	void ReadImagesFromCamera();
	bool WriteParameters();

	int CreateCameraSocket(std::string const& requestString, bool setError);

	DISALLOW_COPY_AND_ASSIGN(AxisCamera);
};
