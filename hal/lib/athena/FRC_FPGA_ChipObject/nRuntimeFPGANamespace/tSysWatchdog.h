// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_SysWatchdog_h__
#define __nFRC_2012_1_6_4_SysWatchdog_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
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




private:
   tSysWatchdog(const tSysWatchdog&);
   void operator=(const tSysWatchdog&);
};

}
}

#endif // __nFRC_2012_1_6_4_SysWatchdog_h__
