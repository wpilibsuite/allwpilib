/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#ifndef WPIUTIL_SUPPORT_TIMESTAMP_H_
#define WPIUTIL_SUPPORT_TIMESTAMP_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned long long WPI_Now(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace wpi {

unsigned long long Now();

}  // namespace wpi
#endif

#endif  // WPIUTIL_SUPPORT_TIMESTAMP_H_
