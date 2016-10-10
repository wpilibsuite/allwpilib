// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_Accumulator_h__
#define __nFRC_2012_1_6_4_Accumulator_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tAccumulator
{
public:
   tAccumulator(){}
   virtual ~tAccumulator(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAccumulator* create(unsigned char sys_index, tRioStatusCode *status);
   virtual unsigned char getSystemIndex() = 0;


   typedef enum
   {
      kNumSystems = 2,
   } tIfaceConstants;

   typedef
   union{
      struct{
         signed long long Value;
         unsigned Count : 32;
      };
      struct{
         unsigned value : 32;
         unsigned value2 : 32;
         unsigned value3 : 32;
      };
   } tOutput;


   typedef enum
   {
   } tOutput_IfaceConstants;

   virtual tOutput readOutput(tRioStatusCode *status) = 0;
   virtual signed long long readOutput_Value(tRioStatusCode *status) = 0;
   virtual unsigned int readOutput_Count(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tCenter_IfaceConstants;

   virtual void writeCenter(signed int value, tRioStatusCode *status) = 0;
   virtual signed int readCenter(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDeadband_IfaceConstants;

   virtual void writeDeadband(signed int value, tRioStatusCode *status) = 0;
   virtual signed int readDeadband(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReset_IfaceConstants;

   virtual void strobeReset(tRioStatusCode *status) = 0;





private:
   tAccumulator(const tAccumulator&);
   void operator=(const tAccumulator&);
};

}
}

#endif // __nFRC_2012_1_6_4_Accumulator_h__
