// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_AO_h__
#define __nFRC_2015_1_0_A_AO_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tAO
{
public:
   tAO(){}
   virtual ~tAO(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tAO* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;






   typedef enum
   {
      kNumMXPRegisters = 2,
   } tMXP_IfaceConstants;

   virtual void writeMXP(unsigned char reg_index, unsigned short value, tRioStatusCode *status) = 0;
   virtual unsigned short readMXP(unsigned char reg_index, tRioStatusCode *status) = 0;


private:
   tAO(const tAO&);
   void operator=(const tAO&);
};

}
}

#endif // __nFRC_2015_1_0_A_AO_h__
