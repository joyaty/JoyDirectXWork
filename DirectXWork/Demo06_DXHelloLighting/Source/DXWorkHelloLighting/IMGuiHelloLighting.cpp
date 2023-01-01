/**
 * IMGuiHelloLighting.cpp
 * 光照模型示例程序IMGui调试信息面板代码文件
 */

#include "stdafx.h"
#include "IMGuiHelloLighting.h"
#include "DXHelloLighting.h"
#include <imgui.h>

IMGuiHelloLighting* IMGuiHelloLighting::s_Instance = nullptr;

void IMGuiHelloLighting::OnDrawWindow()
{
	ImGui::Begin("Hello Lighting");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 光照模型调试面板

	ImGui::End();
}