/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Encoder.h"
#include "frc/QuadratureEncoder.h"

using namespace frc;

Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType) :
                 QuadratureEncoder(aChannel, bChannel, reverseDirection, encodingType){}

Encoder::Encoder(DigitalSource* aSource, DigitalSource* bSource,
                 bool reverseDirection, EncodingType encodingType) :
                 QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}

Encoder::Encoder(DigitalSource& aSource, DigitalSource& bSource,
                 bool reverseDirection, EncodingType encodingType):
                 QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}


Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource, bool reverseDirection,
                 EncodingType encodingType) :
                 QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}

