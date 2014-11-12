// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_SysWatchdog_h__
#define __nFRC_2015_1_0_A_SysWatchdog_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tSysWatchdog
{
public:
   tSysWatchdog(){}
   virtual ~tSysWatchdog(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tSysWatchdog* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned SystemActive : 1;
         unsigned PowerAlive : 1;
         unsigned SysDisableCount : 15;
         unsigned PowerDisableCount : 15;
#else
         unsigned PowerDisableCount : 15;
         unsigned SysDisableCount : 15;
         unsigned PowerAlive : 1;
         unsigned SystemActive : 1;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tStatus;



   typedef enum
   {
   } tStatus_IfaceConstants;

   virtual tStatus readStatus(tRioStatusCode *status) = 0;
   virtual bool readStatus_SystemActive(tRioStatusCode *status) = 0;
   virtual bool readStatus_PowerAlive(tRioStatusCode *status) = 0;
   virtual unsigned short readStatus_SysDisableCount(tRioStatusCode *status) = 0;
   virtual unsigned short readStatus_PowerDisableCount(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tCommand_IfaceConstants;

   virtual void writeCommand(unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readCommand(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tChallenge_IfaceConstants;

   virtual unsigned char readChallenge(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tActive_IfaceConstants;

   virtual void writeActive(bool value, tRioStatusCode *status) = 0;
   virtual bool readActive(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tTimer_IfaceConstants;

   virtual unsigned int readTimer(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tForcedKills_IfaceConstants;

   virtual unsigned short readForcedKills(tRioStatusCode *status) = 0;




private:
   tSysWatchdog(const tSysWatchdog&);
   void operator=(const tSysWatchdog&);
};

}
}

#endif // __nFRC_2015_1_0_A_SysWatchdog_h__
