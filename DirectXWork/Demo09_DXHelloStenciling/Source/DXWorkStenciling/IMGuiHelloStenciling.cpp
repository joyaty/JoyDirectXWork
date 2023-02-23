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

	// 雾效开关
	bool isChange = ImGui::Checkbox("Enable fog", &m_EnableFog);
	if (isChange)
	{
		m_HelloStencilingDemo->SetEnableFog(m_EnableFog);
	}
	// 镜像物件模板测试开关
	isChange = ImGui::Checkbox("Enable Reflect Stencil", &m_EnableStencil);
	if (isChange)
	{
		m_HelloStencilingDemo->SetEnableStencil(m_EnableStencil);
	}
	// 裁剪模式开关
	static int fillMode = 0;
	if (ImGui::Combo("Fill Mode: ", &fillMode, "D3D12_FILL_MODE_SOLID\0D3D12_FILL_MODE_WIREFRAME\0"))
	{
		switch (fillMode)
		{
		case 0:
			m_FillMode = D3D12_FILL_MODE_SOLID;
			if (m_HelloStencilingDemo != nullptr)
			{
				m_HelloStencilingDemo->SetFillMode(m_FillMode);
			}
			break;
		case 1:
			m_FillMode = D3D12_FILL_MODE_WIREFRAME;
			if (m_HelloStencilingDemo != nullptr)
			{
				m_HelloStencilingDemo->SetFillMode(m_FillMode);
			}
			break;
		}
	}

	ImGui::End();
}