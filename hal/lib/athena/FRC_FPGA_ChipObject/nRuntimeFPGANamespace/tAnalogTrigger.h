// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_AnalogTrigger_h__
#define __nFRC_2012_1_6_4_AnalogTrigger_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tAnalogTrigger
{
public:
   tAnalogTrigger(){}
   virtual ~tAnalogTrigger(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAnalogTrigger* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 8,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned InHysteresis : 1;
         unsigned OverLimit : 1;
         unsigned Rising : 1;
         unsigned Falling : 1;
#else
         unsigned Falling : 1;
         unsigned Rising : 1;
         unsigned OverLimit : 1;
         unsigned InHysteresis : 1;
#endif
      };
      struct{
         unsigned value : 4;
      };
   } tOutput;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Channel : 3;
         unsigned Module : 1;
         unsigned Averaged : 1;
         unsigned Filter : 1;
         unsigned FloatingRollover : 1;
         signed RolloverLimit : 8;
#else
         signed RolloverLimit : 8;
         unsigned FloatingRollover : 1;
         unsigned Filter : 1;
         unsigned Averaged : 1;
         unsigned Module : 1;
         unsigned Channel : 3;
#endif
      };
      struct{
         unsigned value : 15;
      };
   } tSourceSelect;


   typedef enum
   {
   } tSourceSelect_IfaceConstants;

   virtual void writeSourceSelect(tSourceSelect value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_Channel(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_Module(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_Averaged(bool value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_Filter(bool value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_FloatingRollover(bool value, tRioStatusCode *status) = 0;
   virtual void writeSourceSelect_RolloverLimit(signed short value, tRioStatusCode *status) = 0;
   virtual tSourceSelect readSourceSelect(tRioStatusCode *status) = 0;
   virtual unsigned char readSourceSelect_Channel(tRioStatusCode *status) = 0;
   virtual unsigned char readSourceSelect_Module(tRioStatusCode *status) = 0;
   virtual bool readSourceSelect_Averaged(tRioStatusCode *status) = 0;
   virtual bool readSourceSelect_Filter(tRioStatusCode *status) = 0;
   virtual bool readSourceSelect_FloatingRollover(tRioStatusCode *status) = 0;
   virtual signed short readSourceSelect_RolloverLimit(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tUpperLimit_IfaceConstants;

   virtual void writeUpperLimit(signed int value, tRioStatusCode *status) = 0;
   virtual signed int readUpperLimit(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLowerLimit_IfaceConstants;

   virtual void writeLowerLimit(signed int value, tRioStatusCode *status) = 0;
   virtual signed int readLowerLimit(tRioStatusCode *status) = 0;



   typedef enum
   {
      kNumOutputElements = 8,
   } tOutput_IfaceConstants;

   virtual tOutput readOutput(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readOutput_InHysteresis(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readOutput_OverLimit(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readOutput_Rising(unsigned char bitfield_index, tRioStatusCode *status) = 0;
   virtual bool readOutput_Falling(unsigned char bitfield_index, tRioStatusCode *status) = 0;




private:
   tAnalogTrigger(const tAnalogTrigger&);
   void operator=(const tAnalogTrigger&);
};

}
}

#endif // __nFRC_2012_1_6_4_AnalogTrigger_h__
