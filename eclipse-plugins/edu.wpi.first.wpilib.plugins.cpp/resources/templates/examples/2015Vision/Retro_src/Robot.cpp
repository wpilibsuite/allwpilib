#include "WPILib.h"
#include <vector>
#include <cmath>

/**
 * Example of finding yellow totes based on retroreflective target.
 * This example utilizes an image file, which you need to copy to the roboRIO
 * To use a camera you will have to integrate the appropriate camera details with this example.
 * To use a USB camera instead, see the IntermediateVision example for details
 * on using the USB camera. To use an Axis Camera, see the AxisCamera example for details on
 * using an Axis Camera.
 *
 * Sample images can be found here: http://wp.wpi.edu/wpilib/2015/01/16/sample-images-for-vision-projects/ 
 */
class VisionRetro2015Sample : public SampleRobot
{
	//A structure to hold measurements of a particle
	struct ParticleReport {
		double PercentAreaToImageArea;
		double Area;
		double BoundingRectLeft;
		double BoundingRectTop;
		double BoundingRectRight;
		double BoundingRectBottom;
	};

	//Structure to represent the scores for the various tests used for target identification
	struct Scores {
		double Area;
		double Aspect;
	};

	//Images
	Image *frame;
	Image *binaryFrame;
	int imaqError;

	//Constants
	Range RING_HUE_RANGE = {101, 64};	//Default hue range for ring light
	Range RING_SAT_RANGE = {88, 255};	//Default saturation range for ring light
	Range RING_VAL_RANGE = {134, 255};	//Default value range for ring light
	double AREA_MINIMUM = 0.5; //Default Area minimum for particle as a percentage of total image area
	double LONG_RATIO = 2.22; //Tote long side = 26.9 / Tote height = 12.1 = 2.22
	double SHORT_RATIO = 1.4; //Tote short side = 16.9 / Tote height = 12.1 = 1.4
	double SCORE_MIN = 75.0;  //Minimum score to be considered a tote
	double VIEW_ANGLE = 49.4; //View angle fo camera, set to Axis m1011 by default, 64 for m1013, 51.7 for 206, 52 for HD3000 square, 60 for HD3000 640x480
	ParticleFilterCriteria2 criteria[1];
	ParticleFilterOptions2 filterOptions = {0,0,1,1};
	Scores scores;


public:
	void RobotInit() override {
	    // create images
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		binaryFrame = imaqCreateImage(IMAQ_IMAGE_U8, 0);

		//Put default values to SmartDashboard so fields will appear
		SmartDashboard::PutNumber("Tote hue min", RING_HUE_RANGE.minValue);
		SmartDashboard::PutNumber("Tote hue max", RING_HUE_RANGE.maxValue);
		SmartDashboard::PutNumber("Tote sat min", RING_SAT_RANGE.minValue);
		SmartDashboard::PutNumber("Tote sat max", RING_SAT_RANGE.maxValue);
		SmartDashboard::PutNumber("Tote val min", RING_VAL_RANGE.minValue);
		SmartDashboard::PutNumber("Tote val max", RING_VAL_RANGE.maxValue);
		SmartDashboard::PutNumber("Area min %", AREA_MINIMUM);
	}

	void Autonomous() override {
		while (IsAutonomous() && IsEnabled())
		{
			//read file in from disk. For this example to run you need to copy image.jpg from the SampleImages folder to the
			//directory shown below using FTP or SFTP: http://wpilib.screenstepslive.com/s/4485/m/24166/l/282299-roborio-ftp
			imaqError = imaqReadFile(frame, "//home//lvuser//SampleImages//image.jpg", NULL, NULL);

			//Update threshold values from SmartDashboard. For performance reasons it is recommended to remove this after calibration is finished.
			RING_HUE_RANGE.minValue = SmartDashboard::GetNumber("Tote hue min", RING_HUE_RANGE.minValue);
			RING_HUE_RANGE.maxValue = SmartDashboard::GetNumber("Tote hue max", RING_HUE_RANGE.maxValue);
			RING_SAT_RANGE.minValue = SmartDashboard::GetNumber("Tote sat min", RING_SAT_RANGE.minValue);
			RING_SAT_RANGE.maxValue = SmartDashboard::GetNumber("Tote sat max", RING_SAT_RANGE.maxValue);
			RING_VAL_RANGE.minValue = SmartDashboard::GetNumber("Tote val min", RING_VAL_RANGE.minValue);
			RING_VAL_RANGE.maxValue = SmartDashboard::GetNumber("Tote val max", RING_VAL_RANGE.maxValue);

			//Threshold the image looking for ring light color
			imaqError = imaqColorThreshold(binaryFrame, frame, 255, IMAQ_HSV, &RING_HUE_RANGE, &RING_SAT_RANGE, &RING_VAL_RANGE);

			//Send particle count to dashboard
			int numParticles = 0;
			imaqError = imaqCountParticles(binaryFrame, 1, &numParticles);
			SmartDashboard::PutNumber("Masked particles", numParticles);

			//Send masked image to dashboard to assist in tweaking mask.
			SendToDashboard(binaryFrame, imaqError);

			//filter out small particles
			float areaMin = SmartDashboard::GetNumber("Area min %", AREA_MINIMUM);
			criteria[0] = {IMAQ_MT_AREA_BY_IMAGE_AREA, areaMin, 100, false, false};
			imaqError = imaqParticleFilter4(binaryFrame, binaryFrame, criteria, 1, &filterOptions, NULL, NULL);

			//Send particle count after filtering to dashboard
			imaqError = imaqCountParticles(binaryFrame, 1, &numParticles);
			SmartDashboard::PutNumber("Filtered particles", numParticles);

			if(numParticles > 0) {
				//Measure particles and sort by particle size
				std::vector<ParticleReport> particles;
				for(int particleIndex = 0; particleIndex < numParticles; particleIndex++)
				{
					ParticleReport par;
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_AREA_BY_IMAGE_AREA, &(par.PercentAreaToImageArea));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_AREA, &(par.Area));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_TOP, &(par.BoundingRectTop));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_LEFT, &(par.BoundingRectLeft));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_BOTTOM, &(par.BoundingRectBottom));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_RIGHT, &(par.BoundingRectRight));
					particles.push_back(par);
				}
				sort(particles.begin(), particles.end(), CompareParticleSizes);

				//This example only scores the largest particle. Extending to score all particles and choosing the desired one is left as an exercise
				//for the reader. Note that this scores and reports information about a single particle (single L shaped target). To get accurate information 
				//about the location of the tote (not just the distance) you will need to correlate two adjacent targets in order to find the true center of the tote.
				scores.Aspect = AspectScore(particles.at(0));
				SmartDashboard::PutNumber("Aspect", scores.Aspect);
				scores.Area = AreaScore(particles.at(0));
				SmartDashboard::PutNumber("Area", scores.Area);
				bool isTarget = scores.Area > SCORE_MIN && scores.Aspect > SCORE_MIN;

				//Send distance and tote status to dashboard. The bounding rect, particularly the horizontal center (left - right) may be useful for rotating/driving towards a tote
				SmartDashboard::PutBoolean("IsTarget", isTarget);
				SmartDashboard::PutNumber("Distance", computeDistance(binaryFrame, particles.at(0)));
			} else {
				SmartDashboard::PutBoolean("IsTarget", false);
			}

			Wait(0.005);				// wait for a motor update time
		}
	}

	void OperatorControl() override {
		while(IsOperatorControl() && IsEnabled()) {
			Wait(0.005);				// wait for a motor update time
		}
	}


	//Send image to dashboard if IMAQ has not thrown an error
	void SendToDashboard(Image *image, int error)
	{
		if(error < ERR_SUCCESS) {
			DriverStation::ReportError("Send To Dashboard error: " + std::to_string((long)imaqError) + "\n");
		} else {
			CameraServer::GetInstance()->SetImage(binaryFrame);
		}
	}

	//Comparator function for sorting particles. Returns true if particle 1 is larger
	static bool CompareParticleSizes(ParticleReport particle1, ParticleReport particle2)
	{
		//we want descending sort order
		return particle1.PercentAreaToImageArea > particle2.PercentAreaToImageArea;
	}

	/**
	 * Converts a ratio with ideal value of 1 to a score. The resulting function is piecewise
	 * linear going from (0,0) to (1,100) to (2,0) and is 0 for all inputs outside the range 0-2
	 */
	double ratioToScore(double ratio)
	{
		return (fmax(0, fmin(100*(1-fabs(1-ratio)), 100)));
	}


	double AreaScore(ParticleReport report)
	{
		double boundingArea = (report.BoundingRectBottom - report.BoundingRectTop) * (report.BoundingRectRight - report.BoundingRectLeft);
		//Tape is 7" edge so 49" bounding rect. With 2" wide tape it covers 24" of the rect.
		return ratioToScore((49/24)*report.Area/boundingArea);
	}

	/**
	 * Method to score if the aspect ratio of the particle appears to match the retro-reflective target. Target is 7"x7" so aspect should be 1
	 */
	double AspectScore(ParticleReport report)
	{
		return ratioToScore(((report.BoundingRectRight-report.BoundingRectLeft)/(report.BoundingRectBottom-report.BoundingRectTop)));
	}


	/**
	 * Computes the estimated distance to a target using the width of the particle in the image. For more information and graphics
	 * showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
	 *
	 * @param image The image to use for measuring the particle estimated rectangle
	 * @param report The Particle Analysis Report for the particle
	 * @return The estimated distance to the target in feet.
	 */
	double computeDistance (Image *image, ParticleReport report) {
		double normalizedWidth, targetWidth;
		int xRes, yRes;

		imaqGetImageSize(image, &xRes, &yRes);
		normalizedWidth = 2*(report.BoundingRectRight - report.BoundingRectLeft)/xRes;
		SmartDashboard::PutNumber("Width", normalizedWidth);
		targetWidth = 7;

		return  targetWidth/(normalizedWidth*12*tan(VIEW_ANGLE*M_PI/(180*2)));
	}
};

START_ROBOT_CLASS(VisionRetro2015Sample)
