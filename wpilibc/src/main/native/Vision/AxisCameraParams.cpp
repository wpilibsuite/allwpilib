/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/AxisCameraParams.h"

#include "Vision/AxisCamera.h"
#include <inetLib.h>
#include "pcre.h"
#include <sockLib.h>
#include <string.h>
#include "HAL/cpp/Synchronized.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

#if JAVA_CAMERA_LIB != 1
#include "DriverStation.h"
#endif

static const char *const kRotationChoices[] = {"0", "180"};
static const char *const kResolutionChoices[] = {"640x480", "640x360", "320x240", "160x120"};
static const char *const kExposureControlChoices[] = { "automatic", "hold", "flickerfree50", "flickerfree60" };
static const char *const kWhiteBalanceChoices[] = { "auto", "holdwb", "fixed_outdoor1",
		"fixed_outdoor2", "fixed_indoor", "fixed_fluor1", "fixed_fluor2" };

/**
 * AxisCamera constructor
 */
AxisCameraParams::AxisCameraParams(const char* ipAddress)
	: m_paramTask("paramTask", (FUNCPTR) s_ParamTaskFunction)
	, m_paramChangedSem (NULL)
	, m_socketPossessionSem (NULL)
	, m_brightnessParam (NULL)
	, m_compressionParam (NULL)
	, m_exposurePriorityParam (NULL)
	, m_colorLevelParam (NULL)
	, m_maxFPSParam (NULL)
	, m_rotationParam (NULL)
	, m_resolutionParam (NULL)
	, m_exposureControlParam (NULL)
	, m_whiteBalanceParam (NULL)
{
	if (ipAddress == NULL || strlen(ipAddress) == 0)
	{
#if JAVA_CAMERA_LIB == 1
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "IP Address must be specified");
		return;
#else
		DriverStation *ds = DriverStation::GetInstance();
		ds->WaitForData();
		uint16_t teamNumber = ds->GetTeamNumber();
		char cameraIP[16];
		snprintf(cameraIP, 16, "10.%d.%d.11", teamNumber / 100, teamNumber % 100);
		m_ipAddress = inet_addr(cameraIP);
#endif
	}
	else
	{
		m_ipAddress = inet_addr((char*)ipAddress);
	}

	if (m_ipAddress == (u_long)ERROR)
	{
		wpi_setErrnoError();
		return;
	}

	m_brightnessParam = new IntCameraParameter("ImageSource.I0.Sensor.Brightness=%i",
			"root.ImageSource.I0.Sensor.Brightness=(.*)", false);
	m_parameters.push_back(m_brightnessParam);
	m_colorLevelParam = new IntCameraParameter("ImageSource.I0.Sensor.ColorLevel=%i",
			"root.ImageSource.I0.Sensor.ColorLevel=(.*)", false);
	m_parameters.push_back(m_colorLevelParam);
	m_exposurePriorityParam = new IntCameraParameter("ImageSource.I0.Sensor.exposurePriority=%i",
			"root.ImageSource.I0.Sensor.ExposurePriority=(.*)", false);
	m_parameters.push_back(m_exposurePriorityParam);
	m_compressionParam = new IntCameraParameter("Image.I0.Appearance.Compression=%i",
			"root.Image.I0.Appearance.Compression=(.*)", true);
	m_parameters.push_back(m_compressionParam);
	m_maxFPSParam = new IntCameraParameter("Image.I0.Stream.FPS=%i",
			"root.Image.I0.Stream.FPS=(.*)", false);
	m_parameters.push_back(m_maxFPSParam);
	m_rotationParam = new EnumCameraParameter("Image.I0.Appearance.Rotation=%s",
			"root.Image.I0.Appearance.Rotation=(.*)", true, kRotationChoices, sizeof(kRotationChoices)/sizeof(kRotationChoices[0]));
	m_parameters.push_back(m_rotationParam);
	m_resolutionParam = new EnumCameraParameter("Image.I0.Appearance.Resolution=%s",
			"root.Image.I0.Appearance.Resolution=(.*)", true, kResolutionChoices, sizeof(kResolutionChoices)/sizeof(kResolutionChoices[0]));
	m_parameters.push_back(m_resolutionParam);
	m_exposureControlParam = new EnumCameraParameter("ImageSource.I0.Sensor.Exposure=%s",
			"root.ImageSource.I0.Sensor.Exposure=(.*)", false, kExposureControlChoices, sizeof(kExposureControlChoices)/sizeof(kExposureControlChoices[0]));
	m_parameters.push_back(m_exposureControlParam);
	m_whiteBalanceParam = new EnumCameraParameter("ImageSource.IO.Sensor.WhiteBalance=%s",
			"root.ImageSource.I0.Sensor.WhiteBalance=(.*)", false, kWhiteBalanceChoices, sizeof(kWhiteBalanceChoices)/sizeof(kWhiteBalanceChoices[0]));
	m_parameters.push_back(m_whiteBalanceParam);

	m_paramChangedSem = initializeSemaphore(SEMAPHORE_Q_PRIORITY, SEMAPHORE_EMPTY);
	m_socketPossessionSem = initializeSemaphore(SEMAPHORE_Q_PRIORITY, SEMAPHORE_FULL);

	m_paramTask.Start((int)this);
}

/**
 * Destructor
 */
AxisCameraParams::~AxisCameraParams()
{
	m_paramTask.Stop();

	deleteSemaphore(m_socketPossessionSem);
	deleteSemaphore(m_paramChangedSem);

	delete m_whiteBalanceParam;
	delete m_exposureControlParam;
	delete m_resolutionParam;
	delete m_rotationParam;
	delete m_maxFPSParam;
	delete m_compressionParam;
	delete m_exposurePriorityParam;
	delete m_colorLevelParam;
	delete m_brightnessParam;
}

/**
 * Static function to start the parameter updating task
 */
int AxisCameraParams::s_ParamTaskFunction(AxisCameraParams* thisPtr)
{
	return thisPtr->ParamTaskFunction();
}

/**
 * Main loop of the parameter task.
 * This loop runs continuously checking parameters from the camera for
 * posted changes and updating them if necessary.
 */
// TODO: need to synchronize the actual setting of parameters (the assignment statement)
int AxisCameraParams::ParamTaskFunction()
{
	static bool firstTime = true;

	while (true)
	{
		takeSemaphore(m_socketPossessionSem, SEMAPHORE_WAIT_FOREVER);
		if (firstTime)
		{
			while (ReadCamParams() == 0);
			firstTime = false;
		}
		bool restartRequired = false;

		ParameterVector_t::iterator it = m_parameters.begin();
		ParameterVector_t::iterator end = m_parameters.end();
		for(; it != end; it++)
		{
			bool changed = false;
			char param[150];
			restartRequired |= (*it)->CheckChanged(changed, param);
			if (changed)
			{
				UpdateCamParam(param);
			}
		}
		if (restartRequired)
		{
			RestartCameraTask();
		}
		giveSemaphore(m_socketPossessionSem);
	}
	return 0;
}

/**
 * Write the brightness value to the camera.
 * @param brightness valid values 0 .. 100
 */
void AxisCameraParams::WriteBrightness(int brightness)
{
	m_brightnessParam->SetValue(brightness);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the brightness value.
 * @return Brightness value from the camera.
 */
int AxisCameraParams::GetBrightness()
{
	return m_brightnessParam->GetValue();
}

/**
 * Set the white balance value.
 * @param whiteBalance Valid values from the WhiteBalance_t enum.
 */
void AxisCameraParams::WriteWhiteBalance(WhiteBalance_t whiteBalance)
{
	m_whiteBalanceParam->SetValue(whiteBalance);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Retrieve the current white balance parameter.
 * @return The white balance value.
 */
AxisCameraParams::WhiteBalance_t AxisCameraParams::GetWhiteBalance()
{
	return (WhiteBalance_t) m_whiteBalanceParam->GetValue();
}

/**
 * Write the color level to the camera.
 * @param colorLevel valid values are 0 .. 100
 */
void AxisCameraParams::WriteColorLevel(int colorLevel)
{
	m_colorLevelParam->SetValue(colorLevel);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Retrieve the color level from the camera.
 * @Returns the camera color level.
 */
int AxisCameraParams::GetColorLevel()
{
	return m_colorLevelParam->GetValue();
}

/**
 * Write the exposure control value to the camera.
 * @param exposureControl A mode to write in the Exposure_t enum.
 */
void AxisCameraParams::WriteExposureControl(Exposure_t exposureControl)
{
	m_exposureControlParam->SetValue(exposureControl);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the exposure value from the camera.
 * @returns the exposure value from the camera.
 */
AxisCameraParams::Exposure_t AxisCameraParams::GetExposureControl()
{
	return (Exposure_t) m_exposureControlParam->GetValue();
}

/**
 * Write resolution value to camera.
 * @param resolution The camera resolution value to write to the camera.  Use the Resolution_t enum.
 */
void AxisCameraParams::WriteResolution(Resolution_t resolution)
{
	m_resolutionParam->SetValue(resolution);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the resolution value from the camera.
 * @returns resultion value for the camera.
 */
AxisCameraParams::Resolution_t AxisCameraParams::GetResolution()
{
	return (Resolution_t) m_resolutionParam->GetValue();
}

/**
 * Write the exposre priority value to the camera.
 * @param exposurePriority Valid values are 0, 50, 100.
 * 0 = Prioritize image quality
 * 50 = None
 * 100 = Prioritize frame rate
 */
void AxisCameraParams::WriteExposurePriority(int exposurePriority)
{
	m_exposurePriorityParam->SetValue(exposurePriority);
	giveSemaphore(m_paramChangedSem);
}

int AxisCameraParams::GetExposurePriority()
{
	return m_exposurePriorityParam->GetValue();
}

/**
 * Write the rotation value to the camera.
 * If you mount your camera upside down, use this to adjust the image for you.
 * @param rotation The image from the Rotation_t enum in AxisCameraParams (kRotation_0 or kRotation_180)
 */
void AxisCameraParams::WriteRotation(Rotation_t rotation)
{
	m_rotationParam->SetValue(rotation);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the rotation value from the camera.
 * @return The rotation value from the camera (Rotation_t).
 */
AxisCameraParams::Rotation_t AxisCameraParams::GetRotation()
{
	return (Rotation_t) m_rotationParam->GetValue();
}

/**
 * Write the compression value to the camera.
 * @param compression Values between 0 and 100.
 */

void AxisCameraParams::WriteCompression(int compression)
{
	m_compressionParam->SetValue(compression);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the compression value from the camera.
 * @return The cached compression value from the camera.
 */
int AxisCameraParams::GetCompression()
{
	return m_compressionParam->GetValue();
}

/**
 * Write the maximum frames per second that the camera should send
 * Write 0 to send as many as possible.
 * @param maxFPS The number of frames the camera should send in a second, exposure permitting.
 */
void AxisCameraParams::WriteMaxFPS(int maxFPS)
{
	m_maxFPSParam->SetValue(maxFPS);
	giveSemaphore(m_paramChangedSem);
}

/**
 * Get the max number of frames per second that the camera will send
 * @return Maximum frames per second.
 */
int AxisCameraParams::GetMaxFPS()
{
	return m_maxFPSParam->GetValue();
}

/**
 * Update a camera parameter.
 * Write a camera parameter to the camera when it has bene changed.
 * @param param the string to insert into the http request.
 * @returns 0 if it failed, otherwise nonzero.
 */
int AxisCameraParams::UpdateCamParam(const char* param)
{
	const char *requestString =
		"GET /axis-cgi/admin/param.cgi?action=update&%s HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";
	char completedRequest[1024];
	sprintf(completedRequest, requestString, param);
	// Send request
	int camSocket = CreateCameraSocket(completedRequest);
	if (camSocket == ERROR)
	{
		printf("UpdateCamParam failed: %s\n", param);
		return 0;
	}
	close(camSocket);
	return 1;
}

/**
 * Read the full param list from camera, use regular expressions to find the bits we care about
 * assign values to member variables.
 */
int AxisCameraParams::ReadCamParams()
{
	const char * requestString =
		"GET /axis-cgi/admin/param.cgi?action=list HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";

	int camSocket = CreateCameraSocket(requestString);
	if (camSocket == ERROR)
	{
		return 0;
	}
	// Allocate on the heap since it is very large and only needed once
	char *readBuffer = new char[27000];
	int totalRead = 0;
	while (1)
	{
		wpi_assert(totalRead < 26000);
		int bytesRead = recv(camSocket, &readBuffer[totalRead], 1000, 0);
		if (bytesRead == ERROR)
		{
			wpi_setErrnoErrorWithContext("Failed to read image header");
			close(camSocket);
			return 0;
		}
		else if (bytesRead <= 0)
		{
			break;
		}
		totalRead += bytesRead;
	}
	readBuffer[totalRead] = '\0';

	ParameterVector_t::iterator it = m_parameters.begin();
	ParameterVector_t::iterator end = m_parameters.end();
	for(; it != end; it++)
	{
		(*it)->GetParamFromString(readBuffer, totalRead);
	}
	close(camSocket);
	delete [] readBuffer;
	return 1;
}

/*
 * Create a socket connected to camera
 * Used to create a connection to the camera by both AxisCameraParams and AxisCamera.
 * @param requestString The initial request string to send upon successful connection.
 * @return ERROR if failed, socket handle if successful.
 */
int AxisCameraParams::CreateCameraSocket(const char *requestString)
{
	int sockAddrSize;
	struct sockaddr_in serverAddr;
	int camSocket;
	/* create socket */
	if ((camSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		wpi_setErrnoErrorWithContext("Failed to create the camera socket");
		return ERROR;
	}

	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(80);

	serverAddr.sin_addr.s_addr = m_ipAddress;

	/* connect to server */
	struct timeval connectTimeout;
	connectTimeout.tv_sec = 5;
	connectTimeout.tv_usec = 0;
	if (connectWithTimeout(camSocket, (struct sockaddr *) &serverAddr, sockAddrSize, &connectTimeout) == ERROR)
	{
		wpi_setErrnoErrorWithContext("Failed to connect to the camera");
		close(camSocket);
		return ERROR;
	}
	int sent = send(camSocket, requestString, strlen(requestString), 0);
	if (sent == ERROR)
	{
		wpi_setErrnoErrorWithContext("Failed to send a request to the camera");
		close(camSocket);
		return ERROR;
	}
	return camSocket;
}
