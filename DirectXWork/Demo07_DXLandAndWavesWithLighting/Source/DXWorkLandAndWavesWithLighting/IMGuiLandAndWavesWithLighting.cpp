/**
 * IMGuiLandAndWavesWithLighting.cpp
 * 带有光照的陆地和波浪实例程序IMGui调试面板代码文件
 */

#include "stdafx.h"
#include "IMGuiLandAndWavesWithLighting.h"
#include <imgui.h>

IMGuiLandAndWavesWithLighting* IMGuiLandAndWavesWithLighting::s_Instance = nullptr;

void IMGuiLandAndWavesWithLighting::OnDrawWindow()
{
	ImGui::Begin("Hello Land And Waves With Lighting!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制带有光照的陆地和波浪程序调试面板

	ImGui::End();
}