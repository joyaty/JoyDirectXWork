/**
 * HelloWindow范例的DearIMGui面板头文件 
 */

#pragma once

#include "imgui.h"
#include "DearIMGuiHelper/DearIMGuiBaseHelper.h"

class IMGuiHelloWindow : public DearIMGuiBaseHelper
{
public:
	static IMGuiHelloWindow* GetInstance()
	{
		if (s_IMGUIWindowInstance == nullptr)
		{
			s_IMGUIWindowInstance = new IMGuiHelloWindow();
		}
		return s_IMGUIWindowInstance;
	}

public:
	ImVec4 GetBackColor() const { return m_ClearColor; }

protected:
	virtual void OnDrawWindow() override;

private:
	static IMGuiHelloWindow* s_IMGUIWindowInstance;

private:
	float m_FloatValue = 0.f;
	int m_Counter = 0;
	ImVec4 m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};