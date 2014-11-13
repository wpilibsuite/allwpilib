// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_DMA_h__
#define __nFRC_2015_1_0_A_DMA_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tDMA
{
public:
   tDMA(){}
   virtual ~tDMA(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tDMA* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Pause : 1;
         unsigned Enable_AI0_Low : 1;
         unsigned Enable_AI0_High : 1;
         unsigned Enable_AIAveraged0_Low : 1;
         unsigned Enable_AIAveraged0_High : 1;
         unsigned Enable_AI1_Low : 1;
         unsigned Enable_AI1_High : 1;
         unsigned Enable_AIAveraged1_Low : 1;
         unsigned Enable_AIAveraged1_High : 1;
         unsigned Enable_Accumulator0 : 1;
         unsigned Enable_Accumulator1 : 1;
         unsigned Enable_DI : 1;
         unsigned Enable_AnalogTriggers : 1;
         unsigned Enable_Counters_Low : 1;
         unsigned Enable_Counters_High : 1;
         unsigned Enable_CounterTimers_Low : 1;
         unsigned Enable_CounterTimers_High : 1;
         unsigned Enable_Encoders : 1;
         unsigned Enable_EncoderTimers : 1;
         unsigned ExternalClock : 1;
#else
         unsigned ExternalClock : 1;
         unsigned Enable_EncoderTimers : 1;
         unsigned Enable_Encoders : 1;
         unsigned Enable_CounterTimers_High : 1;
         unsigned Enable_CounterTimers_Low : 1;
         unsigned Enable_Counters_High : 1;
         unsigned Enable_Counters_Low : 1;
         unsigned Enable_AnalogTriggers : 1;
         unsigned Enable_DI : 1;
         unsigned Enable_Accumulator1 : 1;
         unsigned Enable_Accumulator0 : 1;
         unsigned Enable_AIAveraged1_High : 1;
         unsigned Enable_AIAveraged1_Low : 1;
         unsigned Enable_AI1_High : 1;
         unsigned Enable_AI1_Low : 1;
         unsigned Enable_AIAveraged0_High : 1;
         unsigned Enable_AIAveraged0_Low : 1;
         unsigned Enable_AI0_High : 1;
         unsigned Enable_AI0_Low : 1;
         unsigned Pause : 1;
#endif
      };
      struct{
         unsigned value : 20;
      };
   } tConfig;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned ExternalClockSource_Channel : 4;
         unsigned ExternalClockSource_Module : 1;
         unsigned ExternalClockSource_AnalogTrigger : 1;
         unsigned RisingEdge : 1;
         unsigned FallingEdge : 1;
#else
         unsigned FallingEdge : 1;
         unsigned RisingEdge : 1;
         unsigned ExternalClockSource_AnalogTrigger : 1;
         unsigned ExternalClockSource_Module : 1;
         unsigned ExternalClockSource_Channel : 4;
#endif
      };
      struct{
         unsigned value : 8;
      };
   } tExternalTriggers;



   typedef enum
   {
   } tRate_IfaceConstants;

   virtual void writeRate(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readRate(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Pause(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AI0_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AI0_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AIAveraged0_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AIAveraged0_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AI1_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AI1_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AIAveraged1_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AIAveraged1_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_Accumulator0(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_Accumulator1(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_DI(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_AnalogTriggers(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_Counters_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_Counters_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_CounterTimers_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_CounterTimers_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_Encoders(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enable_EncoderTimers(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ExternalClock(bool value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual bool readConfig_Pause(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AI0_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AI0_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AIAveraged0_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AIAveraged0_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AI1_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AI1_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AIAveraged1_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AIAveraged1_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_Accumulator0(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_Accumulator1(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_DI(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_AnalogTriggers(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_Counters_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_Counters_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_CounterTimers_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_CounterTimers_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_Encoders(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enable_EncoderTimers(tRioStatusCode *status) = 0;
   virtual bool readConfig_ExternalClock(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumExternalTriggersElements = 4,
   } tExternalTriggers_IfaceConstants;

   virtual void writeExternalTriggers(unsigned char bitfield_index, tExternalTriggers value, tRioStatusCode *status) = 0;
   virtual void writeExternalTriggers_ExternalClockSource_Channel(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeExternalTriggers_ExternalClockSource_Module(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeExternalTriggers_ExternalClockSource_AnalogTrigger(unsigned char bitfield_index, bool value, tRioStatusCode *status) = 0;
   virtual void writeExternalTriggers_RisingEdge(unsigned char bitfield_index, bool value, tRioStatusCode *status) = 0;
   virtual void writeExternalTriggers_FallingEdge(unsigned char bitfield_index, bool value, tRioStatusCode *status) = 0;
   virtual tExternalTriggers readExternalTriggers(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual unsigned char readExternalTriggers_ExternalClockSource_Channel(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual unsigned char readExternalTriggers_ExternalClockSource_Module(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readExternalTriggers_ExternalClockSource_AnalogTrigger(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readExternalTriggers_RisingEdge(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readExternalTriggers_FallingEdge(unsigned char bitfield_index, tRioStatusCode *status) = 0;




private:
   tDMA(const tDMA&);
   void operator=(const tDMA&);
};

}
}

#endif // __nFRC_2015_1_0_A_DMA_h__
