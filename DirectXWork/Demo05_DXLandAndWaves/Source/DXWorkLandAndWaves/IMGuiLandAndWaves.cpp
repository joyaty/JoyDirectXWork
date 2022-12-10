/**
 * IMGuiLandAndWaves.cpp
 * 陆地与波浪示例程序IMGui调试信息面板实现文件
 */

#include "stdafx.h"
#include "IMGuiLandAndWaves.h"
#include "D3D12LandAndWaves.h"
#include <imgui.h>

IMGuiLandAndWaves* IMGuiLandAndWaves::s_Instance{ nullptr };

void IMGuiLandAndWaves::OnDrawWindow()
{
	ImGui::Begin("Hello Land And Waves!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制陆地和波浪调试面板

	ImGui::End();
}