/*
 * IMGuiHelloCube.h
 * DearIMGui - 绘制立方体实例程序参数调试面板调试面板
 */

#pragma once

#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class IMGuiHelloCube : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloCube* GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new IMGuiHelloCube();
		}
		return s_Instance;
	}

protected:
	void OnDrawWindow() override;

private:
	static IMGuiHelloCube* s_Instance;
};