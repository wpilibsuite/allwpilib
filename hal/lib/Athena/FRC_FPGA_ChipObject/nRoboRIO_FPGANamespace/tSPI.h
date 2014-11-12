// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_SPI_h__
#define __nFRC_2015_1_0_A_SPI_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
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
         unsigned Hdr : 4;
         unsigned MXP : 1;
#else
         unsigned MXP : 1;
         unsigned Hdr : 4;
#endif
      };
      struct{
         unsigned value : 5;
      };
   } tChipSelectActiveHigh;



   typedef enum
   {
   } tChipSelectActiveHigh_IfaceConstants;

   virtual void writeChipSelectActiveHigh(tChipSelectActiveHigh value, tRioStatusCode *status) = 0;
   virtual void writeChipSelectActiveHigh_Hdr(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeChipSelectActiveHigh_MXP(unsigned char value, tRioStatusCode *status) = 0;
   virtual tChipSelectActiveHigh readChipSelectActiveHigh(tRioStatusCode *status) = 0;
   virtual unsigned char readChipSelectActiveHigh_Hdr(tRioStatusCode *status) = 0;
   virtual unsigned char readChipSelectActiveHigh_MXP(tRioStatusCode *status) = 0;




private:
   tSPI(const tSPI&);
   void operator=(const tSPI&);
};

}
}

#endif // __nFRC_2015_1_0_A_SPI_h__
