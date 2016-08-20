/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_C_H_
#define CAMERASERVER_C_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum CS_PropertyType {
  CS_PROP_NONE = 0,
  CS_PROP_BOOLEAN,
  CS_PROP_DOUBLE,
  CS_PROP_STRING,
  CS_PROP_ENUM
};

#ifdef __cplusplus
}
#endif

#endif /* CAMERASERVER_C_H_ */
