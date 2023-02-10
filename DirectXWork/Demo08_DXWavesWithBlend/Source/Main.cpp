/**
 * 范例08 - WavesWithBlend 混合练习程序
 * Main.cpp
 **/

#include "Application/Win32Application.h"
#include "DXWorkBlend/DXWavesWithBlend.h"
#include "DXWorkBlend/IMGuiWavesWithBlend.h"

/// <summary>
/// Win32入口Main函数
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdSize"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdSize)
{
	DXWavesWithBlend wavesWithBlend(L"D3D12 Hello Waves With Blend", 1280U, 720U);
	Win32Application application(hInstance, &wavesWithBlend, IMGuiWavesWithBlend::GetInstance());
	application.InitializeMainWindow();
	return application.Run();
}