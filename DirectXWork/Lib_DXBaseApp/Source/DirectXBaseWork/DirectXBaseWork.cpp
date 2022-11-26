/**
 * DirectXBaseWork.cpp
 * 初始化DirectX
 */

#include "stdafx.h"
#include "DirectXBaseWork.h"
#include "DirectXBaseWork/DXWorkHelper.h"
#include <iostream>

using namespace Microsoft::WRL;

DirectXBaseWork::DirectXBaseWork(std::wstring title, UINT width, UINT height)
	: m_Title(title)
	, m_Width(width)
	, m_Height(height)
	, m_UseWarpDevice(false)
{
	WCHAR assetPath[512];
	GetAssetsPath(assetPath, _countof(assetPath));
	m_AssetPath = assetPath;
	size_t rootFolderIndex = m_AssetPath.find(L"\\Build", 0);
	m_AssetPath = m_AssetPath.substr(0, rootFolderIndex);

	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
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

bool DirectXBaseWork::Initialize(HWND hWnd)
{
	m_Hwnd = hWnd;
	CreateD3D12Device();
	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
	OnResize(m_Width, m_Height);

	OnInit();
	return true;
}

void DirectXBaseWork::Terminate()
{
	OnDestroy();
	// 确保退出时，GPU执行完所有指令，避免退出释放资源，GPU仍执行命令，引用到被释放的资源报错
	FlushCommandQueue();
	CloseHandle(m_FenceEvent);
}

void DirectXBaseWork::OnResize(UINT width, UINT height)
{
	// 等待指令队列执行完毕
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 清理渲染目标缓冲区资源
	for (int i = kFrameBufferCount - 1; i >= 0; --i)
	{
		if (m_RenderTargets[i] != nullptr)
		{
			m_RenderTargets[i].Reset();
		}
	}
	// 清理深度模板缓冲区资源
	if (m_DepthStencilBuffer != nullptr)
	{
		m_DepthStencilBuffer.Reset();
	}
	// 更新窗口尺寸
	m_Width = width;
	m_Height = height;
	// 重建交换链
	m_SwapChain->ResizeBuffers(kFrameBufferCount, m_Width, m_Height, m_BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	// 重建渲染缓冲区视图
	CreateRenderTargetView();
	// 重建深度模板缓冲区视图
	CreateDepthStencilView();
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// 更新视窗
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.Width = static_cast<float>(m_Width);
	m_Viewport.Height = static_cast<float>(m_Height);

	// 更新裁剪区域
	m_ScissorRect = { 0, 0, static_cast<int>(m_Width), static_cast<int>(m_Height) };
}

void DirectXBaseWork::Update(float deltaTime, float totalTime)
{
	OnUpdate(deltaTime, totalTime);
}

void DirectXBaseWork::Render()
{
	OnRender();
	// 切换渲染目标缓冲区索引
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
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
		ThrowIfFailed(D3D12CreateDevice(pDXGIAdapter.Get()	// 指定使用的显示适配器，nullptr则为默认显示适配器
			, D3D_FEATURE_LEVEL_11_0						// 功能级别
			, IID_PPV_ARGS(m_Device.GetAddressOf())));		// COMID
		// 获取当前使用的显示适配器描述
		pDXGIAdapter->GetDesc(&m_AdapterDesc);
	}
	else
	{
		// 使用硬件设备
		ComPtr<IDXGIAdapter1> pDXGIAdapter{};
		GetHardwardAdapter(m_DXGIFactory.Get(), pDXGIAdapter.GetAddressOf());
		ThrowIfFailed(D3D12CreateDevice(pDXGIAdapter.Get()	// 指定使用的显示适配器，nullptr则为默认显示适配器
			, D3D_FEATURE_LEVEL_11_0						// 功能级别
			, IID_PPV_ARGS(m_Device.GetAddressOf())));		// COMID
		// 获取当前使用的显示适配器描述
		pDXGIAdapter->GetDesc(&m_AdapterDesc);
	}

	if (m_Device.Get() != nullptr)
	{
		// 检查是否支持4XMSAA，获取4XMSAA下的质量品质
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		msQualityLevels.Format = m_BackBufferFormat; // 需要与后台缓冲区格式一致
		ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m_4XMSAAQualityLevel = msQualityLevels.NumQualityLevels;

		// 获取RenderTargetView描述符大小
		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// 获取Depth/Stencil描述符大小
		m_DSVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		// 获取ConstantBufferView/ShaderResourceView/UnorderAccessView描述符大小
		m_CBVUAVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 创建围栏
		ThrowIfFailed(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));
		// 创建围栏触发事件
		m_FenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
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
	desc.Format = m_BackBufferFormat;	// 交换链缓冲区格式
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.SampleDesc.Count = m_Enable4XMSAA ? 4 : 1;			// 多重采样数量
	desc.SampleDesc.Quality = m_Enable4XMSAA ? (m_4XMSAAQualityLevel - 1) : 0;	// 多重采样质量级别
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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

void DirectXBaseWork::CreateRenderTargetView()
{
	// 获取RTV描述符堆的第一个描述符Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle{ m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < kFrameBufferCount; ++i)
	{
		// 从交换链中获取缓冲区资源
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_RenderTargets[i].GetAddressOf())));
		m_RenderTargetDesciptorHandles[i] = rtvDescriptorHandle;
		// 创建渲染目标缓冲区视图
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvDescriptorHandle);
		// handle偏移，获取下一个描述符handle
		rtvDescriptorHandle.Offset(1, m_RTVDescriptorSize);
	}
}

void DirectXBaseWork::CreateDepthStencilView()
{
	// 深度/模板缓冲区资源描述
	D3D12_RESOURCE_DESC dsvResourceDesc{};
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;						// 深度模板缓冲区本质上是一种2D纹理
	dsvResourceDesc.Alignment = 0;
	dsvResourceDesc.Width = m_Width;
	dsvResourceDesc.Height = m_Height;
	dsvResourceDesc.DepthOrArraySize = 1;
	dsvResourceDesc.MipLevels = 1;														// 深度模板缓冲区只能有一个MipLevels
	dsvResourceDesc.Format = m_DepthStencilFormat;										// 深度/模板缓冲区纹素格式
	dsvResourceDesc.SampleDesc.Count = m_Enable4XMSAA ? 4 : 1;							// 多重采样设置需要与渲染目标设置保持一致
	dsvResourceDesc.SampleDesc.Quality = m_Enable4XMSAA ? (m_4XMSAAQualityLevel - 1) : 0;		// 多重采样设置需要与渲染目标设置保持一致
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;					// 深度/模板缓冲区资源的杂项标记

	// 堆属性，GPU资源都在堆中，本质是具有特定属性的GPU显存块。ID3D12Device::CreateCommittedResource会创建一个资源和一个堆，并将资源提交到堆上。
	// 这里指定堆属性为默认堆，只有GPU可以读写
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	// 深度模板缓冲区清除值，选择合适的清除优化值，可以提高清除操作的执行速度
	CD3DX12_CLEAR_VALUE clearValue{ m_DepthStencilFormat, 1.f, 0 };
	// 创建深度/模板缓冲区资源
	ThrowIfFailed(m_Device->CreateCommittedResource(
		&heapProperties
		, D3D12_HEAP_FLAG_NONE
		, &dsvResourceDesc
		, D3D12_RESOURCE_STATE_COMMON
		, &clearValue
		, IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	// 获取深度/模板缓冲区描述符句柄
	m_DepthStencilDescriptorHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// 创建深度/模板缓冲区描述符
	m_Device->CreateDepthStencilView(
		m_DepthStencilBuffer.Get()
		, nullptr				// 深度/模板资源创建时指定了具体的格式，不是无格式资源，则创建深度模板缓冲图描述符时，描述符描述结构体可以为空
		, m_DepthStencilDescriptorHandle);

	// 将资源从初始状态转换为深度/模板缓冲区状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	));

	ThrowIfFailed(m_CommandList->Close());
	// 执行初始化指令
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();
}

void DirectXBaseWork::FlushCommandQueue()
{
	// 围栏值递增
	++m_FenceValue;
	// 通知GPU设置围栏值
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));
	// 在CPU端等待GPU，直到GPU执行完这个围栏值之前的命令
	if (m_Fence->GetCompletedValue() < m_FenceValue)
	{
		// 若GPU命中当前的围栏，即执行到Signal指令，修改了围栏值，则激发预定事件
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent));
		// 等待GPU命中围栏，激发事件
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
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