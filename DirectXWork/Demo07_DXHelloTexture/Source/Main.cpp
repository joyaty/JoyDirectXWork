/*
 * 范例07 - HelloTexture 纹理贴图示例程序
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkHelloTexture/DXHelloTexture.h"
#include "DXWorkHelloTexture/IMGuiHelloTexture.h"

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
	DXHelloTexture helloLighting(L"D3D12 Hello Texture", 1280U, 720U);

	Win32Application application(hInstance, &helloLighting, IMGuiHelloTexture::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}