/**
 * DirectXBaseWork.cpp
 * 初始化DirectX
 */

#include "stdafx.h"
#include "DirectXBaseWork.h"
#include "DXWork/DXWorkHelper.h"

using namespace Microsoft::WRL;

DirectXBaseWork::DirectXBaseWork(std::wstring title, UINT width, UINT height)
	: m_Title(title)
	, m_Width(width)
	, m_Height(height)
	, m_UseWarpDevice(false)
{
}

DirectXBaseWork::~DirectXBaseWork()
{
}

// _Use_decl_annotations_
void DirectXBaseWork::GetHardwardAdapter(IDXGIFactory1* pDXGIFactory, IDXGIAdapter1** ppTargetDXGIAdapter, bool requestHightPerformanceAdapter/* = false*/)
{
	*ppTargetDXGIAdapter = nullptr;
	ComPtr<IDXGIFactory6> pDXGIFactory6{};
	ComPtr<IDXGIAdapter1> pCurrentAdapter{};
	// 支持IDXGIFactory6，使用指定GPU偏好的枚举接口
	if (SUCCEEDED(pDXGIFactory->QueryInterface(IID_PPV_ARGS(pDXGIFactory6.GetAddressOf()))))
	{
		for (UINT nAdapterIndex = 0;
			SUCCEEDED(pDXGIFactory6->EnumAdapterByGpuPreference(nAdapterIndex
				, requestHightPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED // 是否需要指定高性能GPU标记
				, IID_PPV_ARGS(pCurrentAdapter.GetAddressOf())));
			++nAdapterIndex)
		{
			// 获取当前显示适配器描述
			DXGI_ADAPTER_DESC1 desc;
			pCurrentAdapter->GetDesc1(&desc);
			// 不选择软件显示适配器，要使用，需要在命令行使用"/warp"
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			// 尝试(不对返回设备赋值)创建D3D12设备，成功直接返回
			if (SUCCEEDED(D3D12CreateDevice(pCurrentAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	if (pCurrentAdapter.Get() == nullptr)
	{
		for (UINT nAdapterIndex = 0;
			SUCCEEDED(pDXGIFactory->EnumAdapters1(nAdapterIndex, pCurrentAdapter.GetAddressOf()));
			++nAdapterIndex)
		{
			// 获取当前显示适配器描述
			DXGI_ADAPTER_DESC1 desc;
			pCurrentAdapter->GetDesc1(&desc);
			// 不选择软件显示适配器，要使用，需要在命令行使用"/warp"
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			// 尝试(不对返回设备赋值)创建D3D12设备，成功直接返回
			if (SUCCEEDED(D3D12CreateDevice(pCurrentAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	// 当前选取的显示适配器作为目标显示适配器
	if (pCurrentAdapter.Get() != nullptr)
	{
		*ppTargetDXGIAdapter = pCurrentAdapter.Detach();
	}
}

void DirectXBaseWork::Initialize(HWND hWnd)
{
	m_Hwnd = hWnd;
	CreateD3D12Device();
	CreateCommandObjects();
	CreateSwapChain();
}

void DirectXBaseWork::CreateD3D12Device()
{
	UINT createDXGIFactoryFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	// 启用D3D12调试层
	ComPtr<ID3D12Debug> pDebugController{};
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf()))))
	{
		pDebugController->EnableDebugLayer();
		createDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif // defined(_DEBUG) || defined(DEBUG)

	// 创建IDXGIFactory
	ThrowIfFailed(CreateDXGIFactory2(createDXGIFactoryFlags, IID_PPV_ARGS(m_DXGIFactory.GetAddressOf())));
	// 获取显示适配器，使用显示适配器创建D3D12设备
	if (m_UseWarpDevice)
	{
		// 使用WARP设备，WARP = Windows Advanced Rasterization Platform（Windows高级光栅化平台），可将它当做是不依赖于任何硬件图形适配器的纯软件渲染器
		// GPU硬件不能工作时，可以使用WARP
		ComPtr<IDXGIAdapter> pDXGIAdapter{};
		ThrowIfFailed(m_DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(pDXGIAdapter.GetAddressOf())));
		D3D12CreateDevice(pDXGIAdapter.Get()				// 指定使用的显示适配器，nullptr则为默认显示适配器
			, D3D_FEATURE_LEVEL_11_0						// 功能级别
			, IID_PPV_ARGS(m_Device.GetAddressOf()));		// COMID
	}
	else
	{
		// 使用硬件设备
		ComPtr<IDXGIAdapter1> pDXGIAdapter{};
		GetHardwardAdapter(m_DXGIFactory.Get(), pDXGIAdapter.GetAddressOf());
		D3D12CreateDevice(pDXGIAdapter.Get()							// 指定使用的显示适配器，nullptr则为默认显示适配器
			, D3D_FEATURE_LEVEL_11_0						// 功能级别
			, IID_PPV_ARGS(m_Device.GetAddressOf()));		// COMID
	}

	if (m_Device.Get() != nullptr)
	{
		// 检查是否支持4XMSAA，获取4XMSAA下的质量品质
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		msQualityLevels.Format = m_BackbufferFormat; // 需要与后台缓冲区格式一致
		ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m_4XMSAAQualityLevel = msQualityLevels.NumQualityLevels;

		// 获取RenderTargetView描述符大小
		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// 获取Depth/Stencil描述符大小
		m_DSVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		// 获取ConstantBufferView/ShaderResourceView/UnorderAccessView描述符大小
		m_CBVUAVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void DirectXBaseWork::CreateCommandObjects()
{
	// 创建命令队列
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// GPU可直接执行的命令列表类型
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	// 创建命令分配器
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	// 创建命令列表
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	// 命令列表创建后默认处于记录状态，需要关闭
	ThrowIfFailed(m_CommandList->Close());
}

void DirectXBaseWork::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.BufferCount = kFrameBufferCount; // 缓冲区数量
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// 作为渲染目标输出
	desc.Width = m_Width;				// 交换链缓冲区宽度
	desc.Height = m_Height;				// 交换链缓冲区高度
	desc.Format = m_BackbufferFormat;	// 交换链缓冲区格式
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.SampleDesc.Count = m_Use4XMSAA ? 4 : 1;			// 多重采样数量
	desc.SampleDesc.Quality = m_Use4XMSAA ? m_4XMSAAQualityLevel : 0;	// 多重采样质量级别
	// 使用描述创建交换链
	ComPtr<IDXGISwapChain1> pSwapChain{};
	ThrowIfFailed(m_DXGIFactory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(),	// 交换链需要通过命令队列刷新，DX11以及之前，此参数为ID3D12Device指针，DX12变更为ID3D12CommandQueue指针，不可为空
		m_Hwnd,					// 窗口句柄，不可为空
		&desc,					// 交换链描述，不可为空
		nullptr,				// 全屏交换链描述，可选
		nullptr,				// 指定输出的显示设备，可选
		pSwapChain.GetAddressOf()
	));
	// 不需要全屏模式，防止ALT+ENTER组合键进入全屏
	ThrowIfFailed(m_DXGIFactory->MakeWindowAssociation(m_Hwnd, DXGI_MWA_NO_ALT_ENTER));
	// 保存交换链，并且记录当前的后台缓冲区索引
	ThrowIfFailed(pSwapChain.As(&m_SwapChain));
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void DirectXBaseWork::CreateDescriptorHeaps()
{
	// 创建RTV描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorDesc{};
	rtvDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorDesc.NumDescriptors = kFrameBufferCount;		// 两个缓冲区组成渲染目标视图交换链
	rtvDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvDescriptorDesc, IID_PPV_ARGS(m_RTVDescriptorHeap.GetAddressOf())));
	// 创建DSV描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorDesc{};
	dsvDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorDesc.NumDescriptors = 1;				// 只需要一个深度/模板缓冲区
	dsvDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvDescriptorDesc, IID_PPV_ARGS(m_DSVDescriptorHeap.GetAddressOf())));
}

// _Use_decl_annotations_
void DirectXBaseWork::ParseCommandLineArgs(WCHAR* argv[], int argc)
{
	for (int i = 1; i < argc; ++i)
	{
		if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0
			|| _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
		{
			m_UseWarpDevice = true;
			m_Title += L" (WARP)";
		}
	}
}


