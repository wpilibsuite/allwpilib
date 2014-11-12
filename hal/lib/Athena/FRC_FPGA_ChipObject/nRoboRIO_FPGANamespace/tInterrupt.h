// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Interrupt_h__
#define __nFRC_2015_1_0_A_Interrupt_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tInterrupt
{
public:
   tInterrupt(){}
   virtual ~tInterrupt(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tInterrupt* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 8,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Source_Channel : 4;
         unsigned Source_Module : 1;
         unsigned Source_AnalogTrigger : 1;
         unsigned RisingEdge : 1;
         unsigned FallingEdge : 1;
         unsigned WaitForAck : 1;
#else
         unsigned WaitForAck : 1;
         unsigned FallingEdge : 1;
         unsigned RisingEdge : 1;
         unsigned Source_AnalogTrigger : 1;
         unsigned Source_Module : 1;
         unsigned Source_Channel : 4;
#endif
      };
      struct{
         unsigned value : 9;
      };
   } tConfig;


   typedef enum
   {
   } tFallingTimeStamp_IfaceConstants;

   virtual unsigned int readFallingTimeStamp(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Source_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Source_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_Source_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_RisingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_FallingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_WaitForAck(bool value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_Source_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_Source_Module(tRioStatusCode *status) = 0;
   virtual bool readConfig_Source_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual bool readConfig_RisingEdge(tRioStatusCode *status) = 0;
   virtual bool readConfig_FallingEdge(tRioStatusCode *status) = 0;
   virtual bool readConfig_WaitForAck(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tRisingTimeStamp_IfaceConstants;

   virtual unsigned int readRisingTimeStamp(tRioStatusCode *status) = 0;





private:
   tInterrupt(const tInterrupt&);
   void operator=(const tInterrupt&);
};

}
}

#endif // __nFRC_2015_1_0_A_Interrupt_h__
