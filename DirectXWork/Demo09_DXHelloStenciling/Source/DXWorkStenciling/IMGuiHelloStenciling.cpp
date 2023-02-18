/**
 * IMGuiHelloStenciling.cpp
 * Demo09_DXHelloStenciling练习项目调试面板实现文件
 **/

#include "stdafx.h"
#include "IMGuiHelloStenciling.h"
#include "DXHelloStenciling.h"
#include <imgui.h>

void IMGuiHelloStenciling::SetHelloStencilingDemo(DXHelloStenciling* pDemo)
{
	m_HelloStencilingDemo = pDemo;
}

void IMGuiHelloStenciling::OnDrawWindow()
{
	ImGui::Begin("Hello Stenciling");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}