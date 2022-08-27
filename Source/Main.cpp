/**
 * DirectX12学习工程入口文件
 */

#include "Application/Win32Application.h"

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
	return Win32Application::Run(hInstance, nCmdShow);
}