

#include "stdafx.h"
#include "D3D12HelloWindow.h"
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
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (m_UseWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	}
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
	// Describe and create the swap chain.
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
		m_CommandQueue.Get(),	// Swap chain needs the queue so that it can force a flush on it.
		Win32Application::GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));
	// This sample does not support fullscreen transitions.
	ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain.As(&m_SwapChain));
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = kFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)));
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resource.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Create a RTV for each frame.
		for (UINT n = 0; n < kFrameCount; ++n)
		{
			ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
			m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}
	}
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
}

void D3D12HelloWindow::LoadAsset()
{
	// Create the command list.
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
	// Command lists are created in the recording state, but there is nothing to record yet. the main loop expects it to be closed, so close it now.
	ThrowIfFailed(m_CommandList->Close());

	// Creae synchroniztion objects.
	{
		ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValue = 1;

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
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

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHanle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	// Record commands.
	const float clearColor[] = { 0.8f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHanle, clearColor, 0, nullptr);
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