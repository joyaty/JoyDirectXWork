/*
 * IMGuiHelloShapes.cpp
 * DearIMGui - 绘制多个几何体实例程序参数调试面板调试面板
 */

#include "stdafx.h"
#include "IMGuiHelloShapes.h"
#include "imgui.h"
#include "D3D12HelloShapes.h"

IMGuiHelloShapes* IMGuiHelloShapes::s_Instance = nullptr;

void IMGuiHelloShapes::OnDrawWindow()
{
	ImGui::Begin("Hello Shapes!");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// 绘制示例项目参数调试面板
	static int cullMode = -1;
	if (ImGui::Combo("Cull Mode", &cullMode, "D3D12_CULL_MODE_BACK\0D3D12_CULL_MODE_FRONT\0D3D12_CULL_MODE_NONE\0"))
	{
		switch (cullMode)
		{
		case 0:
			m_CullMode = D3D12_CULL_MODE_BACK;
			if (m_pHelloShapesDemo != nullptr)
			{
				m_pHelloShapesDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 1:
			m_CullMode = D3D12_CULL_MODE_FRONT;
			if (m_pHelloShapesDemo != nullptr)
			{
				m_pHelloShapesDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 2:
			m_CullMode = D3D12_CULL_MODE_NONE;
			if (m_pHelloShapesDemo != nullptr)
			{
				m_pHelloShapesDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		}
	}

	static int fillMode = -1;
	if (ImGui::Combo("Fill Mode: ", &fillMode, "D3D12_FILL_MODE_SOLID\0D3D12_FILL_MODE_WIREFRAME\0"))
	{
		switch (fillMode)
		{
		case 0:
			m_FillMode = D3D12_FILL_MODE_SOLID;
			if (m_pHelloShapesDemo != nullptr)
			{
				m_pHelloShapesDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 1:
			m_FillMode = D3D12_FILL_MODE_WIREFRAME;
			if (m_pHelloShapesDemo != nullptr)
			{
				m_pHelloShapesDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		}
	}

	ImGui::End();
}