/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/util/Color.h"

using namespace frc;

/*
 * FIRST Colors
 */
constexpr Color Color::kDenim{0.0823529412, 0.376470589, 0.7411764706};
constexpr Color Color::kFirstBlue{0.0, 0.4, 0.7019607844};
constexpr Color Color::kFirstRed{0.9294117648, 0.1098039216, 0.1411764706};

/*
 * Standard Colors
 */
constexpr Color Color::kAliceBlue{0.9411765f, 0.972549f, 1.0f};
constexpr Color Color::kAntiqueWhite{0.98039216f, 0.92156863f, 0.84313726f};
constexpr Color Color::kAqua{0.0f, 1.0f, 1.0f};
constexpr Color Color::kAquamarine{0.49803922f, 1.0f, 0.83137256f};
constexpr Color Color::kAzure{0.9411765f, 1.0f, 1.0f};
constexpr Color Color::kBeige{0.9607843f, 0.9607843f, 0.8627451f};
constexpr Color Color::kBisque{1.0f, 0.89411765f, 0.76862746f};
constexpr Color Color::kBlack{0.0f, 0.0f, 0.0f};
constexpr Color Color::kBlanchedAlmond{1.0f, 0.92156863f, 0.8039216f};
constexpr Color Color::kBlue{0.0f, 0.0f, 1.0f};
constexpr Color Color::kBlueViolet{0.5411765f, 0.16862746f, 0.8862745f};
constexpr Color Color::kBrown{0.64705884f, 0.16470589f, 0.16470589f};
constexpr Color Color::kBurlywood{0.87058824f, 0.72156864f, 0.5294118f};
constexpr Color Color::kCadetBlue{0.37254903f, 0.61960787f, 0.627451f};
constexpr Color Color::kChartreuse{0.49803922f, 1.0f, 0.0f};
constexpr Color Color::kChocolate{0.8235294f, 0.4117647f, 0.11764706f};
constexpr Color Color::kCoral{1.0f, 0.49803922f, 0.3137255f};
constexpr Color Color::kCornflowerBlue{0.39215687f, 0.58431375f, 0.92941177f};
constexpr Color Color::kCornsilk{1.0f, 0.972549f, 0.8627451f};
constexpr Color Color::kCrimson{0.8627451f, 0.078431375f, 0.23529412f};
constexpr Color Color::kCyan{0.0f, 1.0f, 1.0f};
constexpr Color Color::kDarkBlue{0.0f, 0.0f, 0.54509807f};
constexpr Color Color::kDarkCyan{0.0f, 0.54509807f, 0.54509807f};
constexpr Color Color::kDarkGoldenrod{0.72156864f, 0.5254902f, 0.043137256f};
constexpr Color Color::kDarkGray{0.6627451f, 0.6627451f, 0.6627451f};
constexpr Color Color::kDarkGreen{0.0f, 0.39215687f, 0.0f};
constexpr Color Color::kDarkKhaki{0.7411765f, 0.7176471f, 0.41960785f};
constexpr Color Color::kDarkMagenta{0.54509807f, 0.0f, 0.54509807f};
constexpr Color Color::kDarkOliveGreen{0.33333334f, 0.41960785f, 0.18431373f};
constexpr Color Color::kDarkOrange{1.0f, 0.54901963f, 0.0f};
constexpr Color Color::kDarkOrchid{0.6f, 0.19607843f, 0.8f};
constexpr Color Color::kDarkRed{0.54509807f, 0.0f, 0.0f};
constexpr Color Color::kDarkSalmon{0.9137255f, 0.5882353f, 0.47843137f};
constexpr Color Color::kDarkSeaGreen{0.56078434f, 0.7372549f, 0.56078434f};
constexpr Color Color::kDarkSlateBlue{0.28235295f, 0.23921569f, 0.54509807f};
constexpr Color Color::kDarkSlateGray{0.18431373f, 0.30980393f, 0.30980393f};
constexpr Color Color::kDarkTurquoise{0.0f, 0.80784315f, 0.81960785f};
constexpr Color Color::kDarkViolet{0.5803922f, 0.0f, 0.827451f};
constexpr Color Color::kDeepPink{1.0f, 0.078431375f, 0.5764706f};
constexpr Color Color::kDeepSkyBlue{0.0f, 0.7490196f, 1.0f};
constexpr Color Color::kDimGray{0.4117647f, 0.4117647f, 0.4117647f};
constexpr Color Color::kDodgerBlue{0.11764706f, 0.5647059f, 1.0f};
constexpr Color Color::kFirebrick{0.69803923f, 0.13333334f, 0.13333334f};
constexpr Color Color::kFloralWhite{1.0f, 0.98039216f, 0.9411765f};
constexpr Color Color::kForestGreen{0.13333334f, 0.54509807f, 0.13333334f};
constexpr Color Color::kFuchsia{1.0f, 0.0f, 1.0f};
constexpr Color Color::kGainsboro{0.8627451f, 0.8627451f, 0.8627451f};
constexpr Color Color::kGhostWhite{0.972549f, 0.972549f, 1.0f};
constexpr Color Color::kGold{1.0f, 0.84313726f, 0.0f};
constexpr Color Color::kGoldenrod{0.85490197f, 0.64705884f, 0.1254902f};
constexpr Color Color::kGray{0.5019608f, 0.5019608f, 0.5019608f};
constexpr Color Color::kGreen{0.0f, 0.5019608f, 0.0f};
constexpr Color Color::kGreenYellow{0.6784314f, 1.0f, 0.18431373f};
constexpr Color Color::kHoneydew{0.9411765f, 1.0f, 0.9411765f};
constexpr Color Color::kHotPink{1.0f, 0.4117647f, 0.7058824f};
constexpr Color Color::kIndianRed{0.8039216f, 0.36078432f, 0.36078432f};
constexpr Color Color::kIndigo{0.29411766f, 0.0f, 0.50980395f};
constexpr Color Color::kIvory{1.0f, 1.0f, 0.9411765f};
constexpr Color Color::kKhaki{0.9411765f, 0.9019608f, 0.54901963f};
constexpr Color Color::kLavender{0.9019608f, 0.9019608f, 0.98039216f};
constexpr Color Color::kLavenderBlush{1.0f, 0.9411765f, 0.9607843f};
constexpr Color Color::kLawnGreen{0.4862745f, 0.9882353f, 0.0f};
constexpr Color Color::kLemonChiffon{1.0f, 0.98039216f, 0.8039216f};
constexpr Color Color::kLightBlue{0.6784314f, 0.84705883f, 0.9019608f};
constexpr Color Color::kLightCoral{0.9411765f, 0.5019608f, 0.5019608f};
constexpr Color Color::kLightCyan{0.8784314f, 1.0f, 1.0f};
constexpr Color Color::kLightGoldenrodYellow{0.98039216f, 0.98039216f,
                                             0.8235294f};
constexpr Color Color::kLightGray{0.827451f, 0.827451f, 0.827451f};
constexpr Color Color::kLightGreen{0.5647059f, 0.93333334f, 0.5647059f};
constexpr Color Color::kLightPink{1.0f, 0.7137255f, 0.75686276f};
constexpr Color Color::kLightSalmon{1.0f, 0.627451f, 0.47843137f};
constexpr Color Color::kLightSeagGeen{0.1254902f, 0.69803923f, 0.6666667f};
constexpr Color Color::kLightSkyBlue{0.5294118f, 0.80784315f, 0.98039216f};
constexpr Color Color::kLightSlateGray{0.46666667f, 0.53333336f, 0.6f};
constexpr Color Color::kLightSteellue{0.6901961f, 0.76862746f, 0.87058824f};
constexpr Color Color::kLightYellow{1.0f, 1.0f, 0.8784314f};
constexpr Color Color::kLime{0.0f, 1.0f, 0.0f};
constexpr Color Color::kLimeGreen{0.19607843f, 0.8039216f, 0.19607843f};
constexpr Color Color::kLinen{0.98039216f, 0.9411765f, 0.9019608f};
constexpr Color Color::kMagenta{1.0f, 0.0f, 1.0f};
constexpr Color Color::kMaroon{0.5019608f, 0.0f, 0.0f};
constexpr Color Color::kMediumAquamarine{0.4f, 0.8039216f, 0.6666667f};
constexpr Color Color::kMediumBlue{0.0f, 0.0f, 0.8039216f};
constexpr Color Color::kMediumOrchid{0.7294118f, 0.33333334f, 0.827451f};
constexpr Color Color::kMediumPurple{0.5764706f, 0.4392157f, 0.85882354f};
constexpr Color Color::kMediumSeaGreen{0.23529412f, 0.7019608f, 0.44313726f};
constexpr Color Color::kMediumSlateBlue{0.48235294f, 0.40784314f, 0.93333334f};
constexpr Color Color::kMediumSpringGreen{0.0f, 0.98039216f, 0.6039216f};
constexpr Color Color::kMediumTurquoise{0.28235295f, 0.81960785f, 0.8f};
constexpr Color Color::kMediumVioletRed{0.78039217f, 0.08235294f, 0.52156866f};
constexpr Color Color::kMidnightBlue{0.09803922f, 0.09803922f, 0.4392157f};
constexpr Color Color::kMintcream{0.9607843f, 1.0f, 0.98039216f};
constexpr Color Color::kMistyRose{1.0f, 0.89411765f, 0.88235295f};
constexpr Color Color::kMoccasin{1.0f, 0.89411765f, 0.70980394f};
constexpr Color Color::kNavajoWhite{1.0f, 0.87058824f, 0.6784314f};
constexpr Color Color::kNavy{0.0f, 0.0f, 0.5019608f};
constexpr Color Color::kOldLace{0.99215686f, 0.9607843f, 0.9019608f};
constexpr Color Color::kOlive{0.5019608f, 0.5019608f, 0.0f};
constexpr Color Color::kOliveDrab{0.41960785f, 0.5568628f, 0.13725491f};
constexpr Color Color::kOrange{1.0f, 0.64705884f, 0.0f};
constexpr Color Color::kOrangeRed{1.0f, 0.27058825f, 0.0f};
constexpr Color Color::kOrchid{0.85490197f, 0.4392157f, 0.8392157f};
constexpr Color Color::kPaleGoldenrod{0.93333334f, 0.9098039f, 0.6666667f};
constexpr Color Color::kPaleGreen{0.59607846f, 0.9843137f, 0.59607846f};
constexpr Color Color::kPaleTurquoise{0.6862745f, 0.93333334f, 0.93333334f};
constexpr Color Color::kPaleVioletRed{0.85882354f, 0.4392157f, 0.5764706f};
constexpr Color Color::kPapayaWhip{1.0f, 0.9372549f, 0.8352941f};
constexpr Color Color::kPeachPuff{1.0f, 0.85490197f, 0.7254902f};
constexpr Color Color::kPeru{0.8039216f, 0.52156866f, 0.24705882f};
constexpr Color Color::kPink{1.0f, 0.7529412f, 0.79607844f};
constexpr Color Color::kPlum{0.8666667f, 0.627451f, 0.8666667f};
constexpr Color Color::kPowderBlue{0.6901961f, 0.8784314f, 0.9019608f};
constexpr Color Color::kPurple{0.5019608f, 0.0f, 0.5019608f};
constexpr Color Color::kRed{1.0f, 0.0f, 0.0f};
constexpr Color Color::kRosyBrown{0.7372549f, 0.56078434f, 0.56078434f};
constexpr Color Color::kRoyalBlue{0.25490198f, 0.4117647f, 0.88235295f};
constexpr Color Color::kSaddleBrown{0.54509807f, 0.27058825f, 0.07450981f};
constexpr Color Color::kSalmon{0.98039216f, 0.5019608f, 0.44705883f};
constexpr Color Color::kSandyBrown{0.95686275f, 0.6431373f, 0.3764706f};
constexpr Color Color::kSeaGreen{0.18039216f, 0.54509807f, 0.34117648f};
constexpr Color Color::kSeashell{1.0f, 0.9607843f, 0.93333334f};
constexpr Color Color::kSienna{0.627451f, 0.32156864f, 0.1764706f};
constexpr Color Color::kSilver{0.7529412f, 0.7529412f, 0.7529412f};
constexpr Color Color::kSkyBlue{0.5294118f, 0.80784315f, 0.92156863f};
constexpr Color Color::kSlateBlue{0.41568628f, 0.3529412f, 0.8039216f};
constexpr Color Color::kSlateGray{0.4392157f, 0.5019608f, 0.5647059f};
constexpr Color Color::kSnow{1.0f, 0.98039216f, 0.98039216f};
constexpr Color Color::kSpringGreen{0.0f, 1.0f, 0.49803922f};
constexpr Color Color::kSteelBlue{0.27450982f, 0.50980395f, 0.7058824f};
constexpr Color Color::kTan{0.8235294f, 0.7058824f, 0.54901963f};
constexpr Color Color::kTeal{0.0f, 0.5019608f, 0.5019608f};
constexpr Color Color::kThistle{0.84705883f, 0.7490196f, 0.84705883f};
constexpr Color Color::kTomato{1.0f, 0.3882353f, 0.2784314f};
constexpr Color Color::kTurquoise{0.2509804f, 0.8784314f, 0.8156863f};
constexpr Color Color::kViolet{0.93333334f, 0.50980395f, 0.93333334f};
constexpr Color Color::kWheat{0.9607843f, 0.87058824f, 0.7019608f};
constexpr Color Color::kWhite{1.0f, 1.0f, 1.0f};
constexpr Color Color::kWhiteSmoke{0.9607843f, 0.9607843f, 0.9607843f};
constexpr Color Color::kYellow{1.0f, 1.0f, 0.0f};
constexpr Color Color::kYellowGreen{0.6039216f, 0.8039216f, 0.19607843f};

constexpr Color::Color(Color8Bit color)
    : red(round(color.red / 255)), green(round(color.green / 255)), blue(round(color.blue / 255)) {}
