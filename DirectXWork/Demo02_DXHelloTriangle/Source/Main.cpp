/*
 * 范例02 - 绘制一个三角形 入口
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkHelloTriangle/D3D12HelloTriangle.h"
#include "DXWorkHelloTriangle/IMGuiHelloTriangle.h"

/// <summary>
/// Win32入口Main函数
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPreInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	D3D12HelloTriangle helloTriangle(L"D3D12 Hello Triangle", 1280U, 720U);
	Win32Application application(hInstance, &helloTriangle, IMGuiHelloTriangle::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}