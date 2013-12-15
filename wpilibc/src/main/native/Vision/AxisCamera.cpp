/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/AxisCamera.h"

#include <string.h>
#include "NetworkCommunication/UsageReporting.h"
#include "HAL/cpp/Synchronized.h"
#include "Vision/PCVideoServer.h"
#include "WPIErrors.h"

/** Private NI function to decode JPEG */
IMAQ_FUNC int Priv_ReadJPEGString_C(Image* _image, const unsigned char* _string, uint32_t _stringLength);

// Max packet without jumbo frames is 1500... add 36 because??
#define kMaxPacketSize 1536
#define kImageBufferAllocationIncrement 1000

AxisCamera *AxisCamera::_instance = NULL;

/**
 * AxisCamera constructor
 */
AxisCamera::AxisCamera(const char *ipAddress)
	: AxisCameraParams(ipAddress)
	, m_cameraSocket(ERROR)
	, m_protectedImageBuffer(NULL)
	, m_protectedImageBufferLength(0)
	, m_protectedImageSize(0)
	, m_protectedImageSem(NULL)
	, m_freshImage(false)
	, m_imageStreamTask("cameraTask", (FUNCPTR)s_ImageStreamTaskFunction)
	, m_videoServer(NULL)
{
	m_protectedImageSem = initializeMutex(SEMAPHORE_Q_PRIORITY | SEMAPHORE_INVERSION_SAFE | SEMAPHORE_DELETE_SAFE);

#if JAVA_CAMERA_LIB != 1
	nUsageReporting::report(nUsageReporting::kResourceType_AxisCamera, ipAddress == NULL ? 1 : 2);
#endif

	if (!StatusIsFatal())
		m_imageStreamTask.Start((int)this); 
}

/**
 * Destructor
 */
AxisCamera::~AxisCamera()
{
	delete m_videoServer;
	m_videoServer = NULL;

	m_imageStreamTask.Stop();
	close(m_cameraSocket);

	SemSet_t::iterator it = m_newImageSemSet.begin();
	SemSet_t::iterator end = m_newImageSemSet.end();
	for (;it != end; it++)
	{
		deleteSemaphore(*it);
	}
	m_newImageSemSet.clear();

	deleteMutex(m_protectedImageSem);
}

/**
 * Get a pointer to the AxisCamera object, if the object does not exist, create it
 * To use the camera on port 2 of a cRIO-FRC, pass "192.168.0.90" to the first GetInstance call.
 * @return reference to AxisCamera object
 */
AxisCamera &AxisCamera::GetInstance(const char *cameraIP)
{
	if (NULL == _instance)
	{
		_instance = new AxisCamera(cameraIP);

		_instance->m_videoServer = new PCVideoServer();
	}

	return *_instance;
}

/**
 * Called by Java to delete the camera... how thoughtful
 */
void AxisCamera::DeleteInstance()
{
	delete _instance;
	_instance = NULL;
}

/**
 * Return true if the latest image from the camera has not been retrieved by calling GetImage() yet.
 * @return true if the image has not been retrieved yet.
 */
bool AxisCamera::IsFreshImage()
{
	return m_freshImage;
}

/**
 * Get the semaphore to be used to synchronize image access with camera acquisition
 *
 * Call semTake on the returned semaphore to block until a new image is acquired.
 *
 * The semaphore is owned by the AxisCamera class and will be deleted when the class is destroyed.
 * @return A semaphore to notify when new image is received
 */
SEMAPHORE_ID AxisCamera::GetNewImageSem()
{
	SEMAPHORE_ID sem = initializeSemaphore(SEMAPHORE_Q_PRIORITY, SEMAPHORE_EMPTY);
	m_newImageSemSet.insert(sem);
	return sem;
}

/**
 * Get an image from the camera and store it in the provided image.
 * @param image The imaq image to store the result in. This must be an HSL or RGB image
 * This function is called by Java.
 * @return 1 upon success, zero on a failure
 */
int AxisCamera::GetImage(Image* imaqImage)
{
	if (m_protectedImageBuffer == NULL)
		return 0;
	Synchronized sync(m_protectedImageSem);
	Priv_ReadJPEGString_C(imaqImage,
		(unsigned char*)m_protectedImageBuffer, m_protectedImageSize);
	m_freshImage = false;
	return 1;
}

#if JAVA_CAMERA_LIB != 1
/**
 * Get an image from the camera and store it in the provided image.
 * @param image The image to store the result in. This must be an HSL or RGB image
 * @return 1 upon success, zero on a failure
 */
int AxisCamera::GetImage(ColorImage* image)
{
	return GetImage(image->GetImaqImage());
}

/**
 * Instantiate a new image object and fill it with the latest image from the camera.
 *
 * The returned pointer is owned by the caller and is their responsibility to delete.
 * @return a pointer to an HSLImage object
 */
HSLImage* AxisCamera::GetImage()
{
	HSLImage *image = new HSLImage();
	GetImage(image);
	return image;
}
#endif

/**
 * Copy an image into an existing buffer.
 * This copies an image into an existing buffer rather than creating a new image
 * in memory. That way a new image is only allocated when the image being copied is
 * larger than the destination.
 * This method is called by the PCVideoServer class.
 * @param imageData The destination image.
 * @param numBytes The size of the destination image.
 * @return 0 if failed (no source image or no memory), 1 if success.
 */
int AxisCamera::CopyJPEG(char **destImage, int &destImageSize, int &destImageBufferSize)
{
	Synchronized sync(m_protectedImageSem);
	if (destImage == NULL)
		wpi_setWPIErrorWithContext(NullParameter, "destImage must not be NULL");

	if (m_protectedImageBuffer == NULL || m_protectedImageSize <= 0)
		return 0; // if no source image

	if (destImageBufferSize < m_protectedImageSize) // if current destination buffer too small
	{
		if (*destImage != NULL) delete [] *destImage;
		destImageBufferSize = m_protectedImageSize + kImageBufferAllocationIncrement;
		*destImage = new char[destImageBufferSize];
		if (*destImage == NULL) return 0;
	}
	// copy this image into destination buffer
	if (*destImage == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "*destImage must not be NULL");
	}
	// TODO: Is this copy realy necessary... perhaps we can simply transmit while holding the protected buffer
	memcpy(*destImage, m_protectedImageBuffer, m_protectedImageSize);
	destImageSize = m_protectedImageSize;
	return 1;
}

/**
 * Static interface that will cause an instantiation if necessary.
 * This static stub is directly spawned as a task to read images from the camera.
 */
int AxisCamera::s_ImageStreamTaskFunction(AxisCamera *thisPtr)
{
	return thisPtr->ImageStreamTaskFunction();
}

/**
 * Task spawned by AxisCamera constructor to receive images from cam
 * If setNewImageSem has been called, this function does a semGive on each new image
 * Images can be accessed by calling getImage()
 */
int AxisCamera::ImageStreamTaskFunction()
{
	// Loop on trying to setup the camera connection. This happens in a background
	// thread so it shouldn't effect the operation of user programs.
	while (1)
	{
		const char *requestString = "GET /mjpg/video.mjpg HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";
		takeSemaphore(m_socketPossessionSem, SEMAPHORE_WAIT_FOREVER);
		m_cameraSocket = CreateCameraSocket(requestString);
		if (m_cameraSocket == ERROR)
		{
			// Don't hammer the camera if it isn't ready.
			giveSemaphore(m_socketPossessionSem);
			delayTicks(1000);
		}
		else
		{
			ReadImagesFromCamera();
		}
	}
	return 0;
}

/**
 * This function actually reads the images from the camera.
 */
int AxisCamera::ReadImagesFromCamera()
{
	char *imgBuffer = NULL;
	int imgBufferLength = 0;
	//Infinite loop, task deletion handled by taskDeleteHook
	// Socket cleanup handled by destructor

	// TODO: these recv calls must be non-blocking. Otherwise if the camera
	// fails during a read, the code hangs and never retries when the camera comes
	// back up.

	int counter = 2;
	while (1)
	{
		char initialReadBuffer[kMaxPacketSize] = "";
		char intermediateBuffer[1];
		char *trailingPtr = initialReadBuffer;
		int trailingCounter = 0;
		while (counter)
		{
			// TODO: fix me... this cannot be the most efficient way to approach this, reading one byte at a time.
			if(recv(m_cameraSocket, intermediateBuffer, 1, 0) == ERROR)
			{
				wpi_setErrnoErrorWithContext("Failed to read image header");
				close (m_cameraSocket);
				return ERROR;
			}
			strncat(initialReadBuffer, intermediateBuffer, 1);
			// trailingCounter ensures that we start looking for the 4 byte string after
			// there is at least 4 bytes total. Kind of obscure.
			// look for 2 blank lines (\r\n)
			if (NULL != strstr(trailingPtr, "\r\n\r\n"))
			{
				--counter;
			}
			if (++trailingCounter >= 4)
			{
				trailingPtr++;
			}
		}
		counter = 1;
		char *contentLength = strstr(initialReadBuffer, "Content-Length: ");
		if (contentLength == NULL)
		{
			wpi_setWPIErrorWithContext(IncompatibleMode, "No content-length token found in packet");
			close(m_cameraSocket);
			return ERROR;
		}
		contentLength = contentLength + 16; // skip past "content length"
		int readLength = atol(contentLength); // get the image byte count

		// Make sure buffer is large enough
		if (imgBufferLength < readLength)
		{
			if (imgBuffer) delete[] imgBuffer;
			imgBufferLength = readLength + kImageBufferAllocationIncrement;
			imgBuffer = new char[imgBufferLength];
			if (imgBuffer == NULL)
			{
				imgBufferLength = 0;
				continue;
			}
		}

		// Read the image data for "Content-Length" bytes
		int bytesRead = 0;
		int remaining = readLength;
		while(bytesRead < readLength)
		{
			int bytesThisRecv = recv(m_cameraSocket, &imgBuffer[bytesRead], remaining, 0);
			bytesRead += bytesThisRecv;
			remaining -= bytesThisRecv;
		}
		// Update image
		UpdatePublicImageFromCamera(imgBuffer, readLength);
		if (takeSemaphore(m_paramChangedSem, SEMAPHORE_NO_WAIT) == OK)
		{
			// params need to be updated: close the video stream; release the camera.
			close(m_cameraSocket);
			giveSemaphore(m_socketPossessionSem);
			return 0;
		}
	}
}

/**
 * Copy the image from private buffer to shared buffer.
 * @param imgBuffer The buffer containing the image
 * @param bufLength The length of the image
 */
void AxisCamera::UpdatePublicImageFromCamera(char *imgBuffer, int imgSize)
{
	{
		Synchronized sync(m_protectedImageSem);

		// Adjust the buffer size if current destination buffer is too small.
		if (m_protectedImageBufferLength < imgSize)
		{
			if (m_protectedImageBuffer != NULL) delete [] m_protectedImageBuffer;
			m_protectedImageBufferLength = imgSize + kImageBufferAllocationIncrement;
			m_protectedImageBuffer = new char[m_protectedImageBufferLength];
			if (m_protectedImageBuffer == NULL)
			{
				m_protectedImageBufferLength = 0;
				return;
			}
		}

		memcpy(m_protectedImageBuffer, imgBuffer, imgSize);
		m_protectedImageSize = imgSize;
	}

	m_freshImage = true;
	// Notify everyone who is interested.
	SemSet_t::iterator it = m_newImageSemSet.begin();
	SemSet_t::iterator end = m_newImageSemSet.end();
	for (;it != end; it++)
	{
		giveSemaphore(*it);
	}
}

/**
 * Implement the pure virtual interface so that when parameter changes require a restart, the image task can be bounced.
 */
void AxisCamera::RestartCameraTask()
{
	m_imageStreamTask.Stop();
	m_imageStreamTask.Start((int)this);
}

#if JAVA_CAMERA_LIB == 1

// C bindings used by Java
// These need to stay as is or Java has to change

void AxisCameraStart(const char *IPAddress)
{
#ifdef SVN_REV
	if (strlen(SVN_REV))
	{
		printf("JavaCameraLib was compiled from SVN revision %s\n", SVN_REV);
	}
	else
	{
		printf("JavaCameraLib was compiled from a location that is not source controlled.\n");
	}
#else
	printf("JavaCameraLib was compiled without -D'SVN_REV=nnnn'\n");
#endif
	AxisCamera::GetInstance(IPAddress);
}

int AxisCameraGetImage (Image* image)
{
	return AxisCamera::GetInstance().GetImage(image);
}

void AxisCameraWriteBrightness(int brightness)
{
	AxisCamera::GetInstance().WriteBrightness(brightness);
}

int AxisCameraGetBrightness()
{
	return AxisCamera::GetInstance().GetBrightness();
}

void AxisCameraWriteWhiteBalance(AxisCameraParams::WhiteBalance_t whiteBalance)
{
	AxisCamera::GetInstance().WriteWhiteBalance(whiteBalance);
}

AxisCameraParams::WhiteBalance_t AxisCameraGetWhiteBalance()
{
	return AxisCamera::GetInstance().GetWhiteBalance();
}

void AxisCameraWriteColorLevel(int colorLevel)
{
	AxisCamera::GetInstance().WriteColorLevel(colorLevel);
}

int AxisCameraGetColorLevel()
{
	return AxisCamera::GetInstance().GetColorLevel();
}

void AxisCameraWriteExposureControl(AxisCameraParams::Exposure_t exposure)
{
	AxisCamera::GetInstance().WriteExposureControl(exposure);
}

AxisCameraParams::Exposure_t AxisCameraGetExposureControl()
{
	return AxisCamera::GetInstance().GetExposureControl();
}

void AxisCameraWriteExposurePriority(int exposure)
{
	AxisCamera::GetInstance().WriteExposurePriority(exposure);
}

int AxisCameraGetExposurePriority()
{
	return AxisCamera::GetInstance().GetExposurePriority();
}

void AxisCameraWriteMaxFPS(int maxFPS)
{
	AxisCamera::GetInstance().WriteMaxFPS(maxFPS);
}

int AxisCameraGetMaxFPS()
{
	return AxisCamera::GetInstance().GetMaxFPS();
}

void AxisCameraWriteResolution(AxisCameraParams::Resolution_t resolution)
{
	AxisCamera::GetInstance().WriteResolution(resolution);
}

AxisCameraParams::Resolution_t AxisCameraGetResolution()
{
	return AxisCamera::GetInstance().GetResolution();
}

void AxisCameraWriteCompression(int compression)
{
	AxisCamera::GetInstance().WriteCompression(compression);
}

int AxisCameraGetCompression()
{
	return AxisCamera::GetInstance().GetCompression();
}

void AxisCameraWriteRotation(AxisCameraParams::Rotation_t rotation)
{
	AxisCamera::GetInstance().WriteRotation(rotation);
}

AxisCameraParams::Rotation_t AxisCameraGetRotation()
{
	return AxisCamera::GetInstance().GetRotation();
}

void AxisCameraDeleteInstance()
{
	AxisCamera::DeleteInstance();
}

int AxisCameraFreshImage()
{
	return AxisCamera::GetInstance().IsFreshImage();
}

#endif // JAVA_CAMERA_LIB == 1

