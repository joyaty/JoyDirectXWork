/*
 * 范例02 - 绘制一个立方体入口
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkHelloCube/D3D12HelloCube.h"
#include "DXWorkHelloCube/IMGuiHelloCube.h"

/// <summary>
/// Win32入口Main函数
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	D3D12HelloCube helloCube(L"D3D12 Hello Cube", 1280U, 720U);

	Win32Application application(hInstance, &helloCube, IMGuiHelloCube::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}