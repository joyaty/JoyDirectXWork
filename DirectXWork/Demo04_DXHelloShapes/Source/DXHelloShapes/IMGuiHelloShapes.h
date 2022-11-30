/*
 * IMGuiHelloShapes.h
 * DearIMGui - 绘制多个几何体实例程序参数调试面板调试面板
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

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

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloShapes* s_Instance;
};