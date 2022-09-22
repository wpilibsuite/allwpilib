#include "TimeManager.h"
#include <imgui.h>

using namespace sapphire;

void TimeManager::Display() {
    ImGui::SliderInt("Test", &test, 0, 100);
}