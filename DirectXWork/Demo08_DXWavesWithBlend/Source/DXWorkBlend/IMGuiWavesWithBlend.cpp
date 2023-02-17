/**
 * IMGuiWavesWithBlend.cpp
 * Demo08_DXWavesWithBlend示例项目DearIMGui调试面板实现文件
 **/

#include "stdafx.h"
#include "IMGuiWavesWithBlend.h"
#include <imgui.h>
#include "DXWavesWithBlend.h"

IMGuiWavesWithBlend* IMGuiWavesWithBlend::GetInstance()
{
	static IMGuiWavesWithBlend s_Instance{};
	return &s_Instance;
}

void IMGuiWavesWithBlend::SetWavesWithBlendDemo(DXWavesWithBlend* pDemo)
{
	m_WavesWithBlendDemo = pDemo;
}

void IMGuiWavesWithBlend::OnDrawWindow()
{
	ImGui::Begin("Hello Waves With Blend");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制HelloBlend的调试面板

	ImGui::End();
}