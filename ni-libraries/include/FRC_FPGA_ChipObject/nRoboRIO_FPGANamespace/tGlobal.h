// Copyright (c) National Instruments 2008.  All Rights Reserved.
// Do Not Edit... this file is generated!

#ifndef __nFRC_2017_17_0_2_Global_h__
#define __nFRC_2017_17_0_2_Global_h__

#include "../tSystem.h"
#include "../tSystemInterface.h"

namespace nFPGA
{
namespace nFRC_2017_17_0_2
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

   typedef
   union{
      struct{
#ifdef __vxworks
         unsigned Comm : 8;
         unsigned Mode : 8;
         unsigned RSL : 1;
#else
         unsigned RSL : 1;
         unsigned Mode : 8;
         unsigned Comm : 8;
#endif
      };
      struct{
         unsigned value : 17;
      };
   } tLEDs;



   typedef enum
   {
   } tLEDs_IfaceConstants;

   virtual void writeLEDs(tLEDs value, tRioStatusCode *status) = 0;
   virtual void writeLEDs_Comm(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeLEDs_Mode(unsigned char value, tRioStatusCode *status) = 0;
   virtual void writeLEDs_RSL(bool value, tRioStatusCode *status) = 0;
   virtual tLEDs readLEDs(tRioStatusCode *status) = 0;
   virtual unsigned char readLEDs_Comm(tRioStatusCode *status) = 0;
   virtual unsigned char readLEDs_Mode(tRioStatusCode *status) = 0;
   virtual bool readLEDs_RSL(tRioStatusCode *status) = 0;


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
   } tUserButton_IfaceConstants;

   virtual bool readUserButton(tRioStatusCode *status) = 0;


   typedef enum
   {
   } tRevision_IfaceConstants;

   virtual unsigned int readRevision(tRioStatusCode *status) = 0;




private:
   tGlobal(const tGlobal&);
   void operator=(const tGlobal&);
};

}
}

#endif // __nFRC_2017_17_0_2_Global_h__
