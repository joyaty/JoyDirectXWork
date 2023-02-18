/**
 * IMGuiWavesWithBlend.h
 * Demo08_DXWavesWithBlend示例项目DearIMGui调试面板头文件
 **/

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

// 避免循环依赖，这里使用前置申明
class DXWavesWithBlend;

/// <summary>
/// Demo08_DXWavesWithBlend示例项目DearIMGui调试面板
/// </summary>
class IMGuiWavesWithBlend : public DearIMGuiBaseHelper
{
public:
	static IMGuiWavesWithBlend* GetInstance();

public:
	void SetWavesWithBlendDemo(DXWavesWithBlend* pDemo);

	bool GetEnableFrog() const { return m_EnableFrog; }

protected:
	void OnDrawWindow() override;

private:
	DXWavesWithBlend* m_WavesWithBlendDemo{ nullptr };
	bool m_EnableFrog{ true };
	float m_BlendFactor[4]{ 0.4f, 0.4f, 0.4f, 1.f };
};