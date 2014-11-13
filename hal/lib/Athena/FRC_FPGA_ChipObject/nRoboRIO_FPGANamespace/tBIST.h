// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_BIST_h__
#define __nFRC_2015_1_0_A_BIST_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tBIST
{
public:
   tBIST(){}
   virtual ~tBIST(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tBIST* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
   } tDO0SquareTicks_IfaceConstants;

   virtual void writeDO0SquareTicks(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readDO0SquareTicks(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tEnable_IfaceConstants;

   virtual void writeEnable(bool value, tRioStatusCode *status) = 0;
   virtual bool readEnable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDO1SquareEnable_IfaceConstants;

   virtual void writeDO1SquareEnable(bool value, tRioStatusCode *status) = 0;
   virtual bool readDO1SquareEnable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDO0SquareEnable_IfaceConstants;

   virtual void writeDO0SquareEnable(bool value, tRioStatusCode *status) = 0;
   virtual bool readDO0SquareEnable(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDO1SquareTicks_IfaceConstants;

   virtual void writeDO1SquareTicks(unsigned int value, tRioStatusCode *status) = 0;
   virtual unsigned int readDO1SquareTicks(tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumDORegisters = 2,
   } tDO_IfaceConstants;

   virtual void writeDO(unsigned char reg_index, bool value, tRioStatusCode *status) = 0;
   virtual bool readDO(unsigned char reg_index, tRioStatusCode *status) = 0;


private:
   tBIST(const tBIST&);
   void operator=(const tBIST&);
};

}
}

#endif // __nFRC_2015_1_0_A_BIST_h__
