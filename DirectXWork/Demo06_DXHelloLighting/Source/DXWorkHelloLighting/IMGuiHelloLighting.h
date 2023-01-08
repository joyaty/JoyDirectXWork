/**
 * IMGuiHelloLighting.h
 * 光照模型示例程序IMGui调试信息面板头文件
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"
#include <imgui.h>

 // 强制声明 - 光照模型DX示例
class DXHelloLighting;

/// <summary>
/// 光照模型示例程序IMGui调试信息面板
/// </summary>
class IMGuiHelloLighting : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloLighting* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiHelloLighting();
		}
		return s_Instance;
	}

public:
	void SetWorkHelloLighting(DXHelloLighting* pHelloLighting);

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloLighting* s_Instance;

private:
	/// <summary>
	/// HelloLighting示例
	/// </summary>
	DXHelloLighting* m_HelloLightingDemo;

	/// <summary>
	/// 裁剪模式
	/// </summary>
	D3D12_CULL_MODE m_CullMode{ D3D12_CULL_MODE_BACK };
	/// <summary>
	/// 填充模式
	/// </summary>
	D3D12_FILL_MODE m_FillMode{ D3D12_FILL_MODE_WIREFRAME };

	/// <summary>
	/// 环境光颜色
	/// </summary>
	ImVec4 m_AmbientColor{};
	/// <summary>
	/// 直接光颜色
	/// </summary>
	ImVec4 m_DirectColor{};
};