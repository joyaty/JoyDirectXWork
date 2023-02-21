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
	m_WavesWithBlendDemo->SetBlendFactor(m_BlendFactor[0], m_BlendFactor[1], m_BlendFactor[2], m_BlendFactor[3]);
}

void IMGuiWavesWithBlend::OnDrawWindow()
{
	ImGui::Begin("Hello Waves With Blend");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// 雾效开关
	bool isChange = ImGui::Checkbox("Enable fog", &m_EnableFog);
	if (isChange)
	{
		m_WavesWithBlendDemo->SwitchFogState(m_EnableFog);
	}
	// 混合因子设置
	isChange = ImGui::SliderFloat3("Blend factor", m_BlendFactor, 0.f, 1.f);
	if (isChange)
	{
		m_WavesWithBlendDemo->SetBlendFactor(m_BlendFactor[0], m_BlendFactor[1], m_BlendFactor[2], m_BlendFactor[3]);
	}

	static int fillMode = 0;
	if (ImGui::Combo("Fill Mode: ", &fillMode, "D3D12_FILL_MODE_SOLID\0D3D12_FILL_MODE_WIREFRAME\0"))
	{
		switch (fillMode)
		{
		case 0:
			m_FillMode = D3D12_FILL_MODE_SOLID;
			if (m_WavesWithBlendDemo != nullptr)
			{
				m_WavesWithBlendDemo->SetFillMode(m_FillMode);
			}
			break;
		case 1:
			m_FillMode = D3D12_FILL_MODE_WIREFRAME;
			if (m_WavesWithBlendDemo != nullptr)
			{
				m_WavesWithBlendDemo->SetFillMode(m_FillMode);
			}
			break;
		}
	}

	ImGui::End();
}