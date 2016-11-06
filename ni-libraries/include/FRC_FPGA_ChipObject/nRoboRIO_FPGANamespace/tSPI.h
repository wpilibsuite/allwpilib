// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2017_17_0_2_SPI_h__
#define __nFRC_2017_17_0_2_SPI_h__

#include "../tSystem.h"
#include "../tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2017_17_0_2
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
   } tEnableDIO_IfaceConstants;

   virtual void writeEnableDIO(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readEnableDIO(tRioStatusCode *status) = 0;


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

#endif // __nFRC_2017_17_0_2_SPI_h__
