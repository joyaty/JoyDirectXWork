/**
 * IMGuiHelloStenciling.h
 * Demo09_DXHelloStenciling练习项目调试面板
 **/

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

// 前置申明模板练习项目，避免与DXHelloStenciling文件循环依赖
class DXHelloStenciling;

/// <summary>
/// 模板练习项目的自定义调试面板
/// </summary>
class IMGuiHelloStenciling : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloStenciling* GetInstance()
	{
		static IMGuiHelloStenciling s_Instance{};
		return &s_Instance;
	}

public:
	void SetHelloStencilingDemo(DXHelloStenciling* pDemo);
	bool GetEnableFog() const { return m_EnableFog; }
	D3D12_FILL_MODE GetFillMode() const { return m_FillMode; }

protected:
	void OnDrawWindow() override;

private:
	DXHelloStenciling* m_HelloStencilingDemo{ nullptr };
	bool m_EnableFog{ true };
	D3D12_FILL_MODE m_FillMode{ D3D12_FILL_MODE_SOLID };
};