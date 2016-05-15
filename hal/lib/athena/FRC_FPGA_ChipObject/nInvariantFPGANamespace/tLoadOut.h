// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_C0EF_1_1_0_LoadOut_h__
#define __nFRC_C0EF_1_1_0_LoadOut_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_C0EF_1_1_0
{

class tLoadOut
{
public:
   tLoadOut(){}
   virtual ~tLoadOut(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tLoadOut* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
   } tReady_IfaceConstants;

   virtual bool readReady(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDoneTime_IfaceConstants;

   virtual unsigned int readDoneTime(tRioStatusCode *status) = 0;




   typedef enum
   {
      kNumVendorIDRegisters = 8,
   } tVendorID_IfaceConstants;

   virtual unsigned short readVendorID(unsigned char reg_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumSerialNumberRegisters = 8,
   } tSerialNumber_IfaceConstants;

   virtual unsigned int readSerialNumber(unsigned char reg_index, tRioStatusCode *status) = 0;


   typedef enum
   {
      kNumModuleIDRegisters = 8,
   } tModuleID_IfaceConstants;

   virtual unsigned short readModuleID(unsigned char reg_index, tRioStatusCode *status) = 0;


private:
   tLoadOut(const tLoadOut&);
   void operator=(const tLoadOut&);
};

}
}

#endif // __nFRC_C0EF_1_1_0_LoadOut_h__
