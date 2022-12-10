/**
 * IMGuiLandAndWaves.h
 * 陆地与波浪示例程序IMGui调试信息面板头文件声明
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

// 前置声明
class D3D12LandAndWaves;

/// <summary>
/// 陆地与波浪示例程序IMGui调试信息面板
/// </summary>
class IMGuiLandAndWaves : public DearIMGuiBaseHelper
{
public:
	static IMGuiLandAndWaves* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiLandAndWaves();
		}
		return s_Instance;
	}

public:
	void SetHelloShapesDemo(D3D12LandAndWaves* pHelloShapesDemo) { m_pLandAndWavesDemo = pHelloShapesDemo; }

protected:
	void OnDrawWindow() override;

private:
	static IMGuiLandAndWaves* s_Instance;

private:
	/// <summary>
	/// HelloShapes示例
	/// </summary>
	D3D12LandAndWaves* m_pLandAndWavesDemo;
};