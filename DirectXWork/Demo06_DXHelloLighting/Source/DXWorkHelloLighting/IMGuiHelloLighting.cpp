/**
 * IMGuiHelloLighting.cpp
 * 光照模型示例程序IMGui调试信息面板代码文件
 */

#include "stdafx.h"
#include "IMGuiHelloLighting.h"
#include "DXHelloLighting.h"
#include <imgui.h>

IMGuiHelloLighting* IMGuiHelloLighting::s_Instance = nullptr;

void IMGuiHelloLighting::SetWorkHelloLighting(DXHelloLighting* pHelloLighting)
{
	m_HelloLightingDemo = pHelloLighting;
	// 获取初始环境光颜色
	const DirectX::XMFLOAT4& ambientLight = pHelloLighting->GetAmbientLight();
	m_AmbientColor = ImVec4(ambientLight.x, ambientLight.y, ambientLight.z, ambientLight.w);
	// 获取初始直接光颜色
	const DirectX::XMFLOAT3& directLight = pHelloLighting->GetDirectLight();
	m_DirectColor = ImVec4(directLight.x, directLight.y, directLight.z, 1.f);
}

void IMGuiHelloLighting::OnDrawWindow()
{
	ImGui::Begin("Hello Lighting");
	DrawAdapterInfo();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// 光照模型调试面板
	static int cullMode = 0;
	if (ImGui::Combo("Cull Mode", &cullMode, "D3D12_CULL_MODE_BACK\0D3D12_CULL_MODE_FRONT\0D3D12_CULL_MODE_NONE\0"))
	{
		switch (cullMode)
		{
		case 0:
			m_CullMode = D3D12_CULL_MODE_BACK;
			if (m_HelloLightingDemo != nullptr)
			{
				m_HelloLightingDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 1:
			m_CullMode = D3D12_CULL_MODE_FRONT;
			if (m_HelloLightingDemo != nullptr)
			{
				m_HelloLightingDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 2:
			m_CullMode = D3D12_CULL_MODE_NONE;
			if (m_HelloLightingDemo != nullptr)
			{
				m_HelloLightingDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		}
	}

	static int fillMode = 0;
	if (ImGui::Combo("Fill Mode: ", &fillMode, "D3D12_FILL_MODE_SOLID\0D3D12_FILL_MODE_WIREFRAME\0"))
	{
		switch (fillMode)
		{
		case 0:
			m_FillMode = D3D12_FILL_MODE_SOLID;
			if (m_HelloLightingDemo != nullptr)
			{
				m_HelloLightingDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		case 1:
			m_FillMode = D3D12_FILL_MODE_WIREFRAME;
			if (m_HelloLightingDemo != nullptr)
			{
				m_HelloLightingDemo->UpdatePSO(m_CullMode, m_FillMode);
			}
			break;
		}
	}

	if (ImGui::ColorEdit4("Ambient Color", (float*)&m_AmbientColor))
	{
		DirectX::XMFLOAT4 ambientLight = DirectX::XMFLOAT4(m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z, m_AmbientColor.w);
		m_HelloLightingDemo->SetAmbientLight(ambientLight);
	}

	if (ImGui::ColorEdit3("Direct Color", (float*)&m_DirectColor))
	{
		DirectX::XMFLOAT3 directLight = DirectX::XMFLOAT3(m_DirectColor.x, m_DirectColor.y, m_DirectColor.z);
		m_HelloLightingDemo->SetDirectLight(directLight);
	}

	ImGui::End();
}