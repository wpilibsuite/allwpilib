// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_C0EF_1_1_0_AI_h__
#define __nFRC_C0EF_1_1_0_AI_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_C0EF_1_1_0
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



   typedef enum
   {
   } tCalOK_IfaceConstants;

   virtual bool readCalOK(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDoneTime_IfaceConstants;

   virtual unsigned int readDoneTime(tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumOffsetRegisters = 8,
   } tOffset_IfaceConstants;

   virtual signed int readOffset(unsigned char reg_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumLSBWeightRegisters = 8,
   } tLSBWeight_IfaceConstants;

   virtual unsigned int readLSBWeight(unsigned char reg_index, tRioStatusCode *status) = 0;



private:
   tAI(const tAI&);
   void operator=(const tAI&);
};

}
}

#endif // __nFRC_C0EF_1_1_0_AI_h__
