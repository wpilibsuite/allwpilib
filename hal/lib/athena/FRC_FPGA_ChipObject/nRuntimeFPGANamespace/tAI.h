// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_AI_h__
#define __nFRC_2012_1_6_4_AI_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tAI
{
public:
   tAI(){}
   virtual ~tAI(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAI* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 2,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Channel : 3;
         unsigned Module : 1;
         unsigned Averaged : 1;
#else
         unsigned Averaged : 1;
         unsigned Module : 1;
         unsigned Channel : 3;
#endif
      };
      struct{
         unsigned value : 5;
      };
   } tReadSelect;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned ScanSize : 3;
         unsigned ConvertRate : 26;
#else
         unsigned ConvertRate : 26;
         unsigned ScanSize : 3;
#endif
      };
      struct{
         unsigned value : 29;
      };
   } tConfig;


   typedef enum
   {
   } tConfig_IfaceConstants;

   virtual void writeConfig(tConfig value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ScanSize(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeConfig_ConvertRate(unsigned int value, tRioStatusCode *status) = 0;
   virtual tConfig readConfig(tRioStatusCode *status) = 0;
   virtual unsigned char readConfig_ScanSize(tRioStatusCode *status) = 0;
   virtual unsigned int readConfig_ConvertRate(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLoopTiming_IfaceConstants;

   virtual unsigned int readLoopTiming(tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumOversampleBitsElements = 8,
   } tOversampleBits_IfaceConstants;

   virtual void writeOversampleBits(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readOversampleBits(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumAverageBitsElements = 8,
   } tAverageBits_IfaceConstants;

   virtual void writeAverageBits(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readAverageBits(unsigned char bitfield_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumScanListElements = 8,
   } tScanList_IfaceConstants;

   virtual void writeScanList(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readScanList(unsigned char bitfield_index, tRioStatusCode *status) = 0;



   typedef enum
   {
   } tOutput_IfaceConstants;

   virtual signed int readOutput(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLatchOutput_IfaceConstants;

   virtual void strobeLatchOutput(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReadSelect_IfaceConstants;

   virtual void writeReadSelect(tReadSelect value, tRioStatusCode *status) = 0;
   virtual void writeReadSelect_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeReadSelect_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeReadSelect_Averaged(bool value, tRioStatusCode *status) = 0;
   virtual tReadSelect readReadSelect(tRioStatusCode *status) = 0;
   virtual unsigned char readReadSelect_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readReadSelect_Module(tRioStatusCode *status) = 0;
   virtual bool readReadSelect_Averaged(tRioStatusCode *status) = 0;




private:
   tAI(const tAI&);
   void operator=(const tAI&);
};

}
}

#endif // __nFRC_2012_1_6_4_AI_h__
