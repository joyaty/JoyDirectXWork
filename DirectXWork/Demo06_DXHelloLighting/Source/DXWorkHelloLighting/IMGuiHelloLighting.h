/**
 * IMGuiHelloLighting.h
 * 光照模型示例程序IMGui调试信息面板头文件
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

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
	void SetWorkHelloLighting(DXHelloLighting* pHelloLighting) { m_HelloLighting = pHelloLighting; }

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloLighting* s_Instance;

private:
	/// <summary>
	/// HelloLighting示例
	/// </summary>
	DXHelloLighting* m_HelloLighting;
};