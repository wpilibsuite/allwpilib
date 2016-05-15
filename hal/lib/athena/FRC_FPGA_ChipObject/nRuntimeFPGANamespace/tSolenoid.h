// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2012_1_6_4_Solenoid_h__
#define __nFRC_2012_1_6_4_Solenoid_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2012_1_6_4
{

class tSolenoid
{
public:
   tSolenoid(){}
   virtual ~tSolenoid(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tSolenoid* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
      kNumDO7_0Elements = 2,
   } tDO7_0_IfaceConstants;

   virtual void writeDO7_0(unsigned char bitfield_index, unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readDO7_0(unsigned char bitfield_index, tRioStatusCode *status) = 0;




private:
   tSolenoid(const tSolenoid&);
   void operator=(const tSolenoid&);
};

}
}

#endif // __nFRC_2012_1_6_4_Solenoid_h__
