/*
 * 范例05 - 陆地和波浪示例程序
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkHelloLighting/DXHelloLighting.h"
#include "DXWorkHelloLighting/IMGuiHelloLighting.h"

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
	DXHelloLighting helloLighting(L"D3D12 Hello Lighting", 1280U, 720U);

	Win32Application application(hInstance, &helloLighting, IMGuiHelloLighting::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}