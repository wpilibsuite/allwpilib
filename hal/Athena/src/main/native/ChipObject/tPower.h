// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_4_Power_h__
#define __nFRC_2015_1_0_4_Power_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_4
{

class tPower
{
public:
   tPower(){}
   virtual ~tPower(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tPower* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned User3V3 : 8;
         unsigned User5V : 8;
         unsigned User6V : 8;
#else
         unsigned User6V : 8;
         unsigned User5V : 8;
         unsigned User3V3 : 8;
#endif
      };
      struct{
         unsigned value : 24;
      };
   } tStatus;
   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned User3V3 : 1;
         unsigned User5V : 1;
         unsigned User6V : 1;
#else
         unsigned User6V : 1;
         unsigned User5V : 1;
         unsigned User3V3 : 1;
#endif
      };
      struct{
         unsigned value : 3;
      };
   } tDisable;



   typedef enum
   {
   } tStatus_IfaceConstants;

   virtual tStatus readStatus(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User3V3(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User5V(tRioStatusCode *status) = 0;
   virtual unsigned char readStatus_User6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tDisable_IfaceConstants;

   virtual void writeDisable(tDisable value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User3V3(bool value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User5V(bool value, tRioStatusCode *status) = 0;
   virtual void writeDisable_User6V(bool value, tRioStatusCode *status) = 0;
   virtual tDisable readDisable(tRioStatusCode *status) = 0;
   virtual bool readDisable_User3V3(tRioStatusCode *status) = 0;
   virtual bool readDisable_User5V(tRioStatusCode *status) = 0;
   virtual bool readDisable_User6V(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tIndicateOutOfRange_IfaceConstants;

   virtual void writeIndicateOutOfRange(bool value, tRioStatusCode *status) = 0;
   virtual bool readIndicateOutOfRange(tRioStatusCode *status) = 0;




private:
   tPower(const tPower&);
   void operator=(const tPower&);
};

}
}

#endif // __nFRC_2015_1_0_4_Power_h__
