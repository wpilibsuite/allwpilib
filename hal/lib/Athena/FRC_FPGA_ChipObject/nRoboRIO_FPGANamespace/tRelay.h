// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2015_1_0_A_Relay_h__
#define __nFRC_2015_1_0_A_Relay_h__

#include "tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2015_1_0_A
{

class tRelay
{
public:
   tRelay(){}
   virtual ~tRelay(){}

   virtual tSystemInterface* getSystemInterface() = 0;
   static tRelay* create(tRioStatusCode *status);

   typedef enum
   {
      kNumSystems = 1,
   } tIfaceConstants;

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Forward : 4;
         unsigned Reverse : 4;
#else
         unsigned Reverse : 4;
         unsigned Forward : 4;
#endif
      };
      struct{
         unsigned value : 8;
      };
   } tValue;



   typedef enum
   {
   } tValue_IfaceConstants;

   virtual void writeValue(tValue value, tRioStatusCode *status) = 0;
   virtual void writeValue_Forward(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeValue_Reverse(unsigned char value, tRioStatusCode *status) = 0;
   virtual tValue readValue(tRioStatusCode *status) = 0;
   virtual unsigned char readValue_Forward(tRioStatusCode *status) = 0;
   virtual unsigned char readValue_Reverse(tRioStatusCode *status) = 0;




private:
   tRelay(const tRelay&);
   void operator=(const tRelay&);
};

}
}

#endif // __nFRC_2015_1_0_A_Relay_h__
