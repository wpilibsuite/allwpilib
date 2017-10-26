// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2018_18_0_4_SPI_h__
#define __nFRC_2018_18_0_4_SPI_h__

#include "../tSystem.h"
#include "../tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2018_18_0_4
{

class tSPI
{
public:
   tSPI(){}
   virtual ~tSPI(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tSPI* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

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
   } tAutoTriggerConfig;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Hdr : 4;
         unsigned MXP : 1;
#else
         unsigned MXP : 1;
         unsigned Hdr : 4;
#endif
      };
      struct{
         unsigned value : 5;
      };
   } tChipSelectActiveHigh;



   typedef enum
   {
   } tDebugIntStatReadCount_IfaceConstants;

   virtual unsigned int readDebugIntStatReadCount(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDebugState_IfaceConstants;

   virtual unsigned short readDebugState(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAutoTriggerConfig_IfaceConstants;

   virtual void writeAutoTriggerConfig(tAutoTriggerConfig value, tRioStatusCode *status) = 0;
   virtual void writeAutoTriggerConfig_ExternalClockSource_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeAutoTriggerConfig_ExternalClockSource_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeAutoTriggerConfig_ExternalClockSource_AnalogTrigger(bool value, tRioStatusCode *status) = 0;
   virtual void writeAutoTriggerConfig_RisingEdge(bool value, tRioStatusCode *status) = 0;
   virtual void writeAutoTriggerConfig_FallingEdge(bool value, tRioStatusCode *status) = 0;
   virtual tAutoTriggerConfig readAutoTriggerConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readAutoTriggerConfig_ExternalClockSource_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readAutoTriggerConfig_ExternalClockSource_Module(tRioStatusCode *status) = 0;
   virtual bool readAutoTriggerConfig_ExternalClockSource_AnalogTrigger(tRioStatusCode *status) = 0;
   virtual bool readAutoTriggerConfig_RisingEdge(tRioStatusCode *status) = 0;
   virtual bool readAutoTriggerConfig_FallingEdge(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAutoChipSelect_IfaceConstants;

   virtual void writeAutoChipSelect(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readAutoChipSelect(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAutoForceGo_IfaceConstants;

   virtual void strobeAutoForceGo(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDebugRevision_IfaceConstants;

   virtual unsigned int readDebugRevision(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAutoByteCount_IfaceConstants;

   virtual void writeAutoByteCount(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readAutoByteCount(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDebugIntStat_IfaceConstants;

   virtual unsigned int readDebugIntStat(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDebugEnabled_IfaceConstants;

   virtual unsigned int readDebugEnabled(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAutoSPI1Select_IfaceConstants;

   virtual void writeAutoSPI1Select(bool value, tRioStatusCode *status) = 0;
   virtual bool readAutoSPI1Select(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDebugSubstate_IfaceConstants;

   virtual unsigned char readDebugSubstate(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tEnableDIO_IfaceConstants;

   virtual void writeEnableDIO(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readEnableDIO(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tChipSelectActiveHigh_IfaceConstants;

   virtual void writeChipSelectActiveHigh(tChipSelectActiveHigh value, tRioStatusCode *status) = 0;
   virtual void writeChipSelectActiveHigh_Hdr(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChipSelectActiveHigh_MXP(unsigned char value, tRioStatusCode *status) = 0;
   virtual tChipSelectActiveHigh readChipSelectActiveHigh(tRioStatusCode *status) = 0;
   virtual unsigned char readChipSelectActiveHigh_Hdr(tRioStatusCode *status) = 0;
   virtual unsigned char readChipSelectActiveHigh_MXP(tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumAutoTxRegisters = 4,
      kNumAutoTxElements = 4,
   } tAutoTx_IfaceConstants;

   virtual void writeAutoTx(unsigned char reg_index, unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readAutoTx(unsigned char reg_index, unsigned char bitfield_index, tRioStatusCode *status) = 0;


private:
   tSPI(const tSPI&);
   void operator=(const tSPI&);
};

}
}

#endif // __nFRC_2018_18_0_4_SPI_h__
