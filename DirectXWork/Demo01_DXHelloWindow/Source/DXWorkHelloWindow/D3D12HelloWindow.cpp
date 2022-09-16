

#include "stdafx.h"
#include "D3D12HelloWindow.h"
#include "IMGuiHelloWindow.h"
#include "DXWork/DXWorkHelper.h"
#include "Application/Win32Application.h"

D3D12HelloWindow::D3D12HelloWindow(UINT width, UINT height, std::wstring name)
	: DXBaseWork(width, height, name)
	, m_FrameIndex(0)
	, m_RtvDescriptorSize(0)
{
}

void D3D12HelloWindow::OnInit()
{
	LoadPipeline();
	LoadAsset();
}

void D3D12HelloWindow::OnUpdate()
{
}

void D3D12HelloWindow::OnRender()
{
	if (IMGuiHelloWindow::GetInstance() != nullptr)
	{
		IMGuiHelloWindow::GetInstance()->DrawDearIMGuiWindow();
	}
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();
	// Execute the command list.
	ID3D12CommandList* ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	// Present the frame.
	ThrowIfFailed(m_SwapChain->Present(1, 0));

	WaitForPreviousFrame();
}

void D3D12HelloWindow::OnDestroy()
{
	// Ensure that the GPU is no longer referencing resource that are about to be cleaned up by the destructor.
	WaitForPreviousFrame();
	CloseHandle(m_FenceEvent);
}

void D3D12HelloWindow::LoadPipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tool "option feature").
	// NOTE: Enabling the debug layger after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif
	// 创建设备
	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf())));
	if (m_UseWarpDevice)
	{
		// 使用WARP设备，WARP = Windows Advanced Rasterization Platform（Windows高级光栅化平台），可将它当做是不依赖于任何硬件图形适配器的纯软件渲染器
		// GPU硬件不能工作时，可以使用WARP
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf())));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));
	}
	else
	{
		// 使用硬件设备
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));
	}
	// 创建命令队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	// 创建前台缓冲区和后台缓冲区交换链
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = kFrameCount;
	swapChainDesc.Width = m_Width;
	swapChainDesc.Height = m_Height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(),	// 交换链需要通过命令队列刷新，因此这里需要刷新交换链的命令队列
		Win32Application::GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));
	// This sample does not support fullscreen transitions.
	ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain.As(&m_SwapChain));
	// 获取当前的后台缓冲区索引
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// 创建RenderTargetView的描述符堆
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = kFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	// 创建ShaderResourceView的描述符堆
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SrvHeap.GetAddressOf())));
	}

	// 创建RenderTargetView描述符，将描述符绑定到交换链的渲染缓冲区上
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Create a RTV for each frame.
		for (UINT n = 0; n < kFrameCount; ++n)
		{
			m_MainRenderTargetDescriptor[n] = rtvHandle;
			ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(m_RenderTargets[n].GetAddressOf())));
			m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}
	}
	// 创建命令分配器
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	// 通过命令分配器创建命令列表
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	// 命令列表创建结束后处于记录命令状态，此时不需要记录渲染命令，因此关闭
	ThrowIfFailed(m_CommandList->Close());

	// 创建Fence同步对象
	{
		ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));
		m_FenceValue = 1;

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

void D3D12HelloWindow::LoadAsset()
{
	
}

void D3D12HelloWindow::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated command lists have finished execution on th GPU.
	// apps should use fence to determine GPU excution progress.
	ThrowIfFailed(m_CommandAllocator->Reset());
	// However, when ExecuteCommandList() is called on a particular command list, that command list can the be reset at any time and must be before re-recording.
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));
	// Indicated that the back buffer will be used as a render target.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	// Record commands.
	ImVec4 backgroudColor = IMGuiHelloWindow::GetInstance()->GetBackColor();
	const float clearColor[] = { backgroudColor.x, backgroudColor.y, backgroudColor.z, backgroudColor.w };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	m_CommandList->SetDescriptorHeaps(1, m_SrvHeap.GetAddressOf());
	// 提交DearIMGui的渲染命令
	if (IMGuiHelloWindow::GetInstance() != nullptr)
	{
		// m_CommandList->OMSetRenderTargets(1, &rtvHanle, FALSE, NULL);
		// m_CommandList->SetDescriptorHeaps(1, &m_SrvHeap);
		IMGuiHelloWindow::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}

	// Indicated that the back buffer will now be used to present.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}

void D3D12HelloWindow::WaitForPreviousFrame()
{
	// Signal and increment the fence value.
	const UINT64 fence = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fence));
	++m_FenceValue;
	// Wait until the previouse frame is finished.
	if (m_Fence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fence, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}