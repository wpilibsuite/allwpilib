// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_4_Accel_h__
#define __nFRC_2015_1_0_4_Accel_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_4
{

class tAccel
{
public:
   tAccel(){}
   virtual ~tAccel(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAccel* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
   } tX_IfaceConstants;

   virtual signed short readX(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tY_IfaceConstants;

   virtual signed short readY(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tZ_IfaceConstants;

   virtual signed short readZ(tRioStatusCode *status) = 0;




private:
   tAccel(const tAccel&);
   void operator=(const tAccel&);
};

}
}

#endif // __nFRC_2015_1_0_4_Accel_h__
