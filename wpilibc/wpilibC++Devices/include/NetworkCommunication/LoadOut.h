
#ifndef __LoadOut_h__
#define __LoadOut_h__

#ifdef SIMULATION
#include <vxWorks_compat.h>
#define EXPORT_FUNC __declspec(dllexport) __cdecl
#elif defined (__vxworks)
#include <vxWorks.h>
#define EXPORT_FUNC
#else
#include <stdint.h>
#define EXPORT_FUNC
#endif

#define kMaxModuleNumber 2
namespace nLoadOut
{
#if defined(__vxworks) || defined(SIMULATION)
    typedef enum {
        kModuleType_Unknown = 0x00,
        kModuleType_Analog = 0x01,
        kModuleType_Digital = 0x02,
        kModuleType_Solenoid = 0x03,
    } tModuleType;
    bool EXPORT_FUNC getModulePresence(tModuleType moduleType, uint8_t moduleNumber);
#endif
    typedef enum {
        kTargetClass_Unknown = 0x00,
        kTargetClass_FRC1 = 0x10,
        kTargetClass_FRC2 = 0x20,
        kTargetClass_FRC3 = 0x30,
        kTargetClass_RoboRIO = 0x40,
#if defined(__vxworks) || defined(SIMULATION)
        kTargetClass_FRC2_Analog = kTargetClass_FRC2 | kModuleType_Analog,
        kTargetClass_FRC2_Digital = kTargetClass_FRC2 | kModuleType_Digital,
        kTargetClass_FRC2_Solenoid = kTargetClass_FRC2 | kModuleType_Solenoid,
#endif
        kTargetClass_FamilyMask = 0xF0,
        kTargetClass_ModuleMask = 0x0F,
    } tTargetClass;
    tTargetClass EXPORT_FUNC getTargetClass();
}

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__vxworks) || defined(SIMULATION)
    uint32_t EXPORT_FUNC FRC_NetworkCommunication_nLoadOut_getModulePresence(uint32_t moduleType, uint8_t moduleNumber);
#endif
    uint32_t EXPORT_FUNC FRC_NetworkCommunication_nLoadOut_getTargetClass();

#ifdef __cplusplus
}
#endif

#endif // __LoadOut_h__
