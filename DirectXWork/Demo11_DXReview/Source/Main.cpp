
#include "WinApplication.h"
#include "DXSampleForReview.h"
#include "IMGuiCtrlForReview.h"

/// <summary>
/// Demo11程序入口
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="pCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	DXSampleForReview sampleForReview(L"DX12 Review", 1280U, 720U);
	IMGuiCtrlForReview guiForReview{};
	WinApplication application(hInstance, &sampleForReview, &guiForReview);
	return application.Initialize() ? application.Run() : 1;
}