// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Accel_h__
#define __nFRC_2015_1_0_A_Accel_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
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
   } tSTAT_IfaceConstants;

   virtual unsigned char readSTAT(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tCNTR_IfaceConstants;

   virtual void writeCNTR(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readCNTR(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDATO_IfaceConstants;

   virtual void writeDATO(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readDATO(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tCNFG_IfaceConstants;

   virtual void writeCNFG(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readCNFG(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tCNTL_IfaceConstants;

   virtual void writeCNTL(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readCNTL(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDATI_IfaceConstants;

   virtual unsigned char readDATI(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tGO_IfaceConstants;

   virtual void strobeGO(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tADDR_IfaceConstants;

   virtual void writeADDR(unsigned char value, tRioStatusCode *status) = 0;
   virtual unsigned char readADDR(tRioStatusCode *status) = 0;




private:
   tAccel(const tAccel&);
   void operator=(const tAccel&);
};

}
}

#endif // __nFRC_2015_1_0_A_Accel_h__
