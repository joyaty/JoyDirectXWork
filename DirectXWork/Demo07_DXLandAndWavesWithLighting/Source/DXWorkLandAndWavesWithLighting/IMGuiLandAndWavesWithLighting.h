/**
 * IMGuiLandAndWavesWithLighting.h
 * 带有光照的陆地和波浪实例程序IMGui调试面板头文件
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class LandAndWavesWithLighting;

/// <summary>
/// 带有光照的陆地和波浪实例程序IMGui调试面板
/// </summary>
class IMGuiLandAndWavesWithLighting : public DearIMGuiBaseHelper
{
public:
	static IMGuiLandAndWavesWithLighting* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiLandAndWavesWithLighting();
		}
		return s_Instance;
	}

public:
	/// <summary>
	/// 设置调试程序实例
	/// </summary>
	/// <param name="pLandAndWavesWithLighting">调试程序实例</param>
	void SetWorkLandAndWaveWithLighting(LandAndWavesWithLighting* pLandAndWavesWithLighting) { m_LandAndWavesWithLighting = pLandAndWavesWithLighting; }

protected:
	void OnDrawWindow() override;

private:
	static IMGuiLandAndWavesWithLighting* s_Instance;

private:
	/// <summary>
	/// 调试程序实例
	/// </summary>
	LandAndWavesWithLighting* m_LandAndWavesWithLighting{ nullptr };
};