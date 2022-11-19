/*
 * IMGuiHelloCube.cpp
 * DearIMGui - 绘制立方体实例程序参数调试面板调试面板
 */

#include "stdafx.h"
#include "IMGuiHelloCube.h"
#include "imgui.h"

IMGuiHelloCube* IMGuiHelloCube::s_Instance = nullptr;

void IMGuiHelloCube::OnDrawWindow()
{
	ImGui::Begin("Hello Cube!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}