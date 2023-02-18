/**
 * 范例09 - 模板练习项目
 * Main.cpp
 **/

#include "Application/Win32Application.h"
#include "DXWorkStenciling/DXHelloStenciling.h"
#include "DXWorkStenciling/IMGuiHelloStenciling.h"

/// <summary>
/// 范例09程序入口
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdSize"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdSize)
{
	DXHelloStenciling helloStenciling(L"D3d12 Hello Stenciling", 1280U, 720U);
	Win32Application application(hInstance, &helloStenciling, IMGuiHelloStenciling::GetInstance());
	application.InitializeMainWindow();
	return application.Run();
}