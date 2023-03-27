/**
 * IMGuiHelloGeometryShader.cpp
 * Demo10_DXHelloGeometryShader练习项目调试面板
 **/

#include "stdafx.h"
#include "IMGuiHelloGeometryShader.h"
#include "DXHelloGeometryShader.h"
#include <imgui.h>

void IMGuiHelloGeometryShader::SetHelloGeometryShaderDemo(DXHelloGeometryShader* pDemo)
{
	m_HelloGeometryShaderDemo = pDemo;
}

void IMGuiHelloGeometryShader::OnDrawWindow()
{
	ImGui::Begin("Hello Stenciling");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// TODO 绘制几何着色器示例程序的调试面板

	ImGui::End();
}