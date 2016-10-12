// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_Encoder_h__
#define __nFRC_2012_1_6_4_Encoder_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tEncoder
{
public:
   tEncoder(){}
   virtual ~tEncoder(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tEncoder* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 4,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Direction : 1;
         signed Value : 31;
#else
         signed Value : 31;
         unsigned Direction : 1;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tOutput;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned ASource_Channel : 4;
         unsigned ASource_Module : 1;
         unsigned ASource_AnalogTrigger : 1;
         unsigned BSource_Channel : 4;
         unsigned BSource_Module : 1;
         unsigned BSource_AnalogTrigger : 1;
         unsigned IndexSource_Channel : 4;
         unsigned IndexSource_Module : 1;
         unsigned IndexSource_AnalogTrigger : 1;
         unsigned IndexActiveHigh : 1;
         unsigned Reverse : 1;
         unsigned Enable : 1;
#else
         unsigned Enable : 1;
         unsigned Reverse : 1;
         unsigned IndexActiveHigh : 1;
         unsigned IndexSource_AnalogTrigger : 1;
         unsigned IndexSource_Module : 1;
         unsigned IndexSource_Channel : 4;
         unsigned BSource_AnalogTrigger : 1;
         unsigned BSource_Module : 1;
         unsigned BSource_Channel : 4;
         unsigned ASource_AnalogTrigger : 1;
         unsigned ASource_Module : 1;
         unsigned ASource_Channel : 4;
#endif
      };
      struct{
         unsigned value : 21;
      };
   } tConfig;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Period : 23;
         signed Count : 8;
         unsigned Stalled : 1;
#else
         unsigned Stalled : 1;
         signed Count : 8;
         unsigned Period : 23;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tTimerOutput;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned StallPeriod : 24;
         unsigned AverageSize : 7;
         unsigned UpdateWhenEmpty : 1;
#else
         unsigned UpdateWhenEmpty : 1;
         unsigned AverageSize : 7;
         unsigned StallPeriod : 24;
#endif
      };
      struct{
         unsigned value : 32;
      };
   } tTimerConfig;


   typedef enum
   {
   } tOutput_IfaceConstants;

   virtual tOutput readOutput(tRioStatusCode *status) = 0;
   virtual bool readOutput_Direction(tRioStatusCode *status) = 0;
   virtual signed int readOutput_Value(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ASource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ASource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ASource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_BSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_BSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_BSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexActiveHigh(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Reverse(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable(bool value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_ASource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_ASource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_ASource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_BSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_BSource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_BSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_IndexSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_IndexSource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_IndexSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual bool readConfig_IndexActiveHigh(tRioStatusCode *status) = 0;
   virtual bool readConfig_Reverse(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tTimerOutput_IfaceConstants;

   virtual tTimerOutput readTimerOutput(tRioStatusCode *status) = 0;
   virtual unsigned int readTimerOutput_Period(tRioStatusCode *status) = 0;
   virtual signed char readTimerOutput_Count(tRioStatusCode *status) = 0;
   virtual bool readTimerOutput_Stalled(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReset_IfaceConstants;

   virtual void strobeReset(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tTimerConfig_IfaceConstants;

   virtual void writeTimerConfig(tTimerConfig value, tRioStatusCode *status) = 0;
   virtual void writeTimerConfig_StallPeriod(unsigned int value, tRioStatusCode *status) = 0;
   virtual void writeTimerConfig_AverageSize(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeTimerConfig_UpdateWhenEmpty(bool value, tRioStatusCode *status) = 0;
   virtual tTimerConfig readTimerConfig(tRioStatusCode *status) = 0;
   virtual unsigned int readTimerConfig_StallPeriod(tRioStatusCode *status) = 0;
   virtual unsigned char readTimerConfig_AverageSize(tRioStatusCode *status) = 0;
   virtual bool readTimerConfig_UpdateWhenEmpty(tRioStatusCode *status) = 0;





private:
   tEncoder(const tEncoder&);
   void operator=(const tEncoder&);
};

}
}

#endif // __nFRC_2012_1_6_4_Encoder_h__
