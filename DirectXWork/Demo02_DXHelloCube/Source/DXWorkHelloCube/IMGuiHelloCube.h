/*
 * IMGuiHelloCube.h
 * DearIMGui - 绘制立方体实例程序参数调试面板调试面板
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class D3D12HelloCube;

class IMGuiHelloCube : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloCube* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiHelloCube();
		}
		return s_Instance;
	}

public:
	void SetHelloCubeDemo(D3D12HelloCube* pHelloCubeDemo) { m_pHelloCubeDemo = pHelloCubeDemo; }

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloCube* s_Instance;

private:
	/// <summary>
	/// HelloCube示例
	/// </summary>
	D3D12HelloCube* m_pHelloCubeDemo;

	/// <summary>
	/// 裁剪模式
	/// </summary>
	D3D12_CULL_MODE m_CullMode{ D3D12_CULL_MODE_BACK };
	/// <summary>
	/// 填充模式
	/// </summary>
	D3D12_FILL_MODE m_FillMode{ D3D12_FILL_MODE_SOLID };
};