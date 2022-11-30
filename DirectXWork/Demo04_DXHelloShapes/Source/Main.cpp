/*
 * 范例04 - 绘制一个几何体入口
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXHelloShapes/D3D12HelloShapes.h"
#include "DXHelloShapes/IMGuiHelloShapes.h"

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
	D3D12HelloShapes helloShapes(L"D3D12 Hello Shapes", 1280U, 720U);

	Win32Application application(hInstance, &helloShapes, IMGuiHelloShapes::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}