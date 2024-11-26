#pragma once

#include "imgui.h"

namespace MainWindow
{
	void RenderUI()
	{
		ImGui::Begin("Hello, World", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		ImGui::Text("This is some useful text.");

        ImGui::End();
	}
}