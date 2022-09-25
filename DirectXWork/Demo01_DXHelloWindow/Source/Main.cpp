/**
 * DirectX12学习工程入口文件
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
	D3D12HelloWindow helloWindow(1280, 720, L"D3D12 Hello Window");
	IMGuiHelloWindow imGuiWindow;
	return Win32Application::Run(&helloWindow, hInstance, nCmdShow, &imGuiWindow);
}