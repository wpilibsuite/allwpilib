// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_Watchdog_h__
#define __nFRC_2012_1_6_4_Watchdog_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tWatchdog
{
public:
   tWatchdog(){}
   virtual ~tWatchdog(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tWatchdog* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned SystemActive : 1;
         unsigned Alive : 1;
         unsigned SysDisableCount : 15;
         unsigned DisableCount : 15;
#else
         unsigned DisableCount : 15;
         unsigned SysDisableCount : 15;
         unsigned Alive : 1;
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
   virtual bool readStatus_Alive(tRioStatusCode *status) = 0;
   virtual unsigned short readStatus_SysDisableCount(tRioStatusCode *status) = 0;
   virtual unsigned short readStatus_DisableCount(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tKill_IfaceConstants;

   virtual void strobeKill(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tFeed_IfaceConstants;

   virtual void strobeFeed(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tTimer_IfaceConstants;

   virtual unsigned int readTimer(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tExpiration_IfaceConstants;

   virtual void writeExpiration(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readExpiration(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tImmortal_IfaceConstants;

   virtual void writeImmortal(bool value, tRioStatusCode *status) = 0;
   virtual bool readImmortal(tRioStatusCode *status) = 0;




private:
   tWatchdog(const tWatchdog&);
   void operator=(const tWatchdog&);
};

}
}

#endif // __nFRC_2012_1_6_4_Watchdog_h__
