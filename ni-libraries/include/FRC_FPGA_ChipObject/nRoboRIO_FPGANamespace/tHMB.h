// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2018_18_0_8_HMB_h__
#define __nFRC_2018_18_0_8_HMB_h__

#include "../tSystem.h"
#include "../tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2018_18_0_8
{

class tHMB
{
public:
   tHMB(){}
   virtual ~tHMB(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tHMB* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Enables_AI0_Low : 1;
         unsigned Enables_AI0_High : 1;
         unsigned Enables_AIAveraged0_Low : 1;
         unsigned Enables_AIAveraged0_High : 1;
         unsigned Enables_AI1_Low : 1;
         unsigned Enables_AI1_High : 1;
         unsigned Enables_AIAveraged1_Low : 1;
         unsigned Enables_AIAveraged1_High : 1;
         unsigned Enables_Accumulator0 : 1;
         unsigned Enables_Accumulator1 : 1;
         unsigned Enables_DI : 1;
         unsigned Enables_AnalogTriggers : 1;
         unsigned Enables_Counters_Low : 1;
         unsigned Enables_Counters_High : 1;
         unsigned Enables_CounterTimers_Low : 1;
         unsigned Enables_CounterTimers_High : 1;
         unsigned Enables_Encoders_Low : 1;
         unsigned Enables_Encoders_High : 1;
         unsigned Enables_EncoderTimers_Low : 1;
         unsigned Enables_EncoderTimers_High : 1;
#else
         unsigned Enables_EncoderTimers_High : 1;
         unsigned Enables_EncoderTimers_Low : 1;
         unsigned Enables_Encoders_High : 1;
         unsigned Enables_Encoders_Low : 1;
         unsigned Enables_CounterTimers_High : 1;
         unsigned Enables_CounterTimers_Low : 1;
         unsigned Enables_Counters_High : 1;
         unsigned Enables_Counters_Low : 1;
         unsigned Enables_AnalogTriggers : 1;
         unsigned Enables_DI : 1;
         unsigned Enables_Accumulator1 : 1;
         unsigned Enables_Accumulator0 : 1;
         unsigned Enables_AIAveraged1_High : 1;
         unsigned Enables_AIAveraged1_Low : 1;
         unsigned Enables_AI1_High : 1;
         unsigned Enables_AI1_Low : 1;
         unsigned Enables_AIAveraged0_High : 1;
         unsigned Enables_AIAveraged0_Low : 1;
         unsigned Enables_AI0_High : 1;
         unsigned Enables_AI0_Low : 1;
#endif
      };
      struct{
         unsigned value : 20;
      };
   } tConfig;



   typedef enum
   {
   } tForceOnce_IfaceConstants;

   virtual void writeForceOnce(bool value, tRioStatusCode *status) = 0;
   virtual bool readForceOnce(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AI0_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AI0_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AIAveraged0_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AIAveraged0_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AI1_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AI1_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AIAveraged1_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AIAveraged1_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Accumulator0(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Accumulator1(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_DI(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_AnalogTriggers(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Counters_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Counters_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_CounterTimers_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_CounterTimers_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Encoders_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_Encoders_High(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_EncoderTimers_Low(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Enables_EncoderTimers_High(bool value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AI0_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AI0_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AIAveraged0_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AIAveraged0_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AI1_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AI1_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AIAveraged1_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AIAveraged1_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Accumulator0(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Accumulator1(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_DI(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_AnalogTriggers(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Counters_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Counters_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_CounterTimers_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_CounterTimers_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Encoders_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_Encoders_High(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_EncoderTimers_Low(tRioStatusCode *status) = 0;
   virtual bool readConfig_Enables_EncoderTimers_High(tRioStatusCode *status) = 0;




private:
   tHMB(const tHMB&);
   void operator=(const tHMB&);
};

}
}

#endif // __nFRC_2018_18_0_8_HMB_h__
