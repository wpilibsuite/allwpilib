#include "TimeManager.h"
#include "imgui.h"

using namespace sapphire;

void TimeManager::Display() {
    static int test;
    ImGui::SliderInt("Test", &test, 0, 100);
}