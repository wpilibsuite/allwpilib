/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Assembly2D.h"

#include <cmath>

#include <GL/gl3w.h>
#include <hal/SimDevice.h>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>
#include <mockdata/SimDeviceData.h>
#include <units/units.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <list>
#include <list>
#include <string>
#include <cmath>

#include "GuiUtil.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"
#include "portable-file-dialogs.h"
#include "../include/HALSimGui.h"
#include "../../../../../../wpiutil/src/main/native/include/wpi/json.h"

using namespace halsimgui;

struct BodyConfig {
    std::string name;
    std::string type;
    int startLocation;
    int length;
    std::string color;
    std::list<BodyConfig> children;
};

int counter = 0;
std::list<BodyConfig> bodyConfigList;

ImColor ColorToIM_COL32(std::string color) {
    if (color == "blue") {
        return IM_COL32(0, 0, 255, 255);
    } else if (color == "green") {
        return IM_COL32(0, 255, 0, 255);
    } else if (color == "red") {
        return IM_COL32(255, 0, 0, 255);
    }
    return IM_COL32(255, 255, 255, 255);
}

std::tuple<float, float, float>
DrawLine(int startXLocation, int startYLocation, int length, double angle, ImDrawList *drawList, ImVec2 windowPos,
         ImColor color) {

    double radAngle = (angle - 90) * 3.14159 / 180;
    double xEnd = startXLocation + length * std::cos(radAngle);
    double yEnd = startYLocation + length * std::sin(radAngle);
    drawList->AddLine(windowPos + ImVec2(startXLocation, startYLocation),
                      windowPos + ImVec2(xEnd, yEnd),
                      color, 1);
    return {xEnd, yEnd, angle};
}

int angleCount = 0;

static void DisplayAssembly2D() {
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddLine(windowPos + ImVec2(ImGui::GetWindowWidth() / 2 + 0,
                                         ImGui::GetWindowHeight()),
                      windowPos + ImVec2(ImGui::GetWindowWidth() / 2 + 0,
                                         ImGui::GetWindowHeight() - 200),
                      IM_COL32(255, 0, 0, 255), 1);
    auto[firstXEnd, firstYEnd, firstAngle] = DrawLine(ImGui::GetWindowWidth() / 2 + 0, ImGui::GetWindowHeight() - 200,
                                                      100,
                                                      angleCount, drawList,
                                                      windowPos, IM_COL32(255, 255, 255, 255));
    auto[secondXEnd, secondYEnd, secondAngle] = DrawLine(firstXEnd, firstYEnd, 100, angleCount + 90 + firstAngle,
                                                         drawList, windowPos, IM_COL32(255, 255, 0, 255));
    DrawLine(secondXEnd, secondYEnd, 200, angleCount + 45 + secondAngle, drawList, windowPos, IM_COL32(0, 255, 0, 255));
    angleCount++;
}

BodyConfig readSubJson(wpi::json const &body) {

    BodyConfig c;
    try {
        c.name = body.at("name").get<std::string>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "could not read body name: " << e.what() << '\n';
    }
    try {
        c.type = body.at("type").get<std::string>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "camera '" << c.name << "': could not type path: " << e.what() << '\n';
    }
    try {
        c.startLocation = body.at("startLocation").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "startLocation '" << c.name << "': could not find startLocation path: " << e.what()
                    << '\n';
    }
    try {
        c.length = body.at("length").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "length '" << c.name << "': could not find length path: " << e.what() << '\n';
    }
    try {
        c.color = body.at("color").get<std::string>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "color '" << c.name << "': could not find color path: " << e.what() << '\n';
    }
    return c;
}

static std::list<BodyConfig> readJson(std::string jFile) {
    std::list<BodyConfig> cList;
    std::error_code ec;
    wpi::raw_fd_istream is(jFile, ec);
    if (ec) {
        wpi::errs() << "could not open '" << jFile << "': " << ec.message() << '\n';
    }

    // parse file
    wpi::json j;
    try {
        j = wpi::json::parse(is);
    } catch (const wpi::json::parse_error &e) {
        wpi::errs() << "byte " << e.byte << ": " << e.what() << '\n';
    }

    // top level must be an object
    if (!j.is_object()) {
        wpi::errs() << "must be JSON object\n";
    }
    try {
        for (wpi::json const &body : j.at("body")) {
            cList.push_back(readSubJson(body));
        }

    } catch (const wpi::json::exception &e) {
        wpi::errs() << "could not read body: " << e.what() << '\n';
    }
    return cList;
}

void Assembly2D::Initialize() {
    // hook ini handler to save settings
    bodyConfigList = readJson("/home/gabe/github/allwpilib/Assembly2D.json");
    ImGuiSettingsHandler iniHandler;
    iniHandler.TypeName = "2D Assembly";
    iniHandler.TypeHash = ImHashStr(iniHandler.TypeName);
    ImGui::GetCurrentContext()->SettingsHandlers.push_back(iniHandler);
    HALSimGui::AddWindow("2D Assembly", DisplayAssembly2D);
    HALSimGui::SetDefaultWindowPos("2D Assembly", 200, 200);
    HALSimGui::SetDefaultWindowSize("2D Assembly", 400, 200);
    HALSimGui::SetWindowPadding("2D Assembly", 0, 0);


    for (BodyConfig const &bodyConfig : bodyConfigList) {
        wpi::outs() << bodyConfig.name << " " << bodyConfig.type << " " << bodyConfig.startLocation << " "
                    << bodyConfig.length << "\n";
    }
}