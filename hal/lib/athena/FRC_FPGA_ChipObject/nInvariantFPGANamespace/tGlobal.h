// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_C0EF_1_1_0_Global_h__
#define __nFRC_C0EF_1_1_0_Global_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_C0EF_1_1_0
{

class tGlobal
{
public:
   tGlobal(){}
   virtual ~tGlobal(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tGlobal* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;




   typedef enum
   {
   } tVersion_IfaceConstants;

   virtual unsigned short readVersion(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tLocalTime_IfaceConstants;

   virtual unsigned int readLocalTime(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tRevision_IfaceConstants;

   virtual unsigned int readRevision(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tReserved_IfaceConstants;

   virtual unsigned char readReserved(tRioStatusCode *status) = 0;




private:
   tGlobal(const tGlobal&);
   void operator=(const tGlobal&);
};

}
}

#endif // __nFRC_C0EF_1_1_0_Global_h__
