/*
 * IMGuiHelloTriangle.h
 * DearIMGui - 绘制三角形实例程序参数调试面板调试面板
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class IMGuiHelloTriangle : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloTriangle* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiHelloTriangle();
		}
		return s_Instance;
	}

protected:
	virtual void OnDrawWindow() override;

private:
	static IMGuiHelloTriangle* s_Instance;
};