/**
 * Win32窗口代码封装头文件
 */

#pragma once

class Win32Application
{
public:
	/// <summary>
	/// 运行Win32窗口
	/// </summary>
	/// <param name="hInstance"></param>
	/// <param name="nCmdShow"></param>
	/// <returns></returns>
	static int Run(HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_HWND; }

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
	/// Win32窗口Handle
	/// </summary>
	static HWND m_HWND;
};