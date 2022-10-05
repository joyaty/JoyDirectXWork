/***
 * DearIMGui对接类CPP文件
 */

#include "stdafx.h"
#include "DearIMGuiBaseHelper.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "DirectXBaseWork/DXWorkHelper.h"

bool DearIMGuiBaseHelper::InitDearIMGui(HWND hwnd, ID3D12Device* pD3D12Device, int nRTVCount, DXGI_FORMAT rtvFormat, DXGI_ADAPTER_DESC adapterDesc)
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
	
	// 创建DearIMGUI专属的ShaderResourceView
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.NodeMask = 0U;
	ThrowIfFailed(pD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SRVDescriptorHeap.GetAddressOf())));
	// 初始化平台和渲染API
	ImGui_ImplWin32_Init(hwnd);
	// 初始化DearIMGui DX12渲染
	ImGui_ImplDX12_Init(pD3D12Device, nRTVCount, rtvFormat, m_SRVDescriptorHeap.Get()
		, m_SRVDescriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart()
		, m_SRVDescriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart());

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

	m_AdapterName = adapterDesc.Description;
	m_VideoMemorySize = static_cast<float>(adapterDesc.DedicatedVideoMemory) / 1024.f / 1024.f;
	m_SystemMemorySize = static_cast<float>(adapterDesc.SharedSystemMemory) / 1024.f / 1024.f;

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

void DearIMGuiBaseHelper::DrawAdapterInfo()
{
	ImGui::Text("GPU: %ws", m_AdapterName.c_str());
	ImGui::Text("Video Memory: %.2f MB", m_VideoMemorySize);
	ImGui::Text("System Memory: %.2f MB", m_SystemMemorySize);
	ImGui::Separator();
}

void DearIMGuiBaseHelper::PopulateDearIMGuiCommand(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->SetDescriptorHeaps(1, m_SRVDescriptorHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
}