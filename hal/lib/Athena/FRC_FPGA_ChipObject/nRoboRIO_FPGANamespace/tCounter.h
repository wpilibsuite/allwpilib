// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Counter_h__
#define __nFRC_2015_1_0_A_Counter_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tCounter
{
public:
   tCounter(){}
   virtual ~tCounter(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tCounter* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 8,
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
         unsigned UpSource_Channel : 4;
         unsigned UpSource_Module : 1;
         unsigned UpSource_AnalogTrigger : 1;
         unsigned DownSource_Channel : 4;
         unsigned DownSource_Module : 1;
         unsigned DownSource_AnalogTrigger : 1;
         unsigned IndexSource_Channel : 4;
         unsigned IndexSource_Module : 1;
         unsigned IndexSource_AnalogTrigger : 1;
         unsigned IndexActiveHigh : 1;
         unsigned IndexEdgeSensitive : 1;
         unsigned UpRisingEdge : 1;
         unsigned UpFallingEdge : 1;
         unsigned DownRisingEdge : 1;
         unsigned DownFallingEdge : 1;
         unsigned Mode : 2;
         unsigned PulseLengthThreshold : 6;
#else
         unsigned PulseLengthThreshold : 6;
         unsigned Mode : 2;
         unsigned DownFallingEdge : 1;
         unsigned DownRisingEdge : 1;
         unsigned UpFallingEdge : 1;
         unsigned UpRisingEdge : 1;
         unsigned IndexEdgeSensitive : 1;
         unsigned IndexActiveHigh : 1;
         unsigned IndexSource_AnalogTrigger : 1;
         unsigned IndexSource_Module : 1;
         unsigned IndexSource_Channel : 4;
         unsigned DownSource_AnalogTrigger : 1;
         unsigned DownSource_Module : 1;
         unsigned DownSource_Channel : 4;
         unsigned UpSource_AnalogTrigger : 1;
         unsigned UpSource_Module : 1;
         unsigned UpSource_Channel : 4;
#endif
      };
      struct{
         unsigned value : 32;
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
   virtual void writeConfig_UpSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_UpSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_UpSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DownSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DownSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DownSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexActiveHigh(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_IndexEdgeSensitive(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_UpRisingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_UpFallingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DownRisingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DownFallingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Mode(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_PulseLengthThreshold(unsigned short value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_UpSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_UpSource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_UpSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_DownSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_DownSource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_DownSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_IndexSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_IndexSource_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_IndexSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual bool readConfig_IndexActiveHigh(tRioStatusCode *status) = 0;
   virtual bool readConfig_IndexEdgeSensitive(tRioStatusCode *status) = 0;
   virtual bool readConfig_UpRisingEdge(tRioStatusCode *status) = 0;
   virtual bool readConfig_UpFallingEdge(tRioStatusCode *status) = 0;
   virtual bool readConfig_DownRisingEdge(tRioStatusCode *status) = 0;
   virtual bool readConfig_DownFallingEdge(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_Mode(tRioStatusCode *status) = 0;
   virtual unsigned short readConfig_PulseLengthThreshold(tRioStatusCode *status) = 0;


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
   tCounter(const tCounter&);
   void operator=(const tCounter&);
};

}
}

#endif // __nFRC_2015_1_0_A_Counter_h__
