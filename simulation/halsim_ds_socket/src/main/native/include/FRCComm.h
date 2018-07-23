/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  The defines and enums in this file were copied from:
 *     ni-libraries/include/FRC_NetworkCommunication/FRCComm.h
 *  to avoid the complexities of trying to get gradle to
 *  reliably build against it.
 *----------------------------------------------------------------------------*/

#ifndef WPILIB_SIMULATION_HALSIM_DS_SOCKET_SRC_MAIN_NATIVE_INCLUDE_FRCCOMM_H_
#define WPILIB_SIMULATION_HALSIM_DS_SOCKET_SRC_MAIN_NATIVE_INCLUDE_FRCCOMM_H_

#ifdef _WIN32
#include <windows.h>
#elif defined(__vxworks)
#include <vxWorks.h>
#elif defined(__linux)
#include <stdint.h>
#endif

#define ERR_FRCSystem_NetCommNotResponding -44049
#define ERR_FRCSystem_NoDSConnection -44018

enum AllianceStationID_t {
  kAllianceStationID_red1,
  kAllianceStationID_red2,
  kAllianceStationID_red3,
  kAllianceStationID_blue1,
  kAllianceStationID_blue2,
  kAllianceStationID_blue3,
};

enum MatchType_t {
  kMatchType_none,
  kMatchType_practice,
  kMatchType_qualification,
  kMatchType_elimination,
};

struct ControlWord_t {
#ifndef __vxworks
  uint32_t enabled : 1;
  uint32_t autonomous : 1;
  uint32_t test : 1;
  uint32_t eStop : 1;
  uint32_t fmsAttached : 1;
  uint32_t dsAttached : 1;
  uint32_t control_reserved : 26;
#else
  uint32_t control_reserved : 26;
  uint32_t dsAttached : 1;
  uint32_t fmsAttached : 1;
  uint32_t eStop : 1;
  uint32_t test : 1;
  uint32_t autonomous : 1;
  uint32_t enabled : 1;
#endif
};

struct JoystickAxes_t {
  uint16_t count;
  int16_t axes[1];
};

struct JoystickPOV_t {
  uint16_t count;
  int16_t povs[1];
};
#endif  // WPILIB_SIMULATION_HALSIM_DS_SOCKET_SRC_MAIN_NATIVE_INCLUDE_FRCCOMM_H_
