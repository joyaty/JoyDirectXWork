/*
 * IMGuiHelloShapes.h
 * DearIMGui - 绘制多个几何体实例程序参数调试面板调试面板
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class D3D12HelloShapes;

class IMGuiHelloShapes : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloShapes* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiHelloShapes();
		}
		return s_Instance;
	}

public:
	void SetHelloShapesDemo(D3D12HelloShapes* pHelloShapesDemo) { m_pHelloShapesDemo = pHelloShapesDemo; }

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloShapes* s_Instance;

private:
	/// <summary>
	/// HelloShapes示例
	/// </summary>
	D3D12HelloShapes* m_pHelloShapesDemo;

	/// <summary>
	/// 裁剪模式
	/// </summary>
	D3D12_CULL_MODE m_CullMode{ D3D12_CULL_MODE_BACK };
	/// <summary>
	/// 填充模式
	/// </summary>
	D3D12_FILL_MODE m_FillMode{ D3D12_FILL_MODE_WIREFRAME };
};