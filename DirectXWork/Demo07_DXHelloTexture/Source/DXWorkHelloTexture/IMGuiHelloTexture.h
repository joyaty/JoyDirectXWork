/**
 * IMGuiHelloTexture.h
 * HelloTexture示例程序调试面板头文件
 **/

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

// 前置声明 - DXHelloTexture，避免DXHelloTexture.h和IMGuiHelloTexture.h相互引用
class DXHelloTexture;

/// <summary>
/// HelloTexture示例程序的调试面板
/// </summary>
class IMGuiHelloTexture : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloTexture* GetInstance();

public:
	void SetHelloTextureDemo(DXHelloTexture* pHelloTextureDemo) { m_HelloTextureDemo = pHelloTextureDemo; }

protected:
	void OnDrawWindow() override;

private:
	DXHelloTexture* m_HelloTextureDemo{ nullptr };
};