/**
 * IMGuiHelloGeometryShader.h
 * Demo10_DXHelloGeometryShader练习项目调试面板
 **/

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class DXHelloGeometryShader;

class IMGuiHelloGeometryShader : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloGeometryShader* GetInstance()
	{
		static IMGuiHelloGeometryShader s_Instance{};
		return &s_Instance;
	}

public:
	void SetHelloGeometryShaderDemo(DXHelloGeometryShader* pDemo);

protected:
	void OnDrawWindow() override;

private:
	DXHelloGeometryShader* m_HelloGeometryShaderDemo{ nullptr };
};