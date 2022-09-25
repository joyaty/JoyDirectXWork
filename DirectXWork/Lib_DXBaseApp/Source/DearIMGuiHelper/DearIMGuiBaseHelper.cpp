/***
 * DearIMGui对接类CPP文件
 */

#include "stdafx.h"
#include "DearIMGuiBaseHelper.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

bool DearIMGuiBaseHelper::InitDearIMGui(HWND hwnd, ID3D12Device* pD3D12Device, int nFrameCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* pSrvDescHeap)
{
	// 创建DearIMGui上下文
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// 启用键盘控制
	//io.ConfigFlags != ImGuiConfigFlags_NavEnableGamepad;	// 启用手柄控制
	
	// 初始化DearIMGui主题颜色
	ImGui::StyleColorsDark();		// 黑色主题
	// ImGui::StyleColorsLight();	// 白色主题
	
	// 初始化平台和渲染API
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(pD3D12Device, nFrameCount, rtvFormat, pSrvDescHeap
		, pSrvDescHeap->GetCPUDescriptorHandleForHeapStart()
		, pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	// 加载字体
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	return true;
}

void DearIMGuiBaseHelper::TerminateIMGui()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DearIMGuiBaseHelper::DrawDearIMGuiWindow()
{
	// 开始DearIMGui渲染帧
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// 绘制自定义的DearIMGUI窗口
	OnDrawWindow();
	// 渲染
	ImGui::Render();
}

void DearIMGuiBaseHelper::PopulateDearIMGuiCommand(ID3D12GraphicsCommandList* pCommandList)
{
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
}