/*
 * 范例05 - 陆地和波浪示例程序
 * Main.cpp
 */

#include "Application/Win32Application.h"
#include "DXWorkLandAndWavesWithLighting/LandAndWavesWithLighting.h"
#include "DXWorkLandAndWavesWithLighting/IMGuiLandAndWavesWithLighting.h"

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
	LandAndWavesWithLighting landAndWavesWithLighting(L"D3D12 Land And Waves With Lighting", 1280U, 720U);

	Win32Application application(hInstance, &landAndWavesWithLighting, IMGuiLandAndWavesWithLighting::GetInstance());
	application.InitializeMainWindow();

	return application.Run();
}