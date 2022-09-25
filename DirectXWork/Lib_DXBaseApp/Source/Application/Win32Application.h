/**
 * Win32窗口代码封装头文件
 */

#pragma once

#include "GameTimer.h"

class DXBaseWork;
class DearIMGuiBaseHelper;
class DirectXBaseWork;

class Win32Application
{
public:
	Win32Application(HINSTANCE hInstance, DirectXBaseWork* pDirectXWork, DearIMGuiBaseHelper* pDearIMGuiHelper);
	virtual ~Win32Application();

public:
	HINSTANCE GetAppInstance() const { return m_HInstance; }
	HWND GetMainWindow() const { return m_HWND; }
	DirectXBaseWork* GetDirectXWork() const { return m_DirectXWork; }
	DearIMGuiBaseHelper* GetDearIMGuiHelper() const { return m_DearIMGuiHelper; }

public:
	bool InitializeMainWindow();
	int Run();

public:
	/// <summary>
	/// 键盘按下事件
	/// </summary>
	/// <param name="keyCode">按键KeyCode</param>
	void OnKeyDown(UINT8 keyCode);
	/// <summary>
	/// 键盘抬起事件
	/// </summary>
	/// <param name="keyCode">按键KeyCode</param>
	void OnKeyUp(UINT8 keyCode);

	/// <summary>
	/// 鼠标按下事件
	/// </summary>
	/// <param name="kCode">鼠标KeyCode</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseDown(UINT8 kCode, int x, int y);
	/// <summary>
	/// 鼠标抬起事件
	/// </summary>
	/// <param name="keyCode">鼠标KeyCode</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseUp(UINT8 keyCode, int x, int y);
	/// <summary>
	/// 鼠标移动事件
	/// </summary>
	/// <param name="keyCode">鼠标KeyCode</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseMove(UINT8 keyCode, int x, int y);

	/// <summary>
	/// 界面宽高变更
	/// </summary>
	void OnResize();
	
public:
	/// <summary>
	/// 处理Windows窗口消息
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	LRESULT CALLBACK WnMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
	/// <summary>
	/// 应用程序句柄
	/// </summary>
	HINSTANCE m_HInstance{ nullptr };
	/// <summary>
	/// 窗口句柄
	/// </summary>
	HWND m_HWND{ nullptr };

	/// <summary>
	/// 窗口宽度
	/// </summary>
	UINT m_Width;
	/// <summary>
	/// 窗口高度
	/// </summary>
	UINT m_Height;

	/// <summary>
	/// Application是否暂停
	/// </summary>
	bool m_AppPaused{ false };
	/// <summary>
	/// Application是否最小化
	/// </summary>
	bool m_AppMinimized{ false };
	/// <summary>
	/// Application是否最大化
	/// </summary>
	bool m_AppMaximized{ false };
	/// <summary>
	/// Application是否拖拽大小中
	/// </summary>
	bool m_Resizing{ false };

	/// <summary>
	/// 本地DirectX渲染模块
	/// </summary>
	DirectXBaseWork* m_DirectXWork;

	/// <summary>
	/// DearIMGui窗口模块
	/// </summary>
	DearIMGuiBaseHelper* m_DearIMGuiHelper;

	/// <summary>
	/// 计时器模块
	/// </summary>
	GameTimer m_GameTimer;
};