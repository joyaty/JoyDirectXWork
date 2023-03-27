/**
 * 范例10 - 几何着色器练习项目
 * Main.cpp
 **/

#include "Application/Win32Application.h"
#include "DXWorkGeometryShader/DXHelloGeometryShader.h"
#include "DXWorkGeometryShader/IMGuiHelloGeometryShader.h"

 /// <summary>
 /// 范例10程序入口
 /// </summary>
 /// <param name="hInstance"></param>
 /// <param name="hPrevInstance"></param>
 /// <param name="lpCmdLine"></param>
 /// <param name="nCmdSize"></param>
 /// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdSize)
{
	DXHelloGeometryShader helloGeometryShader(L"D3d12 Hello Geometry Shader", 1280U, 720U);
	Win32Application application(hInstance, &helloGeometryShader, IMGuiHelloGeometryShader::GetInstance());
	application.InitializeMainWindow();
	return application.Run();
}