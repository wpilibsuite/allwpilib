/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "vision/VisionRunner.h"

using namespace frc;

class VisionTester : public VisionPipeline {
 public:
  virtual ~VisionTester() = default;
  void Process(cv::Mat& mat) override {}
  void TestThing() {}
};

void TestVisionInitialization() {
  cs::CvSource source;
  VisionTester tester;
  VisionRunner<VisionTester> runner(source, &tester,
                                    [](VisionTester& t) { t.TestThing(); });

  runner.RunOnce();
  runner.RunForever();
}
