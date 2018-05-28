// Example of a custom OpenGL program embedded in ImGui

#include "imgui.h"

bool Custom_CreateDeviceObjects();
void Custom_DeleteDeviceObjects();
void Custom_ImDrawCallback(const ImDrawList* cmd_list, const ImDrawCmd* pcmd);