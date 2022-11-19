/**
 * 范例01 - 构建Win32窗口和初始化Direct3D 12
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkHelloWindow/D3D12HelloWindow.h"
#include "DXWorkHelloWindow/IMGuiHelloWindow.h"

/// <summary>
/// Window程序入口函数
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	D3D12HelloWindow helloWindow(L"D3D12 Hello Window", 1280, 720);

	Win32Application application(hInstance, &helloWindow, IMGuiHelloWindow::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}