// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Alarm_h__
#define __nFRC_2015_1_0_A_Alarm_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tAlarm
{
public:
   tAlarm(){}
   virtual ~tAlarm(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAlarm* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
   } tEnable_IfaceConstants;

   virtual void writeEnable(bool value, tRioStatusCode *status) = 0;
   virtual bool readEnable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tTriggerTime_IfaceConstants;

   virtual void writeTriggerTime(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readTriggerTime(tRioStatusCode *status) = 0;




private:
   tAlarm(const tAlarm&);
   void operator=(const tAlarm&);
};

}
}

#endif // __nFRC_2015_1_0_A_Alarm_h__
