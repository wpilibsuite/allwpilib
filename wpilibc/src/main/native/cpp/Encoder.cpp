// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Encoder.h"

#include "frc/QuadratureEncoder.h"

using namespace frc;

Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType)
    : QuadratureEncoder(aChannel, bChannel, reverseDirection, encodingType) {}

Encoder::Encoder(DigitalSource* aSource, DigitalSource* bSource,
                 bool reverseDirection, EncodingType encodingType)
    : QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}

Encoder::Encoder(DigitalSource& aSource, DigitalSource& bSource,
                 bool reverseDirection, EncodingType encodingType)
    : QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}

Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource, bool reverseDirection,
                 EncodingType encodingType)
    : QuadratureEncoder(aSource, bSource, reverseDirection, encodingType) {}
