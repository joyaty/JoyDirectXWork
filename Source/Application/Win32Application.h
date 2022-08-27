/**
 * Win32���ڴ����װͷ�ļ�
 */

#pragma once

class Win32Application
{
public:
	/// <summary>
	/// ����Win32����
	/// </summary>
	/// <param name="hInstance"></param>
	/// <param name="nCmdShow"></param>
	/// <returns></returns>
	static int Run(HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_HWND; }

protected:
	/// <summary>
	/// ���ڻص���Ӧ����
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	/// <summary>
	/// Win32����Handle
	/// </summary>
	static HWND m_HWND;
};