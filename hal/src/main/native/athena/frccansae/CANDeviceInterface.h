#ifndef __CAN_DEVICE_INTERFACE_H__
#define __CAN_DEVICE_INTERFACE_H__

#define MAX_STRING_LEN 64

#define SUPPORT_UNIQUE_ID	(1) /* depends entirely on old vs new build */
#define USE_NTH_ORDER		(0) /* zero to user deviceId */
#define SUPPORT_MOTOR_CONTROLLER_PROFILE	(1)
namespace CANDeviceInterface1
{

struct PIDSlot
{
	// Proportional gain
	float pGain;
	// Integral gain
	float iGain;
	// Differential gain
	float dGain;
	// Feed-forward gain
	float fGain;
	// Integral zone
	float iZone;
	// Closed-loop ramp rate
	float clRampRate;
};

struct DeviceDescriptor
{
   // The full device ID, including the device number, manufacturer, and device type.
   // The mask of a message the device supports is 0x1FFF003F.
   unsigned int deviceID;
#if SUPPORT_UNIQUE_ID != 0
   // This is the ID that uniquely identifies the device node in the UI.
   // The purpose of this is to be able to track the device across renames or deviceID changes.
   unsigned int uniqueID;
#endif
   // An dynamically assigned ID that will make setting deviceIDs robust,
   //  Never again will you need to isolate a CAN node just to fix it's ID.
   unsigned int dynamicID;
   // User visible name.  This can be customized by the user, but should have a
   // reasonable default.
   char name[MAX_STRING_LEN];
   // This is a user visible model name that should match the can_devices.ini section.
   char model[MAX_STRING_LEN];
   // This is a version number that represents the version of firmware currently
   // installed on the device.
   char currentVersion[MAX_STRING_LEN];
   // Hardware revision.
   char hardwareRev[MAX_STRING_LEN];
   // Bootloader version.  Will not change for the life of the product, but additional
   // field upgrade features could be added in newer hardware.
   char bootloaderRev[MAX_STRING_LEN];
   // Manufacture Date.  Could be a calender date or just the FRC season year.
   // Also helps troubleshooting "old ones" vs "new ones".
   char manufactureDate[MAX_STRING_LEN];
   // General status of the hardware.  For example if the device is in bootloader
   // due to a bad flash UI could emphasize that.
   char softwareStatus[MAX_STRING_LEN];
   // Is the LED currently on?
   bool led;
	// Reserved fields for future use by CTRE.  Not touched by frccansae
   unsigned int dynFlags;
   unsigned int flags; 		/* bitfield */
   unsigned int ptrToString;
   //unsigned int reserved0;
   //unsigned int reserved1;
   //unsigned int reserved2;
#if SUPPORT_MOTOR_CONTROLLER_PROFILE != 0
	// Motor controller properties (ignored if SupportsMotorControllerProperties is false or unset for this model)
	unsigned int brakeMode; // 0=Coast, 1=Brake
	unsigned int limitSwitchFwdMode; // 0=disabled, 1=Normally Closed, 2=Normally Open
	unsigned int limitSwitchRevMode; // 0=disabled, 1=Normally Closed, 2=Normally Open
	// Limit-switch soft limits
	bool bFwdSoftLimitEnable;
	bool bRevSoftLimitEnable;
	float softLimitFwd;
	float softLimitRev;
	// PID constants for slot 0
	struct PIDSlot slot0;
	// PID constants for slot 1
	struct PIDSlot slot1;
#endif
};

#define kLimitSwitchMode_Disabled		(0)
#define kLimitSwitchMode_NormallyClosed	(1)
#define kLimitSwitchMode_NormallyOpen	(2)

// Interface functions that must be implemented by the CAN Firmware Update Library

// Returns the number of devices that will be returned in a call to
// getListOfDevices().  The calling library will use this info to allocate enough
// memory to accept all device info.
int getNumberOfDevices();

// Return info about discovered devices.  The array of structs should be
// populated before returning.  The numDescriptors input describes how many
// elements were allocated to prevent memory corruption.  The number of devices
// populated should be returned from this function as well.
int getListOfDevices(DeviceDescriptor *devices, int numDescriptors);

// When the user requests to update the firmware of a device a thread will be
// spawned and this function is called from that thread.  This function should
// complete the firmware update process before returning.  The image
// contents and size are directly from the file selected by the user.  The
// error message string can be filled with a NULL-terminated message to show the
// user if there was a problem updating firmware.  The error message is only
// displayed if a non-zero value is returned from this function.
int updateFirmware(const DeviceDescriptor *device, const unsigned char *imageContents, unsigned int imageSize, char *errorMessage, int errorMessageMaxSize);

// This function is called periodically from the UI thread while the firmware
// update is in progress.  The percentComplete parameter should the filled in
// with the current progress of the firmware update process to update a progress
// bar in the UI.
void checkUpdateProgress(const DeviceDescriptor *device, int *percentComplete);

// This is called when the user selects a new ID to assign on the bus and
// chooses to save.  The newDeviceID is really just the device number. The
// manufacturer and device type will remain unchanged.  If a problem is detected
// when assigning a new ID, this function should return a non-zero value.
int assignBroadcastDeviceID(unsigned int newDeviceID);
// The device descriptor should be updated with the new device ID.  The name may
// also change in the descriptor and will be updated in the UI immediately.
// Be sure to modify the descriptor first since the refresh from the UI is
// asynchronous.
int assignDeviceID(DeviceDescriptor *device, unsigned int newDeviceID);

// This entry-point will get called when the user chooses to change the value
// of the device's LED.  This will allow the user to identify devices which
// support dynamic addresses or are otherwise unknown.  If this function returns
// a non-zero value, the UI will report an error.
int saveLightLed(const DeviceDescriptor *device, bool newLEDStatus);

// This entry-point will get called when the user chooses to change the alias
// of the device with the device specified.  If this function returns a non-
// zero value, the UI will report an error.  The device descriptor must be
// updated with the new name that was selected. If a different name is saved
// to the descriptor than the user specified, this will require a manual
// refresh by the user.  This is reported as CAR #505139
int saveDeviceName(DeviceDescriptor *device, const char *newName);

// This entry-point will get called when the user changes any of the motor
// controller specific properties. If this function returns a non-zero value,
// the UI will report an error.  The device descriptor may be updated with
// coerced values.
int saveMotorParameters(DeviceDescriptor *device);

// Run some sort of self-test functionality on the device.  This can be anything
// and the results will be displayed to the user.  A non-zero return value
// indicates an error.
int selfTest(const DeviceDescriptor *device, char *detailedResults, int detailedResultsMaxSize);

} /* CANDeviceInterface */

#endif /* __CAN_DEVICE_INTERFACE_H__ */
