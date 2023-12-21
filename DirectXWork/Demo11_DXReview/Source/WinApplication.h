
#pragma once

#include "Timer.h"

class DXSampleForReview;
class IMGuiCtrlForReview;

class WinApplication
{
public:
	WinApplication(HINSTANCE hAppInstance, DXSampleForReview* pSampleForReview, IMGuiCtrlForReview* pGuiForReview);

	bool Initialize();
	int Run();

protected:
	/// <summary>
	/// 窗口回调响应处理
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	LRESULT CALLBACK WndProc(HWND hWindowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	/// <summary>
	/// 窗口宽高发生变化
	/// </summary>
	void OnResize();
	/// <summary>
	/// 键盘按键按下
	/// </summary>
	/// <param name="keyCode">键盘Key</param>
	void OnKeyDown(UINT8 keyCode);
	/// <summary>
	/// 键盘按键抬起
	/// </summary>
	/// <param name="keyCode">键盘Key</param>
	void OnKeyUp(UINT8 keyCode);
	/// <summary>
	/// 鼠标按键按下
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseDown(UINT8 keyCode, int x, int y);
	/// <summary>
	/// 鼠标按键抬起
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseUp(UINT8 keyCode, int x, int y);

	/// <summary>
	/// 鼠标移动
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseMove(UINT8 keyCode, int x, int y);

private:
	/// <summary>
	/// 应用句柄，Windows通过WinMain函数传入
	/// </summary>
	HINSTANCE m_AppInstance{};
	/// <summary>
	/// 窗口句柄
	/// </summary>
	HWND m_WndInstance{};

	/// <summary>
	/// 窗口宽度
	/// </summary>
	uint32_t m_Width;
	/// <summary>
	/// 窗口高度
	/// </summary>
	uint32_t m_Height;

	/// <summary>
	/// DX渲染模块
	/// </summary>
	DXSampleForReview* m_pSample;
	/// <summary>
	/// IMGui调试面板模板
	/// </summary>
	IMGuiCtrlForReview* m_pCtrlGUI;
	/// <summary>
	/// 计时器模块
	/// </summary>
	Timer m_Timer;

	/// <summary>
	/// 程序暂停
	/// </summary>
	bool m_IsAppPause{ false };
	/// <summary>
	/// 窗口是否最小化
	/// </summary>
	bool m_IsAppMinimized{ false };
	/// <summary>
	/// 窗口是否最大化
	/// </summary>
	bool m_IsAppMaximized{ false };
	/// <summary>
	/// 是否拖拽大小中
	/// </summary>
	bool m_IsResizing{ false };
};