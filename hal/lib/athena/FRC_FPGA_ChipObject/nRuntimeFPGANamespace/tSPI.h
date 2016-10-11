// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_SPI_h__
#define __nFRC_2012_1_6_4_SPI_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
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
         unsigned ReceivedDataOverflow : 1;
         unsigned Idle : 1;
#else
         unsigned Idle : 1;
         unsigned ReceivedDataOverflow : 1;
#endif
      };
      struct{
         unsigned value : 2;
      };
   } tStatus;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned BusBitWidth : 8;
         unsigned ClockHalfPeriodDelay : 8;
         unsigned MSBfirst : 1;
         unsigned DataOnFalling : 1;
         unsigned LatchFirst : 1;
         unsigned LatchLast : 1;
         unsigned FramePolarity : 1;
         unsigned WriteOnly : 1;
         unsigned ClockPolarity : 1;
#else
         unsigned ClockPolarity : 1;
         unsigned WriteOnly : 1;
         unsigned FramePolarity : 1;
         unsigned LatchLast : 1;
         unsigned LatchFirst : 1;
         unsigned DataOnFalling : 1;
         unsigned MSBfirst : 1;
         unsigned ClockHalfPeriodDelay : 8;
         unsigned BusBitWidth : 8;
#endif
      };
      struct{
         unsigned value : 23;
      };
   } tConfig;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned SCLK_Channel : 4;
         unsigned SCLK_Module : 1;
         unsigned MOSI_Channel : 4;
         unsigned MOSI_Module : 1;
         unsigned MISO_Channel : 4;
         unsigned MISO_Module : 1;
         unsigned SS_Channel : 4;
         unsigned SS_Module : 1;
#else
         unsigned SS_Module : 1;
         unsigned SS_Channel : 4;
         unsigned MISO_Module : 1;
         unsigned MISO_Channel : 4;
         unsigned MOSI_Module : 1;
         unsigned MOSI_Channel : 4;
         unsigned SCLK_Module : 1;
         unsigned SCLK_Channel : 4;
#endif
      };
      struct{
         unsigned value : 20;
      };
   } tChannels;



   typedef enum
   {
   } tStatus_IfaceConstants;

   virtual tStatus readStatus(tRioStatusCode *status) = 0;
   virtual bool readStatus_ReceivedDataOverflow(tRioStatusCode *status) = 0;
   virtual bool readStatus_Idle(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReceivedData_IfaceConstants;

   virtual unsigned int readReceivedData(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDataToLoad_IfaceConstants;

   virtual void writeDataToLoad(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readDataToLoad(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_BusBitWidth(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ClockHalfPeriodDelay(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_MSBfirst(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_DataOnFalling(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_LatchFirst(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_LatchLast(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_FramePolarity(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_WriteOnly(bool value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ClockPolarity(bool value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_BusBitWidth(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_ClockHalfPeriodDelay(tRioStatusCode *status) = 0;
   virtual bool readConfig_MSBfirst(tRioStatusCode *status) = 0;
   virtual bool readConfig_DataOnFalling(tRioStatusCode *status) = 0;
   virtual bool readConfig_LatchFirst(tRioStatusCode *status) = 0;
   virtual bool readConfig_LatchLast(tRioStatusCode *status) = 0;
   virtual bool readConfig_FramePolarity(tRioStatusCode *status) = 0;
   virtual bool readConfig_WriteOnly(tRioStatusCode *status) = 0;
   virtual bool readConfig_ClockPolarity(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tClearReceivedData_IfaceConstants;

   virtual void strobeClearReceivedData(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReceivedElements_IfaceConstants;

   virtual unsigned short readReceivedElements(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLoad_IfaceConstants;

   virtual void strobeLoad(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReset_IfaceConstants;

   virtual void strobeReset(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tChannels_IfaceConstants;

   virtual void writeChannels(tChannels value, tRioStatusCode *status) = 0;
   virtual void writeChannels_SCLK_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_SCLK_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_MOSI_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_MOSI_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_MISO_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_MISO_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_SS_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChannels_SS_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual tChannels readChannels(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_SCLK_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_SCLK_Module(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_MOSI_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_MOSI_Module(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_MISO_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_MISO_Module(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_SS_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readChannels_SS_Module(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tAvailableToLoad_IfaceConstants;

   virtual unsigned short readAvailableToLoad(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReadReceivedData_IfaceConstants;

   virtual void strobeReadReceivedData(tRioStatusCode *status) = 0;




private:
   tSPI(const tSPI&);
   void operator=(const tSPI&);
};

}
}

#endif // __nFRC_2012_1_6_4_SPI_h__
