
#include "HAL/handles/HandlesInternal.h"
#include <AHRS.h>
#include <VMXIO.h>
#include <VMXCAN.h>
#include <VMXTime.h>
#include <VMXPower.h>
#include <VMXThread.h>
#include <VMXErrors.h>

extern AHRS* vmxIMU;
extern VMXIO* vmxIO;
extern VMXCAN* vmxCAN;
extern VMXTime* vmxTime;
extern VMXPower* vmxPower;
extern VMXThread* vmxThread;

extern VMXErrorCode vmxError;


static VMXResourceHandle HAL_WPIToVMXHandle(HAL_Handle wpiHandle, VMXResourceType vmxType) {
    int16_t index = hal::getHandleIndex(wpiHandle);
    VMXResourceHandle handle = CREATE_VMX_RESOURCE_HANDLE(vmxType,index);
    return handle;
}